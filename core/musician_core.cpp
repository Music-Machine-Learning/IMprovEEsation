/*****************************************************************************/
/* Musician's core facilities                                                */
/* This header is a part of the IMprovEEsation suite.                        */
/*                                                                           */
/* Copyright (C) 2014                                                        */
/* Federico Montori, Marco Melletti, Davide Berardi, Matteo Martelli         */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or any later version.                                     */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,*/
/* USA.                                                                      */
/*****************************************************************************/

#include <improveesation/configuration.h>
#include <improveesation/communication.h>
#include <improveesation/structs.h>
#include <improveesation/db.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* TODO refactor for all global fields, 
   what about a struct to contain all of them? */
struct musician_fields_s{
	int octave_min;
	int octave_max;
	int prev_octave;
	int play_chords;
};

static struct musician_fields_s mfields;

int print_semiquaver(struct semiquaver_s *sq);

int musician_init(PGconn **dbh, int coupling, int instrument, 
			int soloist, int musician_id, int play_chords)
{
	int res;
	struct rc_conf_s conf;

	*dbh = NULL;
	
	if (load_conf(DEFAULT_RC_PATH, &conf) < 4) {
		fprintf(stderr, "error while loading configuration (%s)\n",
			strerror(errno));
		return 1;
	}

	*dbh = db_connect(conf.db_host,
			conf.db_name,
			conf.db_user,
			conf.db_passwd);

	
	if (*dbh == NULL)
		return -1;

	mfields.prev_octave = -1;
	mfields.play_chords = play_chords;

	res = get_range(*dbh, instrument, &(mfields.octave_min), 
			&(mfields.octave_max));
	if (res != 0 || (mfields.octave_max - mfields.octave_min) > MIDI_NOCTAVES)
		fprintf(stderr, "initialization failed\n");

	return 0;
}

/* Decide a note scanning an array of 13 probability values (one is the rest) */
int decide_note(float *pnote)
{
	/* Create a prefix sums array starting from the pnote array.
	 * Then get a random number and scan the prefix sums array until a 
	 * value lesser or equal than the random number is found. 
	 * The index of the found value will be the index of the 
	 * decided note in the pnote array. */
	int i;
	float r, pref[PROB_ARR_SIZE];
	
	pref[0] = pnote[0];
	for (i = 1; i < PROB_ARR_SIZE; i++) 
		pref[i] = pref[i - 1] + pnote[i];
	
	r = (float)rand() / (float)(RAND_MAX);
	for (i = 0; r > pref[i]; i++);
	
	return i;
}

int decide_octave(int octave_min, int octave_max)
{
	int r, oloop_iter, ojump, octave;

	r = rand();
	oloop_iter = ojump = 0;
	
	ojump = r % (OCTAVE_MAX_JUMP * 2) - OCTAVE_MAX_JUMP ;
	
	if (mfields.prev_octave == -1)
		octave = (r % (octave_max - octave_min)) + octave_min;
	else
		octave = mfields.prev_octave;
	
	octave += ojump; 
	
	while (octave > octave_max) {
		oloop_iter++;
		octave--;
	}
	if (oloop_iter > OCTAVE_MAX_JUMP){
		fprintf(stderr, "Warning: something not nice in "
				"note_to_midi oloop_iter: %d\n", 
				oloop_iter);
	}
	oloop_iter = 0;
	while (octave < octave_min) {
		oloop_iter++;
		octave++;
	}
	
	if (oloop_iter > OCTAVE_MAX_JUMP){
		fprintf(stderr, "Warning: something not nice in "
				"note_to_midi oloop_iter: %d\n", 
				oloop_iter);
	}

	printf("octave_jump %d octave %d\n", ojump, octave);

	mfields.prev_octave = octave;
	
	return octave;
}

int note_to_midi(int note_idx, int key_note)
{
	int midi, octave;


	if (note_idx == 0) {
		midi = MIDI_REST_NOTE;
	} else {
		octave = decide_octave(mfields.octave_min, mfields.octave_max);
		midi = (MIDI_FIRST_NOTE + (NSEMITONES * octave)) + 
				key_note + note_idx;
		if (midi < MIDI_FIRST_NOTE)
			midi += NSEMITONES;
		else if (midi > MIDI_LAST_NOTE)
			midi -= NSEMITONES;

		if (midi < MIDI_FIRST_NOTE || midi > MIDI_LAST_NOTE){
			fprintf(stderr, "note out of boundaries\n");
			return -1;
		}
	}

	return midi;
}

int fill_chord_notes(struct notes_s *chord, struct measure_s *minfo, int q_idx)
{
	int chord_note, chord_mode, key_note, i, max, n_done, 
	    octave, nidx, chord_width;
	int chord_mode_arr[NSEMITONES], tmp_notes[MAX_CHORD_SIZE];

	key_note = minfo->tonal_zones[q_idx].note;
	chord_note = minfo->chords[q_idx].note;
	chord_mode = minfo->chords[q_idx].mode;	
	chord_width = 1;


	for (i = 0, n_done = 0; i < NSEMITONES; i++) {
		nidx = chord_mode >> i;
		if (nidx & 1) {
			chord_mode_arr[n_done] = i;
			n_done++;
		}
	}
	chord_mode_arr[n_done] = -1; /* list of notes from the original chord mask */

	/* Randomly decide the size of the chord, but it shouln't be bigger than
	   the given chord mask or a prechoosen max size */
	max = MAX_CHORD_SIZE;
	
	if (n_done < MAX_CHORD_SIZE)
		max = n_done;

	chord->chord_size = rand() % (max - MIN_CHORD_SIZE) + MIN_CHORD_SIZE;

	octave = decide_octave(mfields.octave_min, mfields.octave_max);
	
	/* From the set of the given chords note decide the note to put in the chord */
	for (i = 0; i < chord->chord_size; i++) {
		chord->notes[i] = 0;
		tmp_notes[i] = chord_mode_arr[rand() % n_done];
		if (i > 0) {
			if (tmp_notes[i] < tmp_notes[i - 1]) 
				chord->notes[i] = (NSEMITONES - tmp_notes[i - 1]);
			chord->notes[i] += chord->notes[i - 1] + tmp_notes[i];
			chord->notes[i] += key_note + chord_note + MIDI_FIRST_NOTE 
				+ (octave * NSEMITONES);

		} else { 
			chord->notes[i] = tmp_notes[i];
		}
	}
	
	int max_dist = 0;
	int move_direction = 0;
	int move_octave = 0;
	for (i = 0; i < chord->chord_size; i++) {
		int dist = 0;
		if (chord->notes[i] > MIDI_LAST_NOTE) {
			move_direction = 1;
			dist = chord->notes[i] - MIDI_LAST_NOTE;
			if (dist > max_dist)
				max_dist = dist;
		} else if (chord->notes[i] < MIDI_FIRST_NOTE) {
			move_direction = -1;
			dist = MIDI_FIRST_NOTE - chord->notes[i];
			if (dist > max_dist)
				max_dist = dist;
		}
	}
	
	if (max_dist > 0){
		move_octave = max_dist / NSEMITONES + 1;
		if (move_direction == 1)
			octave -= move_octave;
		else if (move_direction == -1)
			octave += move_octave;
	
		for (i = 0; i < chord->chord_size; i++) {
			if (move_direction == 1)
				chord->notes[i] -= NSEMITONES * move_octave;
			else if (move_direction == -1)
				chord->notes[i] += NSEMITONES * move_octave;
		}
	}
	
	mfields.prev_octave = octave;

	if (n_done > MAX_CHORD_SIZE || chord->chord_size > MAX_CHORD_SIZE || 
			n_done < MIN_CHORD_SIZE || chord->chord_size < MIN_CHORD_SIZE) {
		fprintf(stderr, "Something wrong in fill_chord_notes\n");
		return -1;
	}
}

inline void assign_new_notes(struct play_measure_s *pm, struct notes_s *new_notes,  
		int *ntcount)
{
	printf("new notes assignment\n");
	new_notes->id = *ntcount;
	new_notes->triplets = 0;
	new_notes->tempo = 1;
	pm->measure[*ntcount] = *new_notes;
	(*ntcount)++;
}

int continue_last_note(struct play_measure_s *pm, struct play_measure_s *prev_pm, 
		struct notes_s *new_notes,  int *ntcount)
{
	if (*ntcount == 0) {
		memcpy(new_notes->notes, prev_pm->measure[prev_pm->size - 1].notes, 
				MAX_CHORD_SIZE);
			
		pm->unchanged_fst = 1; 
		printf("fisrt note is unchanged\n");
		assign_new_notes(pm, new_notes, ntcount);
	} else if (*ntcount > 0) {
		pm->measure[(*ntcount)-1].tempo++;
	} else {
		fprintf(stderr, "invalid ntcount: %d\n", *ntcount);
		return -1;
	}
}


/* Compose the notes of a quarter scanning the semiquavers retreived from 
 * the database. The number of notes inside the quarter is returned. */
int compose_quarter(struct play_measure_s *pm, struct play_measure_s *prev_pm,
		struct measure_s *minfo, int q_idx, int max_sqcount,
		struct semiquaver_s **sqs, int sq_size, int ntcount)
{
	float rnd;
	int idx, i, s, ret, key_note;
	struct notes_s new_notes;

	key_note = minfo->tonal_zones[q_idx].note;
	
	printf("composing quarter. ntcount: %d\n", ntcount);
	for (s = 0; s < sq_size; s++)
		print_semiquaver(sqs[s]);

	/* Compose semiquavers until they are present in the array of semiquavers
	 * that have chance to be played or until the maximum of semiquavers 
	 * in the quarter is reached */
	for (s = 0, i = 0; s < sq_size && i < max_sqcount; i++) {
		memset(&new_notes, 0, sizeof(new_notes));
		new_notes.chord_size = 1;
		
		if (s >= max_sqcount) {
			fprintf(stderr, "sq count too big\n");
			return -1;
		}

		rnd = (float)rand() / ((float)(RAND_MAX) / 1.0);
	
		printf("sq_size: %d, ntcount %d, (i %d s %d spos %d) \n", 
				sq_size, ntcount, i, s, sqs[s]->position);

		/* If the current sq doesn't exists in the db for the picked 
		 * quarter or if it exists but does not pass the changing prob, 
		 * we increment the tempo of the previous note, 
		 * event if it's in the previous measure */
		if ((sqs[s]->position != i || sqs[s]->pchange < rnd) &&
				(prev_pm->measure != NULL || ntcount > 0)) {
			printf("continuing last note\n");
			ret = continue_last_note(pm, prev_pm, &new_notes, &ntcount);
			if (ret == -1){
				fprintf(stderr, "error in continue_last_note\n");
				return -1;
			}
		} else {
		
			idx = decide_note(sqs[s]->pnote);
			
			if (!mfields.play_chords) {
				new_notes.notes[0] = note_to_midi(idx, key_note);
				if (new_notes.notes[0] == -1)
					return -1;
				printf("new single note added\n");
			} else  {
				/* If it's a rest I don't care about the chord notes */
				if (idx == 0)
					new_notes.notes[0] = MIDI_REST_NOTE;
				else 
					fill_chord_notes(&new_notes, minfo, q_idx); 
				printf("chord added.\n");
			}
			assign_new_notes(pm, &new_notes, &ntcount);
		} 	

		/* Increment the s counter only if the current sq position
		   matched with the current sq in the quarter */
		if (sqs[s]->position == i)
			s++;
	}
	
	return ntcount;
}

int count_semiquavers(struct tempo_s time_signature)
{
	/* TODO: calculate the number of semiquavers according to the 
	   time signature */
	return 16;
}

/* Split the tags string into 3 strings contained in the "results" array which 
 * should be already allocated */
int split_tags(char *tags_str, char **tags)
{

	char *tmp;
	const char *del = ";";	
	int i = 0;

	asprintf(&tmp, "%s", tags_str);

	tags[0] = strtok(tmp, del);
	tags[1] = strtok(NULL, del);
	tags[2] = strtok(NULL, del);
	
	if (!tags[0] || !tags[1] || !tags[2]){
		fprintf(stderr, "%s\n", "NULL pointer after strtok");
		return -1;
	}

	return 0;
}

int fill_quarter_args(char **args, struct measure_s *minfo, int myid,
			int soloist, int q_idx)
{
	int quarter_solo;
	char *qpos, *qinstr, *qchord_note, *qchorde_mode, *qdyna, *qmood,
		*qgenre, *qscale;
	char *qtags[3]; //TODO check for real how many tags are there

	if (split_tags(minfo->tags.payload, qtags) == -1){
		fprintf(stderr, "%s\n", "Error in split_tags");
		return -1;
	}

	asprintf(&args[0], "%d", q_idx); //pos
	asprintf(&args[1], "%d", myid & 0xff); //instrument class
	asprintf(&args[2], "%d", minfo->chords[q_idx].note); //chord_note
	asprintf(&args[3], "%d", minfo->chords[q_idx].mode); //chord_mode mask
	asprintf(&args[4], "%s", qtags[0]); //tag_dynamic
	asprintf(&args[5], "%s", qtags[1]); //tag_genre
	asprintf(&args[6], "%s", qtags[2]); //tag_mood
	asprintf(&args[7], "%d", minfo->tonal_zones[q_idx].scale); //scale mask
	
	quarter_solo = 0;
	if (soloist == 1 && myid == minfo->soloist_id)
		quarter_solo = 1;
	
	asprintf(&args[8], "%d", quarter_solo);
	return 0;
}

/* Compose a measure looking into the measure hints provided by the director. */
int compose_measure(struct play_measure_s *pm, struct play_measure_s *prev_pm, 
		struct measure_s *minfo, int myid, int soloist, PGconn *dbh)
{

	int q, i, max_sqcount, ntcount, res, q_size, sq_size, key_note, q_id;
	char *qargs[QUARTER_QUERY_ARGS];
	int *qids;
	struct semiquaver_s **sqs;

	q_size = ntcount = 0;

	max_sqcount = count_semiquavers(minfo->tempo);
	
	/* Allocates the array of notes with the max count of notes as size.
	 * It will be truncated later if the notes are lesser than the max. */
	pm->measure = (struct notes_s *)calloc((size_t)max_sqcount,
			sizeof(struct notes_s));
	
	/* First retrieve the quarters that may match what the director wants.
	 * For each quarter look in the DB for the corrisponding semiquavers
	 * and for each of them decide to change (or not) the note and in case
	 * of changing decide which note to choose with the decision function 
	 * which is based on the probabilities array scan. */
	for (q = 0; q < max_sqcount/4; q++) {
		res = fill_quarter_args(qargs, minfo, myid, soloist, q);
		if (res == -1)
			return -1;
		
		i = 0;
		while (q_size == 0 && i < QUARTER_QUERY_ARGS)
			q_size = get_quarters(dbh, qargs, minfo->prioargs, 
					i++, &qids);
		if (q_size <= 0){
			fprintf(stderr, "No quarters found\n");
			return -1;
		}

		/* Randomly choose one quarter */
		q_id = rand() % q_size;
		printf("quarter: %d\n", qids[q_id]);
		sq_size = get_semiquavers(dbh, qids[q_id], &sqs);
		if (sq_size <= 0) {
			fprintf(stderr, "No semiquavers found\n");
			return -1;
		} else if (sq_size > max_sqcount) {
			fprintf(stderr, "Too many semiquavers in one quarter\n");
			return -1;	
		}

		/* Be creative with the current quarter */
		ntcount = compose_quarter(pm, prev_pm, minfo, q, max_sqcount,
				sqs, sq_size, ntcount);
		if (ntcount == -1) {
			fprintf(stderr, "Error in quarter composition\n");
			return -1;
		}
	}

	/* Re-alloc the array of notes according to the notes count */
	if (ntcount < max_sqcount){
		printf("going to realloc old_size %d, new_size %d \n",
				max_sqcount, ntcount);
		pm->measure = (struct notes_s *)realloc((void *)pm->measure, 
				sizeof(struct notes_s) * (size_t)ntcount);
		if (pm->measure == NULL){
			fprintf(stderr, "Realloc error: %s\n", strerror(errno));
			
			return -1;
		}
		pm->size = ntcount;
	}
	return 0;
}

/* Just a debug function that prints the values of a semiquaver structure */
int print_semiquaver(struct semiquaver_s *sq)
{
	int i;
	printf("--semiquaver--\n");
	printf("quarter %d pos %d vmin %d vmax %d\n"
		"pchange %.2f pchange_3qrt %.2f pchange_3qvr %.2f pchange_3smq "
		"%.2f\npnotes {", sq->quarter, sq->position, sq->velocity_min, 
		sq->velocity_max, sq->pchange, sq->pchange_3qrt, sq->pchange_3qvr,
		sq->pchange_3smq);
	
	for (i = 0; i < 13; i++){
		printf("%.2f", sq->pnote[i]);
		if (i != 12)
			printf(", ");
	}
	printf("}\n----\n");
	return 0;
}

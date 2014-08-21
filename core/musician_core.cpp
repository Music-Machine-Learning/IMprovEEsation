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

#include <improveesation/musician_core.h>
#include <improveesation/communication.h>
#include <improveesation/db.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

int musician_init(PGconn **dbh)
{
	*dbh = NULL;
	
	/* TODO: some config file for these parameters */
	*dbh = db_connect("griffin.dberardi.eu",
			"improveesation_experimental_testes",
			"read_only",
			"testiamo123");
	
	if (*dbh == NULL)
		return -1;
	
	return 0;
}

/* Decide a note scanning an array of 13 probability values (one is the rest) */
int decide_note(float *pnote)
{
	int i, max_idx;
	float rnd, ival, max_ival;

	max_idx = max_ival = -1;
	
	/* For each note probability launch a random number, if the note prob
	 * is greater than the random number then the note pass. 
	 * For those notes that passed the random barrier, find the note which
	 * the interval between its prob value and the its random is the maximum 
	 */
	for (i = 0; i < 12; i < i++) {
		rnd = (float)rand() / ((float)(RAND_MAX) / 1.0);
		if (pnote[i] >= rnd) {
			ival = pnote[i] - rnd;
			if (ival > max_ival) {
				max_idx = i;
				max_ival = ival;
			}
		}
	}
	return max_idx;
}

int note_to_midi(int note_idx, int key_note)
{
	int midi, octave;

	octave = rand() % MIDI_NOCTAVES; //TODO: decide a policy

	if (note_idx == 0)
		midi = MIDI_REST_NOTE; //A rest;
	else 
		midi = (MIDI_FIRST_NOTE + (NSEMITONES * octave)) + 
				key_note + note_idx;
	
	return midi;
}

/* Assign the composed note into the given structure
   and return its length as the number of semiquavers */
int compose_note(struct notes_s *snote, int id)
{
	snote->note = 60;
	snote->tempo = 2;
	snote->id = id;
	snote->triplets = 0;
	return 0;
}

int count_semiquavers(struct tempo_s time_signature)
{
	/* TODO: calculate the number of semiquavers according to the 
	   time signature */
	return 16;
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

int fill_quarter_args(char **args, struct measure_s *minfo, int instrument,
			int q_idx)
{
	char *qpos, *qinstr, *qchord_note, *qchorde_mode, *qdyna, *qmood,
		*qgenre, *qscale;
	char *qtags[3]; //TODO check for real how many tags are there
		
	if (split_tags(minfo->tags.payload, qtags) == -1){
		fprintf(stderr, "%s\n", "Error in split_tags");
		return -1;
	}

	asprintf(&args[0], "%d", q_idx); //pos
	asprintf(&args[1], "%d", instrument); //instrument class
	asprintf(&args[2], "%d", minfo->chords[q_idx].note); //chord_note
	asprintf(&args[3], "%d", minfo->chords[q_idx].mode); //chord_mode mask
	asprintf(&args[4], "%s", qtags[0]); //tag_dynamic
	asprintf(&args[5], "%s", qtags[1]); //tag_genre
	/* TODO: We need a HUGE db first, then we can use the given scale
	 * asprintf(&args[6], "%s", qtags[2]); //tag_mood
	 * asprintf(&args[7], "%d", minfo->tonal_zones[q_idx].scale); //scale mask*/
	asprintf(&args[6], "pushed"); //XXX temporary
	asprintf(&args[7], "669"); //XXX temporary
	
	return 0;
}

int compose_measure(struct play_measure_s *pm, struct measure_s *minfo, 
	int instrument, PGconn *dbh)
{
	/* 
	  TODO:
	   Rethink on this based on quaters:
	   - From the minfo get some good quarters V
	   - Once the quarter are decided, scan them and for each semiquaver 
	     check if we should change the note from the previous and in 
	     that case decide the note (reading the probabilities).
	   - Merge the equal consecutive notes (or directly ignore them).
	   - What if the first note doesn't change? What do I send? 
	     The old note as a new note? 
	     Or do we need a new field for this situation?
	*/

	uint8_t q, s, i, sqcount, max_sqcount, ntcount, res, q_size, sq_size;
	char *qargs[8];
	int *qids;
	struct semiquaver_s **sqs;

	max_sqcount = sqcount = count_semiquavers(minfo->tempo);
	
	/* Allocates the array of notes with the max count of notes as size.
	 * It will be truncated later if the notes are lesser than the max. */
	pm->measure = (struct notes_s *)malloc((size_t)max_sqcount *
			sizeof(struct notes_s));


	i = 0; /* Notes indexes in the whole measure */

	for (q = 0; q < max_sqcount/4; q++) {
		res = fill_quarter_args(qargs, minfo, instrument, q);
		if (res == -1)
			return -1;
		q_size = get_quarters(dbh, 0, qargs, &qids);
		if (q_size <= 0){
			fprintf(stderr, "No quarters found\n");
			return -1;
		}

		/* TODO If nquarters are more than 1, do we need some strategy
		 * in order to choose one of them? Now its always the 1st one */
		sq_size = get_semiquavers(dbh, qids[0], &sqs);
		if (sq_size <= 0) {
			fprintf(stderr, "No semiquavers found\n");
			return -1;
		} else if (sq_size > max_sqcount) {
			fprintf(stderr, "Too many semiquavers in one quarter\n");
		}

		float rnd;
		int idx, key_note;
		struct notes_s new_note;
		
		key_note = minfo->tonal_zones[q].note;

		for (s = 0; s < sq_size; s++) {
	    		memset(&new_note, 0, sizeof(new_note));
			//TODO: just for now, we should decide some policy then
			if (s == 0)
				rnd = 0.0;
			else 
				rnd = (float)rand() / ((float)(RAND_MAX) / 1.0);
			
			printf("Bsq_size: %d, s %d\n", sq_size, s);
			if (sqs[s]->pchange >= rnd) {
				idx = decide_note(sqs[s]->pnote);
				
				if (idx == -1) 
					pm->measure[ntcount-1].tempo++;

				new_note.note = note_to_midi(idx, key_note);
				new_note.tempo = 1;
				new_note.id = ntcount;
				new_note.triplets = 0;
				pm->measure[ntcount++] = new_note;
			} else {
				pm->measure[ntcount-1].tempo++;
			}
		}


	}
#if 1 //SOME PROBLEM WITH RE-ALLOC 
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
#endif	
	return 0;
}

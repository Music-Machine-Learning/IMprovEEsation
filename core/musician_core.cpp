/*****************************************************************************/
/* Musician's core facilities                                                */
/* This program is a part of the IMprovEEsation suite.                       */
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

#include <linux/list.h>
#include <improveesation/musician_improviser.h>
#include <improveesation/musician_genetic.h>
#include <improveesation/configuration.h>
#include <improveesation/communication.h>
#include <improveesation/structs.h>
#include <improveesation/db.h>
#include <improveesation/utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

struct musician_fields_s mfields;

int print_semiquaver(struct semiquaver_s *sq);

int musician_init(PGconn **dbh, int coupling, int instrument, int soloist, 
		int musician_id, int play_chords, int genetic)
{
	int res;
	struct rc_conf_s conf;
	int *prioargs, priosize;

	*dbh = NULL;
	res = priosize = 0;
	
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

	/* Check if there is a custom prioargs array for this musician.
	   Otherwise use the one decided by the director */
	mfields.custom_prioargs = 0;
	priosize = get_fixed_prioargs(*dbh, instrument, soloist, &prioargs);
	if (priosize == 1) {
		if (prioargs == NULL) {
			fprintf(stderr, "Error in get fixed prioargs\n");
			return -1;
		}
		memcpy(mfields.prioargs, prioargs, 
		       QUARTER_QUERY_ARGS * sizeof(int));
		mfields.custom_prioargs = 1;
	} else if (priosize != 0) {
		fprintf(stderr, "Error in get fixed prioargs\n");
		return -1;
	}

	int i = 0;
	print_debug("prioargs [%d %d %d %d %d %d %d %d %d]\n", 
		    mfields.prioargs[i++],
		    mfields.prioargs[i++],
		    mfields.prioargs[i++],
	 	    mfields.prioargs[i++],
		    mfields.prioargs[i++],
		    mfields.prioargs[i++],
		    mfields.prioargs[i++],
		    mfields.prioargs[i++],
		    mfields.prioargs[i++]);

	mfields.instrument = instrument;
	mfields.prev_octave = -1;
	mfields.play_chords = play_chords;

	if (mfields.octave_min == -1 && mfields.octave_max == -1) {
		res = get_range(*dbh, instrument, &(mfields.octave_min), 
			        &(mfields.octave_max));
	} else if (mfields.octave_min == -1 || mfields.octave_max == -1) {
		fprintf(stderr, "Invalid octave range [%d %d]\n", 
			mfields.octave_min, mfields.octave_max);
		return -1;
	}

	if (res != 0 || (mfields.octave_max - mfields.octave_min) > MIDI_NOCTAVES) {
		fprintf(stderr, "Invalid octave range [%d %d]\n", 
			mfields.octave_min, mfields.octave_max);
		return -1;
	}

	return 0;
}

int note_to_midi(int note_idx, int key_note)
{
	int midi, octave;


	if (note_idx == 0) {
		midi = MIDI_REST_NOTE;
	} else {
		octave = decide_octave(mfields.octave_min, mfields.octave_max);
		midi = (MIDI_FIRST_NOTE + (NSEMITONES * octave)) + 
				key_note + (note_idx - 1);
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

int assign_new_notes(struct play_measure_s *pm, 
                     struct notes_s *new_notes,
                     int *ntcount,
		     struct semiquaver_s *sq)
{
	printf("new notes assignment\n");
	new_notes->id = *ntcount;
	new_notes->triplets = 0;
	new_notes->tempo = 1;
	new_notes->velocity = decide_velocity(sq->velocity_min, 
	                                      sq->velocity_max);
	
	if ((int8_t)new_notes->velocity == -1)
		return -1;
	
	pm->measure[*ntcount] = *new_notes;
	(*ntcount)++;
	
	return 0;
}

int continue_last_note(struct play_measure_s *pm, 
                       struct play_measure_s *prev_pm, 
                       struct notes_s *new_notes,  
		       int *ntcount,
                       struct semiquaver_s *sq)	
{
	if (*ntcount == 0) {
		new_notes->chord_size = prev_pm->measure[prev_pm->size - 1].chord_size;
		
		memcpy(new_notes->notes, prev_pm->measure[prev_pm->size - 1].notes, 
				MAX_CHORD_SIZE);
	
		pm->unchanged_fst = 1; 
		printf("fisrt note is unchanged\n");
		if (assign_new_notes(pm, new_notes, ntcount, sq))
			return -1;

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
		struct measure_s *minfo, int q_idx, struct semiquaver_s **sqs, 
		int sq_size, int ntcount)
{
	float rnd;
	int idx, i, s, ret, key_note;
	struct notes_s new_notes;

	key_note = minfo->tonal_zones[q_idx].note;
	
	printf("composing quarter. (ntcount: %d)\n", ntcount);
	/*for (s = 0; s < sq_size; s++)
		print_semiquaver(sqs[s]);*/

	/* Compose semiquavers until they are present in the array of semiquavers
	 * that have chance to be played or until the maximum of semiquavers 
	 * in the quarter is reached */
	for (s = 0, i = 0; i < SQS_IN_Q; i++) {
		memset(&new_notes, 0, sizeof(new_notes));
		new_notes.chord_size = 1;
		
		if (s >= sq_size){
			continue_last_note(pm, 
			                   prev_pm, 
			                   &new_notes, 
			                   &ntcount, 
			                   sqs[s]);
			continue;
		}

		printf("sq_size: %d, ntcount %d, (i %d s %d spos %d) \n", 
				sq_size, ntcount, i, s, sqs[s]->position);

		if (s >= SQS_IN_Q) {
			fprintf(stderr, "sq count too big\n");
			return -1;
		} 

		rnd = (float)rand() / ((float)(RAND_MAX) / 1.0);
	
		/* If the current sq doesn't exists in the db for the picked 
		 * quarter or if it exists but does not pass the changing prob, 
		 * we increment the tempo of the previous note, 
		 * event if it's in the previous measure */
		if ((sqs[s]->position != i || sqs[s]->pchange < rnd) &&
				(prev_pm->measure != NULL || ntcount > 0)) {
			printf("continuing last note\n");
			ret = continue_last_note(pm, prev_pm, &new_notes, 
			                         &ntcount, sqs[s]);
			if (ret == -1){
				fprintf(stderr, "error in continue_last_note\n");
				return -1;
			}
		} else {
	
			if (mfields.instrument == INSTRUMENT_DRUM) {
				decide_drum(sqs[s]->pnote, &new_notes, NULL, 0);
				/* TODO: some size check */
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
						decide_chord(&new_notes, minfo, q_idx); 
					printf("chord added.\n");
				}
			}
			if (assign_new_notes(pm, &new_notes, &ntcount, sqs[s]) == -1)
				return -1;
			
		} 	

		/* Increment the s counter only if the current sq position
		   matched with the current sq in the quarter */
		if (sqs[s]->position == i)
			s++;
	}
	
	return ntcount;
}

int fill_quarter_args(char **args, struct measure_s *minfo, int myid,
			int soloist, int q_idx)
{
	int quarter_solo;
	char *qpos, *qinstr, *qchord_note, *qchorde_mode, *qdyna, *qmood,
		*qgenre, *qscale;
	char *qtags[N_TAGS];

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

	int q, i, max_quarters, max_sqcount, ntcount, res, q_size, sq_size, 
	    key_note, q_id;
	char *qargs[QUARTER_QUERY_ARGS];
	int *qids;
	int *prioargs;
	struct semiquaver_s **sqs;

	q_size = ntcount = 0;

	max_quarters = minfo->tempo.upper / (minfo->tempo.lower / SQS_IN_Q);

	max_sqcount = max_quarters * SQS_IN_Q;
	
	/* Allocates the array of notes with the max count of notes as size.
	 * It will be truncated later if the notes are lesser than the max. */
	pm->measure = (struct notes_s *)calloc((size_t)max_sqcount,
			sizeof(struct notes_s));
	

	/* First retrieve the quarters that may match what the director wants.
	 * For each quarter look in the DB for the corrisponding semiquavers
	 * and for each of them decide to change (or not) the note and in case
	 * of changing decide which note to choose with the decision function 
	 * which is based on the probabilities array scan. */
	for (q = 0; q < max_quarters; q++) {
		res = fill_quarter_args(qargs, minfo, myid, soloist, q);
		if (res == -1)
			return -1;
		
		i = 0;

		/* Get the musician prioargs if there is any, 
		   otherwise get the director decided prioargs */
		if (mfields.custom_prioargs == 1)
			prioargs = mfields.prioargs;
		else
			prioargs = minfo->prioargs;

		while (q_size == 0 && i < QUARTER_QUERY_ARGS)
			q_size = get_quarters(dbh, qargs, prioargs, i++, &qids);

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
		} else if (sq_size > max_sqcount / SQS_IN_Q) {
			fprintf(stderr, "Too many semiquavers in one quarter\n");
			return -1;	
		}

		/* Be creative with the current quarter */
		ntcount = compose_quarter(pm, prev_pm, minfo, q, sqs, 
				sq_size, ntcount);
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

int split_note(struct notes_s *nnew, struct list_head *list, int sqcount, 
	       int max_sqcount)
{
	struct notes_s tmp;
	int split_tempo, last_count;

	last_count = sqcount;

	/* Split the new note until it doesn't fit into measures */
	while (nnew->tempo + last_count > max_sqcount) {
		memset((void *)&tmp, 0, sizeof(struct notes_s));
		tmp = *nnew;

		/* Reduce the tempo of the last note (just created) to let 
		   it fit in the measure */
		split_tempo = max_sqcount - last_count;
		nnew->tempo = split_tempo;
		last_count += split_tempo;

		print_debug("Note split left\n");
		print_debug_note(nnew);

		/* Reset the last_counter when we finished a measure */
		if (last_count == max_sqcount) {
			last_count = 0;
		} else {
			fprintf(stderr, "Something wrong in split note\n");
			return -1;
		}
		
		/* Create a new note with the tempo of the previous minus
		 * the amount of tempo reduced from the previous */
		nnew = (struct notes_s *)malloc(sizeof (struct notes_s));
		if (nnew == NULL) {
			fprintf(stderr, "Malloc error in split_note\n");
			return -1;
		}
		*nnew = tmp;
		/* Special id to understand later that it was a continuous note */
		nnew->id = NOTE_CONTINUOUS_ID;
		nnew->tempo -= split_tempo;
		sqcount = nnew->tempo;
		print_debug("Note split right\n");
		print_debug_note(nnew);
		list_add_tail(&(nnew->list), list);
	}

	return sqcount;
}

/* Converts the notes array into a list */
/* TODO  A list should be directly created instead of an array.
   TODO  Otherwise we can't know the time signature at each measure.
   TODO  As a WORKAROUND we assume a fixed time signature that never changes 
*/
int piece_to_list(struct piece_s *piece, struct list_head *list)
{
	int sqcount, max_sqcount, i;
	struct notes_s *nnew;
	
	if (piece->count <= 0 || !piece || !list) {
		fprintf(stderr, "Error in piece_to_list\n");
		return -1;
	}

	max_sqcount = 16; /* TODO: Sorry for this, it's just a workaround */

	sqcount = 0;

	print_debug("Converting into the finalist\n");
	print_debug("Note\tidx\tlength\ttripl\tvelocity\tch_size\tnotes\n");
	for (i = 0; i < piece->count; i++) {
		nnew = (struct notes_s *)malloc(sizeof (struct notes_s));
		*nnew = piece->notes[i];
		list_add_tail(&(nnew->list), list);
	
		print_debug_note(nnew);
		print_debug("counter %d\n", sqcount + nnew->tempo);

		if (sqcount + nnew->tempo == max_sqcount) {
			sqcount = 0;
		} else if (sqcount + nnew->tempo > max_sqcount) {
			print_debug("Splitting the note\n");
			sqcount = split_note(nnew, list, sqcount, max_sqcount);
			if (sqcount == -1) {
				fprintf(stderr, "Error in piece_to_list\n");
				return -1;
			}
		} else {
			sqcount += nnew->tempo;
		}
	}


	return 0;

}

/* Compose a measure looking into the measure hints provided by the director. */
struct list_head *compose_measure_genetic(struct play_measure_s *pm, 
			    		  struct measure_s *minfo,
			    		  struct list_head *node) 
{
	int i, tempo, max_quarters, max_sqcount;

	max_quarters = minfo->tempo.upper / (minfo->tempo.lower / SQS_IN_Q);

	max_sqcount = max_quarters * SQS_IN_Q;
	
	/* Allocates the array of notes with the max count of notes as size.
	 * It will be truncated later if the notes are lesser than the max. */
	pm->measure = (struct notes_s *)calloc((size_t)max_sqcount,
		sizeof(struct notes_s));

	for(i = 0, tempo = 0; tempo < max_sqcount; i++) {
		pm->measure[i] = *(list_entry(node, struct notes_s, list));

		if (pm->measure[i].id == NOTE_CONTINUOUS_ID) {
			if (i != 0) {
				fprintf(stderr, "Invalid note idx\n");
				return NULL;
			}

			/* It's a continuous note if the idx == -1 and it's
			 * the first of the measure. */
			pm->unchanged_fst = 1;
		}

		pm->measure[i].id = i;
		tempo += pm->measure[i].tempo;
		if (tempo > max_sqcount) {
			fprintf(stderr, "Tempo out of measure error\n");
			return NULL;
		}
		node = node->next;
	}

	pm->size = i;

	/* Re-alloc the array of notes according to the notes count */
	if (i < max_sqcount){
		pm->measure = (struct notes_s *)realloc((void *)pm->measure, 
				sizeof(struct notes_s) * (size_t)i);
		if (pm->measure == NULL){
			fprintf(stderr, "Realloc error: %s\n", strerror(errno));
			
			return NULL;
		}
	}

	return node;
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

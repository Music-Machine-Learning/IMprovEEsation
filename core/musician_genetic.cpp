/*****************************************************************************/
/* Musician composistion genetic features.                                   */
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

#include <stdio.h>
#include <improveesation/structs.h>
#include <improveesation/musician_core.h>
#include <improveesation/musician_improviser.h>
#include <improveesation/const.h>
#include <improveesation/utils.h>

LIST_HEAD(finalist);
int musician_init_genetic(int genetic_mode, const char *samplesfile)
{
	if (!genetic_mode) {
		mfields.ginitial.notes = NULL;
		mfields.ggoal.notes = NULL;
		mfields.finalist = NULL;
		return 0;
	}

	mfields.finalist = &finalist;

	printf("samplefile: %s\n", samplesfile);
	if (parse_sample(samplesfile) == -1) {
		fprintf(stderr, "Failed to parse samples file\n");
		return -1;
	}

	mfields.ginitial.notes = (struct notes_s *)malloc(PIECE_START_SIZE * 
				  sizeof(struct notes_s));

	if (mfields.ginitial.notes == NULL) {
		fprintf(stderr, "Malloc error in init_genetic\n");
		return -1;
	}
	
	mfields.ggoal.notes = (struct notes_s *)malloc(PIECE_START_SIZE * 
				  sizeof(struct notes_s));

	if (mfields.ggoal.notes == NULL) {
		fprintf(stderr, "Malloc error in init_genetic\n");
		return -1;
	}

	mfields.ggoal.size = mfields.ginitial.size = PIECE_START_SIZE;
	mfields.ggoal.count = mfields.ginitial.count = 0;
	return 0;
}

int store_gmeasure(struct play_measure_s *pm, struct measure_s *minfo)
{
	int i, j, s, ngoal, r_idx, idx, size, 
	    max_quarters, max_sqs, q, sq_count, key_note;
	struct play_measure_s *goal_ms, goal_m;
	struct notes_s *nt;
	char *tags[N_TAGS];
	

	if (split_tags(minfo->tags.payload, tags) == -1){
		fprintf(stderr, "%s\n", "Error in split_tags");
		return -1;
	}

	/* Retrieve a set of goal measure from the list of samples */
	ngoal = get_goal_measures(&goal_ms, tags[1]);
	if (ngoal == -1)
		return -1;

	r_idx = rand() % ngoal;
	goal_m = goal_ms[r_idx];
	
	
	print_debug("Storing gmeasure:\n"
		    "\tginitial count %d, ginitial size %d, pm->size %d\n"
		    "\tggoal count %d, ggoal size %d, goal_m size %d\n",
		    mfields.ginitial.count, mfields.ginitial.size, pm->size,
		    mfields.ggoal.count, mfields.ggoal.size, goal_m.size);
	
	s = mfields.ginitial.count;
	if (s + pm->size > mfields.ginitial.size) {
		mfields.ginitial.size *= 2;
		mfields.ginitial.notes = (struct notes_s *)realloc(
				         (void *)mfields.ginitial.notes, 
				         (size_t)mfields.ginitial.size * 
					 sizeof(struct notes_s));
		
		if (mfields.ginitial.notes == NULL){
			fprintf(stderr, "Realloc error in store_measure\n");
			return -1;
		}
		
	}
	
	for (i = 0; i < pm->size; i++) {

		mfields.ginitial.notes[s++] = pm->measure[i];
	}
	mfields.ginitial.count = s;
	
	/* Check if the size of the goal piece note container is enough
	 * for the new goal measure. If it's not, realloc it (double size) */
	s = mfields.ggoal.count;
	if (s + goal_m.size > mfields.ggoal.size) { 
		mfields.ggoal.size *= 2;
		struct notes_s *tmp;
		mfields.ggoal.notes = (struct notes_s *)realloc(
				      (void *)mfields.ggoal.notes, 
				      (size_t)mfields.ggoal.size *
				      sizeof(struct notes_s));
		if (mfields.ggoal.notes == NULL) {
			fprintf(stderr, "Realloc error in store_measure\n");
			return -1;
		}
	}
	
	max_quarters = minfo->tempo.upper / (minfo->tempo.lower / SQS_IN_Q);
	max_sqs = max_quarters * SQS_IN_Q;
	q = sq_count = key_note = 0;
	
	print_debug("prev_octave %d\n", mfields.prev_octave);
	/* Copy each note of a goal measure into the global goal piece */
	for (i = 0; i < goal_m.size; i++, s++) {

		mfields.ggoal.notes[s] = goal_m.measure[i];
		
		if (mfields.ggoal.notes[s].chord_size > 1) {
			fprintf(stderr, "Error: found chords in genetic mode.\n");
			return -1;
		}	

		/* Retrieve the key_note from the director information */
		q = sq_count / SQS_IN_Q;
		key_note = minfo->tonal_zones[q].note;

		if (mfields.prev_octave == -1) {
			fprintf(stderr, "something wrong with prev_octave in "
			                "store_gmeasure\n");
			return -1;
		}
		mfields.ggoal.notes[s].notes[0] += MIDI_FIRST_NOTE + 
			(NSEMITONES * mfields.prev_octave) + key_note - 1;
		
		sq_count += goal_m.measure[i].tempo;
	}
	mfields.ggoal.count = s;

	print_debug("Printing seleceted sample measure..\n");
	for (i = s - goal_m.size; i < s; i++){
		nt = &(mfields.ggoal.notes[i]);
		print_debug("\t%d\t%d\t%d\t%d\t%d\t[",
			nt->id, 
			nt->tempo, 
			nt->triplets,
			nt->velocity,
			nt->chord_size);
		for (j = 0; j < nt->chord_size; j++)
			print_debug("%d, ", nt->notes[j]);
		print_debug("]\n");
	}

	if (sq_count != max_sqs) {
		fprintf(stderr, "Sqs wrong count in the goal samples: %d %d\n", 
			    sq_count, max_sqs);
		return -1;
	}

	free(goal_ms);

	return 0;
}

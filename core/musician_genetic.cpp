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
#include <improveesation/const.h>
#include <improveesation/utils.h>

int musician_init_genetic(int genetic_mode)
{
	if (!genetic_mode) {
		mfields.ginitial.notes = NULL;
		mfields.ggoal.notes = NULL;
		return 0;
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
	int i, s, ngoal, r_idx, idx, size, 
	    max_quarters, max_sqs, q, sq_count, key_note;
	struct play_measure_s *goal_ms, goal_m;
	char *tags[N_TAGS];
	s = mfields.ginitial.count;

	if (s + pm->size > mfields.ginitial.size) {
		int newsize = mfields.ginitial.size * 2;
		mfields.ginitial.notes = (struct notes_s *) 
			realloc((void *)mfields.ginitial.notes, (size_t)newsize);
		if (mfields.ginitial.notes == NULL){
			fprintf(stderr, "Realloc error in store_measure\n");
			return -1;
		}
		mfields.ginitial.size = newsize;
		
	}
	
	for (i = 0; i < pm->size; i++) {

		mfields.ginitial.notes[s++] = pm->measure[i];
	}
	mfields.ginitial.count = s;



	if (split_tags(minfo->tags.payload, tags) == -1){
		fprintf(stderr, "%s\n", "Error in split_tags");
		return -1;
	}

	/* Retrieve a set of goal measure from the list of samples */
	ngoal = get_goal_measures(&goal_ms, tags[0]);
	if (ngoal == -1)
		return -1;

	r_idx = rand() % ngoal;
	goal_m = goal_ms[r_idx];

	/* Check if the size of the goal piece note container is enough
	 * for the new goal measure. If it's not, realloc it (double size) */
	s = mfields.ggoal.count;
	if (s + goal_m.size > mfields.ggoal.size) { 
		int newsize = mfields.ggoal.size * 2;
		mfields.ggoal.notes = (struct notes_s *) 
			realloc((void *)mfields.ggoal.notes, (size_t)newsize);
		if (mfields.ggoal.notes == NULL){
			fprintf(stderr, "Realloc error in store_measure\n");
			return -1;
		}
		mfields.ggoal.size = newsize;
	}
	
	max_quarters = minfo->tempo.upper / (minfo->tempo.lower / SQS_IN_Q);
	max_sqs = max_quarters * SQS_IN_Q;
	q = sq_count = key_note = 0;
	/* Copy each note of a goal measure into the global goal piece */
	for (i = 0; i < goal_m.size && sq_count < max_sqs; i++) {
		mfields.ggoal.notes[s++] = goal_m.measure[i];
		q = sq_count / SQS_IN_Q;
		key_note = minfo->tonal_zones[q].note;

		sq_count += goal_m.measure[i].tempo;
	}
	
	if (sq_count != max_sqs) {
		fprintf(stderr, "Sqs wrong count in the goal samples\n");
		return -1;
	}

	free(goal_ms);

	return 0;
}

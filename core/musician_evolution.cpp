/*****************************************************************************/
/* Genetic loop to evolve an improvised melody                               */
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
#include <improveesation/communication.h>
#include <improveesation/structs.h>

#include <improveesation/musician_evolution.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

void swap_pieces(struct notes_s * fst, struct notes_s * snd, int cross){
	struct notes_s temp[cross];
	int x;
	for (x=0; x<cross; x++){
		temp[x] = fst[x];
		fst[x] = snd[x];
		snd[x] = temp[x];
	}
}

/* Function that checks how much the array trial is similar to goal and stores the similarity in the 3array */
int compute_similarity(struct piece_s *ggoal, struct piece_s *gtrial, 
		       uint8_t *sim)
{
	int i,k;
	int csim = 0;
	int ctot = 0;

	/* FIXME check the effective array length */
	for (i = 0; i < ggoal->count; i++) {

		if (ggoal->notes[i].notes[0] == gtrial->notes[i].notes[0])
			csim++;
		if (ggoal->notes[i].tempo == gtrial->notes[i].tempo)
			csim++;
		if (ggoal->notes[i].triplets == gtrial->notes[i].triplets)
			csim++;
		ctot += 3;
	}

	*sim = ((csim * 100) / ctot);
	return *sim; //AHAHAHAHAAh --> ((csim * 100) / ctot);
}

/* Functions which sort the pool according to similarity (with mergesort) */
void merge_pool(struct piece_s **pool, uint8_t *sim, int fst, int q, int end)
{
	int lw, k, hh, j, B[GENETIC_POOL_SIZE];
	struct piece_s sandbox[GENETIC_POOL_SIZE];
	lw = fst; hh = q + 1; k = fst;	

	while (lw <= q && hh <= end) {
		if (sim[lw] <= sim[hh]) {
			B[k] = sim[lw];
			sandbox[k] = *pool[lw];
			lw++;
		} else {
			B[k] = sim[hh];
			sandbox[k] = *pool[hh];
			hh++;
		}
		k++;
	}

	if (lw > q) {
		for(j = hh; j <= end; j++) {
			B[k] = sim[j];
			sandbox[k] = *pool[j];
			k++;
		}
	} else {
		for (j = lw; j <= q; j++) {
			B[k] = sim[j];
			sandbox[k] = *pool[j];
			k++;
		}
	}
	
	for (j = fst; j <= end; j++)
		sim[j] = B[j];

	*pool[j] = sandbox[j];
}

void mergesort_pool(struct piece_s **pool, uint8_t * sim, int fst, int end){
	int q;
	if (fst < end){
		q = ((fst + end) / 2);
		mergesort_pool(pool, sim, fst, q);
		mergesort_pool(pool, sim, q + 1 ,end);
		merge_pool(pool, sim, fst, q, end);
	}
}

int sort_pool(struct piece_s **pool, uint8_t * sim)
{
	mergesort_pool(pool, sim, 0, GENETIC_POOL_SIZE - 1);
	return 0;
}

/* TODO ensure about the length which has to be the same as the initial */
int change_random_note(struct piece_s *piece){
	int i,r;
	for(i=0; i<NUM_CHANGE(piece->size); i++){
		r = rand()%NUM_CHANGE(piece->size);
		piece->notes[r].notes[0] += (rand()%23 - 11);
		piece->notes[r].tempo += (rand()%9 - 4);
	}
}

/** Function used to recombine pieces in the genetic pool.
 * Every piece gets a replacement 
 * Currently we use the upper 25% and we put the recombined in the
 * lower 25% */
int transrecombine(struct piece_s **genetic_pool, int index){
	int j, crossover;
	
	for(j=0; j<GENETIC_POOL_SIZE/4; j+=2){
		/* Choose the crossover point */
		if ((*genetic_pool)[index].size < (*genetic_pool)[index+1].size){
			crossover = rand()%((*genetic_pool)[index].size);
		} else {
			crossover = rand()%((*genetic_pool)[index+1].size);
		}
		swap_pieces((*genetic_pool)[index].notes, (*genetic_pool)[index+1].notes, crossover);
		
		/* TODO add here transposon propagation */
		
		/* Here's the replacement on the lower 25% */
		(*genetic_pool)[GENETIC_POOL_SIZE - index - 1] = (*genetic_pool)[index];
		(*genetic_pool)[GENETIC_POOL_SIZE - index - 2] = (*genetic_pool)[index + 1];
			
	}
}

int genetic_loop(struct piece_s *ginitial, struct piece_s *ggoal)
{

	/* Arrays that contain the genetic pool generated every round */
	struct piece_s genetic_pool[GENETIC_POOL_SIZE];
	/* Array that stats the similarity coefficient (0-100) with the ideal, the postion of the most similar and the position of the less similar */
	uint8_t similarity[GENETIC_POOL_SIZE][3];
	/* Array that stats the similarity coefficient (0-100) with the ideal */
	uint8_t sim[GENETIC_POOL_SIZE];

	int i,j;

	/* Initialize the genetic pool with copies of the original (and the similarities with 0 */
	for (i = 0; i < GENETIC_POOL_SIZE; i++) {
		for (j = 0; j < ginitial->count; j++) {
			genetic_pool[i].notes[j] = ginitial->notes[j];
		}

		/* FIXME to remove when the similarity is complete */
		similarity[i][0] = 0;
		similarity[i][1] = 0;
		similarity[i][2] = 0;

		compute_similarity(ggoal, &genetic_pool[i], &sim[i]);	
	}

	/* Now we have everything set, let's get started with the loop! */
	for (i = 0; i < GENETIC_ROUNDS; i++) {

		/* Recombination and transposon propagation (skip on the first iteration) */
		/* This function automatically replaces the upper 25% with the lower */
		if(i > 0){
			transrecombine((struct piece_s **)&genetic_pool, j);
		}

		/* First we change a number (according to the length) of notes at random */
		for(j=0; j<GENETIC_POOL_SIZE; j++)
			change_random_note(&genetic_pool[j]);
		
		/* Then we need to compute similarity and sort the array according to it */
		for(j=0; j<GENETIC_POOL_SIZE; j++)
			compute_similarity(ggoal, &genetic_pool[j], &sim[j]);

		sort_pool((struct piece_s**)&genetic_pool, sim);
	}

	/* Need to sort once again */
	
	/* Finally we keep the most similar */
	*ginitial = genetic_pool[0];

	return sim[0];
}

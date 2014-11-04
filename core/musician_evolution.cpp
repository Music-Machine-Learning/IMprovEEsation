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
#include <improveesation/utils.h>

#include <improveesation/musician_evolution.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

using namespace std;

/* Bayesian Maps */
map<uint8_t, int> train_unigrams_pitch;
map<uint8_t, int> test_unigrams_pitch;
map<uint8_t, int> train_unigrams_tempo;
map<uint8_t, int> test_unigrams_tempo;

map<uint16_t, int> train_bigrams_pitch;
map<uint16_t, int> test_bigrams_pitch;
map<uint16_t, int> train_bigrams_tempo;
map<uint16_t, int> test_bigrams_tempo;

void swap_pieces(struct notes_s *fst, struct notes_s *snd, int cross){
	struct notes_s temp[cross];
	int x;
	for (x = 0; x < cross; x++){
		temp[x] = fst[x];
		fst[x] = snd[x];
		snd[x] = temp[x];
	}
}

/* This function prints in a user-friendly way a piece in input */
void print_piece(struct piece_s piece, char * name, double sim){
	#ifdef DEBUG
	int i;
	
	print_debug("Piece %s sz:%d cnt:%d sim:%f [",
				name, piece.size, piece.count, sim);
	for (i=0; i<piece.count; i++){
		print_debug("%d/%d ", piece.notes[i].notes[0], piece.notes[i].tempo);
	}
	print_debug("]\n");
	#endif
}

/* Function that checks how much the array trial is similar to goal and stores the similarity in the 3array */
int compute_similarity(struct piece_s *ggoal, struct piece_s *gtrial, 
		       double *sim)
{
	int i,k;
	uint16_t bg_pitch, bg_tempo, tmp_pitch, tmp_tempo;
	
	/* Position similarity in percentage (stored in csim) */
	double csim = 0;
	int ctot = 0;
	
	/* Inverse Perplexities */
	double u_pt_ppl = 0;
	double u_tm_ppl = 0;
	double b_pt_ppl = 0;
	double b_tm_ppl = 0;
	
	test_unigrams_pitch.clear();
	test_unigrams_tempo.clear();
	test_bigrams_pitch.clear();
	test_bigrams_tempo.clear();
	
	
	for (i = 0; i < ggoal->count; i++){
		/* In unigrams, if note or tempo exist in map increase it else
		 * initialize it to 1. */
		if(test_unigrams_pitch[gtrial->notes[i].notes[0]])
			test_unigrams_pitch[gtrial->notes[i].notes[0]]++;
		else
			test_unigrams_pitch[gtrial->notes[i].notes[0]] = 1;
		if(test_unigrams_tempo[gtrial->notes[i].tempo])
			test_unigrams_tempo[gtrial->notes[i].tempo]++;
		else
			test_unigrams_tempo[gtrial->notes[i].tempo] = 1;
			
		/* For bigram is the same, but avoid the last couple (because 
		 * snd is null) */
		if(i+1 != ggoal->count){
			tmp_pitch = LOAD_BIGRAM(
				gtrial->notes[i].notes[0],
				gtrial->notes[i+1].notes[0]);
			tmp_tempo = LOAD_BIGRAM(
				gtrial->notes[i].tempo,
				gtrial->notes[i+1].tempo);
			
			if(test_bigrams_pitch[tmp_pitch])
				test_bigrams_pitch[tmp_pitch]++;
			else
				test_bigrams_pitch[tmp_pitch] = 1;
			if(test_bigrams_tempo[tmp_tempo])
				test_bigrams_tempo[tmp_tempo]++;
			else
				test_bigrams_tempo[tmp_tempo] = 1;
		}
	}
	
	/* Scan the goal array */
	for (i = 0; i < ggoal->count; i++) {
		
		/* Compute the point-to-point similarity */
		if (ggoal->notes[i].notes[0] == gtrial->notes[i].notes[0])
			csim += 1;
		if ((ggoal->notes[i].tempo == gtrial->notes[i].tempo) &&
			(ggoal->notes[i].triplets == gtrial->notes[i].triplets))
			csim++;
		ctot += 2;
	}
	
	/* Scan the unigram pitch map */
	for(uni_iter current = test_unigrams_pitch.begin();
					current != test_unigrams_pitch.end(); current++) {
		/* Euclidean distance */
		if (train_unigrams_pitch[current->first] != 0)		
			u_pt_ppl += 1 / sqrt(1 + pow(
				((double)train_unigrams_pitch[current->first] - 
				(double)current->second), 2));
	}
	/* Scan the unigram tempo map */
	for(uni_iter current = test_unigrams_tempo.begin();
				current != test_unigrams_tempo.end(); current++) {
		/* Euclidean distance */
		if (train_unigrams_tempo[current->first] != 0)		
			u_tm_ppl += 1 / sqrt(1 + pow(
				((double)train_unigrams_tempo[current->first] - 
				(double)current->second), 2));
	}
	/* Scan the bigram pitch map */
	bi_iter end = test_bigrams_pitch.end();

	for(bi_iter current = test_bigrams_pitch.begin();
					current != end; current++) {
		/* Euclidean distance */
		if (train_bigrams_pitch[current->first] != 0)
			b_pt_ppl += 1 / sqrt(1 + pow(
				((double)train_bigrams_pitch[current->first] - 
				(double)current->second), 2));
	}
	/* Scan the bigram tempo map */
	for(bi_iter current = test_bigrams_tempo.begin();
					current != test_bigrams_tempo.end(); current++) {
		/* Euclidean distance */
		if (train_bigrams_tempo[current->first] != 0)		
			b_tm_ppl += 1 / sqrt(1 + pow(
				((double)train_bigrams_tempo[current->first] - 
				(double)current->second), 2));
	}
	
	csim /= ctot;
	u_pt_ppl /= test_unigrams_pitch.size();
	u_tm_ppl /= test_unigrams_tempo.size();
	b_pt_ppl /= test_bigrams_pitch.size();
	b_tm_ppl /= test_bigrams_tempo.size();

	if (!ggoal->count)
		*sim = 0;
	else
		*sim = csim * GET_WEIGHT(P2P) + 
				u_pt_ppl * GET_WEIGHT(UNP) + 
				u_tm_ppl * GET_WEIGHT(UNT) + 
				b_pt_ppl * GET_WEIGHT(BIP) + 
				b_tm_ppl * GET_WEIGHT(BIT);
				
	//~ printf("--> %f %f %f %f %f - %f\n",
		//~ u_pt_ppl, u_tm_ppl, b_pt_ppl, b_tm_ppl, csim, *sim);

	return *sim;
}

/* Functions which sort the pool according to similarity (with mergesort) */
void merge_pool(struct piece_s *pool, double *sim, int fst, int q, int end)
{
	int lw, k, hh, j;
	double B[GENETIC_POOL_SIZE];
	struct piece_s sandbox[GENETIC_POOL_SIZE];
	lw = fst; hh = q + 1; k = fst;	

	while (lw <= q && hh <= end) {
		if (sim[lw] <= sim[hh]) {
			B[k] = sim[lw];
			sandbox[k] = pool[lw];
			lw++;
		} else {
			B[k] = sim[hh];
			sandbox[k] = pool[hh];
			hh++;
		}
		k++;
	}
	if (lw > q) {
		for(j = hh; j <= end; j++) {
			B[k] = sim[j];
			sandbox[k] = pool[j];
			k++;
		}
	} else {
		for (j = lw; j <= q; j++) {
			B[k] = sim[j];
			sandbox[k] = pool[j];
			k++;
		}
	}
	for (j = fst; j <= end; j++){
		sim[j] = B[j];
		pool[j] = sandbox[j];
	}
}

void mergesort_pool(struct piece_s * pool, double * sim, int fst, int end){
	
	int q;
	if (fst < end){
		q = ((fst + end) / 2);
		mergesort_pool(pool, sim, fst, q);
		mergesort_pool(pool, sim, q + 1 ,end);
		merge_pool(pool, sim, fst, q, end);
	}
}

/* This sorts the pool in INCREASING order */
int sort_pool(struct piece_s * pool, double * sim)
{
	mergesort_pool(pool, sim, 0, GENETIC_POOL_SIZE - 1);
	return 0;
}

/* TODO ensure about the length which has to be the same as the initial */
/* Change a random number of notes according to the piece length */
int change_random_note(struct piece_s *piece){
	int i,r;
	
	print_debug("Changing positions: ");
	for(i=0; i<NUM_CHANGE(piece->count); i++){
		r = rand()%piece->count; /* Establish which note to change */
		
		/* Change the pitch */
		if(piece->notes[r].notes[0] != 128)
			piece->notes[r].notes[0] = 
				(piece->notes[r].notes[0] + 
				(rand()%NOTES_CHANGE_RANGE - NOTES_CHANGE_DIST))%128;
		else
			piece->notes[r].notes[0] = (MIDDLE_NOTE +
				(rand()%NOTES_CHANGE_RANGE - NOTES_CHANGE_DIST))%128;
				
		/* Change the tempo */		
		piece->notes[r].tempo = (piece->notes[r].tempo + 
			(rand()%TEMPO_CHANGE_RANGE - TEMPO_CHANGE_DIST));
		/* I know, double assignment, but it doesn't work otherwise */
		piece->notes[r].tempo = (piece->notes[r].tempo)%MAX_TEMPO + 1;

		print_debug("%d ", r);
	}
	print_debug("\n");
}

/** Function used to recombine pieces in the genetic pool.
 * Every piece gets a replacement 
 * Currently we use the upper 25% and we put the recombined in the
 * lower 25% */
int transrecombine(struct piece_s * genetic_pool){
	int j, crossover;
	
	for (j = 0; j < GENETIC_POOL_SIZE/4; j += 2) {
		
		/* We're using these weird indexes because the array is sorted
		 * with increasing order, so the pieces to treat are at the end
		 * and the ones to be replaced at the beginning */
		if ((genetic_pool[GENETIC_POOL_SIZE - j - 1].count == 0) ||
			(genetic_pool[GENETIC_POOL_SIZE - j - 2].count == 0)){
			fprintf(stderr, "Zero size error in transrecombine\n");
			return -1;
		}
		/* Choose the crossover point */
		if (genetic_pool[GENETIC_POOL_SIZE - j - 1].count < 
						genetic_pool[GENETIC_POOL_SIZE - j - 2].count) {
			
			crossover = (rand()%
				(genetic_pool[GENETIC_POOL_SIZE - j - 1].count)) + 1;
		} else {
			crossover = (rand()%
				(genetic_pool[GENETIC_POOL_SIZE - j - 2].count)) + 1;
		}
		
		/* Here's the replacement on the lower 25% */
		memcpy(genetic_pool[j].notes, 
				genetic_pool[GENETIC_POOL_SIZE - j - 1].notes, 
				sizeof(struct notes_s) * 
				genetic_pool[GENETIC_POOL_SIZE - j - 1].count);
		memcpy(genetic_pool[j + 1].notes, 
				genetic_pool[GENETIC_POOL_SIZE - j - 2].notes, 
				sizeof(struct notes_s) * 
				genetic_pool[GENETIC_POOL_SIZE - j - 2].count);
		genetic_pool[j].count = 
				genetic_pool[GENETIC_POOL_SIZE - j - 1].count;
		genetic_pool[j + 1].count = 
				genetic_pool[GENETIC_POOL_SIZE - j - 2].count;
		
		swap_pieces(genetic_pool[j].notes, genetic_pool[j + 1].notes,
					crossover);
		
		/* TODO add here transposon propagation */
			
	}

	return 0;
}

int genetic_loop(struct piece_s *ginitial, struct piece_s *ggoal)
{

	/* Arrays that contain the genetic pool generated every round */
	struct piece_s genetic_pool[GENETIC_POOL_SIZE];
	/* Array that stats the similarity coefficient (0-100) with the ideal */
	double sim[GENETIC_POOL_SIZE];

	int i,j,percent;

	/* Temporary stuff to fill the maps */
	uint16_t bg_pitch, bg_tempo;
	
	/* Print the initial arrays in debug mode */
	print_piece(*ginitial, "init", 0);
	print_piece(*ggoal, "goal", 0);
	percent = 0;

	/* Init and fill the training maps */
	for (i = 0; i < ggoal->count; i++){
		
		/* In unigrams, if note or tempo exist in map increase it else
		 * initialize it to 1. */
		if(train_unigrams_pitch[ggoal->notes[i].notes[0]])
			train_unigrams_pitch[ggoal->notes[i].notes[0]]++;
		else
			train_unigrams_pitch[ggoal->notes[i].notes[0]] = 1;
		if(train_unigrams_tempo[ggoal->notes[i].tempo])
			train_unigrams_tempo[ggoal->notes[i].tempo]++;
		else
			train_unigrams_tempo[ggoal->notes[i].tempo] = 1;
			
		/* For bigram is the same, but avoid the last couple (because 
		 * snd is null) */
		if(i+1 != ggoal->count){
			bg_pitch = LOAD_BIGRAM(
				ggoal->notes[i].notes[0],
				ggoal->notes[i+1].notes[0]);
			bg_tempo = LOAD_BIGRAM(
				ggoal->notes[i].tempo,
				ggoal->notes[i+1].tempo);
			
			if(train_bigrams_pitch[bg_pitch])
				train_bigrams_pitch[bg_pitch]++;
			else
				train_bigrams_pitch[bg_pitch] = 1;
			if(train_bigrams_tempo[bg_tempo])
				train_bigrams_tempo[bg_tempo]++;
			else
				train_bigrams_tempo[bg_tempo] = 1;
		}
	}
	
	/* Initialize the genetic pool with copies of the original (and the similarities with 0) */
	for (i = 0; i < GENETIC_POOL_SIZE; i++) {
		/* XXX FREE */
		genetic_pool[i].notes = (struct notes_s *)calloc(ggoal->size,
						sizeof(struct notes_s));
		if (!genetic_pool[i].notes) {
			perror("calloc");
			return -1;
		}

		/* XXX FREE */
		/* Customized memcpy to ensure that the sizes are the same */
		for(j=0; j<ggoal->count; j++){
			genetic_pool[i].notes[j] = 
							ginitial->notes[j%(ginitial->count)];
		}
			
		genetic_pool[i].size = ggoal->size;
		genetic_pool[i].count = ggoal->count;
		
		compute_similarity(ggoal, &genetic_pool[i], &sim[i]);
	}

	/* Now we have everything set, let's get started with the loop! */
	for (i = 0; i < GENETIC_ROUNDS; i++) {

		print_piece(genetic_pool[GENETIC_POOL_SIZE - 1], "entr", sim[GENETIC_POOL_SIZE - 1]);
		
		/* Recombination and transposon propagation (skip on the first iteration) */
		/* This function automatically replaces the upper 25% with the lower */
		if(i > 0)
			if(transrecombine(genetic_pool) == -1)
				return -1;
		
		print_piece(genetic_pool[GENETIC_POOL_SIZE - 1], "tran", sim[GENETIC_POOL_SIZE - 1]);

		/* First we change a number (according to the length) of notes at random */
		/* Then we need to compute similarity and sort the array according to it */
		for(j=0; j<GENETIC_POOL_SIZE; j++) {
			change_random_note(&genetic_pool[j]);
			compute_similarity(ggoal, &genetic_pool[j], &sim[j]);
		}
		
		print_piece(genetic_pool[GENETIC_POOL_SIZE - 1], "chnt", sim[GENETIC_POOL_SIZE - 1]);
		
		sort_pool(genetic_pool, sim);
		
		print_piece(genetic_pool[GENETIC_POOL_SIZE - 1], "sort", sim[GENETIC_POOL_SIZE - 1]);
		
		printf("Completed %d/500\tsimilarity:%lf\n", i + 1, sim[GENETIC_POOL_SIZE - 1]);
	}
	
	/* Finally we keep the most similar */
	*ginitial = genetic_pool[GENETIC_POOL_SIZE - 1];
	
	print_piece(*ginitial, "final", 0);
	print_piece(*ggoal, "goal", 0);

	return sim[GENETIC_POOL_SIZE - 1];
}

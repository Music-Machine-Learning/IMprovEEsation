/*****************************************************************************/
/* Utilities functions                                                       */
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
#include <stdlib.h>
#include <string.h>

#include <improveesation/structs.h>
#include <improveesation/utils.h>

struct play_measure_s glob_ideal[MAX_SAMPLE_ROW];
char dyna_list[MAX_SAMPLE_ROW][16]; /* Fictional hash corresponding to the above array */

char * trim(char *s); /* in configuration.cpp */

int byte_size(unsigned int n){
	int h = 0;
	int j = n;
	while(j > 0){
		h++;
		j = j>>8;
	}
	return h;
}

/* Arrange the N elements of ARRAY in random order. */
void shuffle_array(int *array, size_t n)
{
	size_t i;
	if (n <= 1) {
		fprintf(stderr, "Warning: array size too small in shuffle_array\n");
		return;
	}
	for (i = 0; i < n - 1; i++) {
		size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
		int t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

/* This allocs a list of "ideal" patterns */
/* the list is extracted from the file given in input selecting the dynamics also given */
int parse_sample(const char * filename){
	int res = 0;
	FILE* file = fopen(filename, "r");
	if (!file){
		perror("open");
		return -1;
	}
		
	char * line = NULL;
	char temp[16];
	size_t size = 0;
	int n, csize, cnote, ctime, ctriplet;	
	
	printf("Paring the sample file...\n");
		
	for (n = 0; (csize = getline(&line, &size, file)) > 0; n++) {
		/* avoid comments */
		strcpy(line, trim(line));
		if (line[0] == '#' || size <= 1){
			n--;
			#ifdef DEBUG
			printf("Found comment\n");
			#endif
			continue;
			}
		
		/* Parse the CSV string */

		#ifdef DEBUG
		printf("Parsing line%n:\n", n);
		#endif
		
		/* Set the basic fields of the play_measure */
		glob_ideal[n].id = 0;
		glob_ideal[n].musician_id = 0;
		strcpy(dyna_list[n], strtok(line, ",")); //TODO control
		glob_ideal[n].unchanged_fst = atoi(strtok(NULL, ","));
		glob_ideal[n].size = 0;
		/* For now we allocate the max size of a play_measure */
		glob_ideal[n].measure = (struct notes_s *) malloc(sizeof(struct notes_s) * 48);
		
		do {
			/* Read in this weird way three by three the parameters (note,time,triplet flag) */
			strcpy(temp, strtok(NULL, ","));
			if(!temp){
				break;
			} else {
				cnote = atoi(temp);
			}
			strcpy(temp, strtok(NULL, ","));
			if(!temp){
				break;
			} else {
				ctime = atoi(temp);
			}
			strcpy(temp, strtok(NULL, ","));
			if(!temp){
				break;
			} else {
				ctriplet = atoi(temp);
			}
		    /* Set the note values (always just one note) */
			glob_ideal[n].measure[glob_ideal[n].size].tempo = ctime;
			glob_ideal[n].measure[glob_ideal[n].size].id = 0;
			glob_ideal[n].measure[glob_ideal[n].size].triplets = ctriplet;
			glob_ideal[n].measure[glob_ideal[n].size].chord_size = 1;
			glob_ideal[n].measure[glob_ideal[n].size].notes[0] = cnote; 
			glob_ideal[n].size++; // We're using the measure size as a counter.... risky?
			#ifdef DEBUG
			printf("\tGot note %n duration %n triplet %n\n", cnote, ctime, ctriplet);
			#endif
		} while(1);
	}
	
	printf("parse successful!\n");
	
	return 0;	
}

/* This crawls the goal measures and gives the ones with the right dyna (output = list size) */
int get_goal_measures(struct play_measure_s ** goal_ms, char * dyna, int key_note){
	
	return 0;
}

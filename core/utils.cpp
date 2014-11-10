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
#include <stdarg.h>
#include <string.h>

#include <improveesation/structs.h>
#include <improveesation/utils.h>
#include <improveesation/const.h>

#ifdef DEBUG
int print_debug(const char *f, ...)
{
	va_list parms;
	va_start(parms, f);
	vfprintf(stderr, f, parms);
	va_end(parms);
}

int print_debug_note(struct notes_s *nt) 
{
	int k;
	print_debug("\t%d\t%d\t%d\t%d\t%d\t[",
		    nt->id, 
		    nt->tempo, 
		    nt->triplets,
		    nt->velocity,
		    nt->chord_size);

	for (k = 0; k < nt->chord_size; k++)
		print_debug("%d, ", nt->notes[k]);

	print_debug("]\n");
	
	return 0;
}

int print_debug_array(int *arr, int size) 
{
	int i;
	printf("[");
	for (i = 0; i < size; i++) {
		printf("%d", arr[i]);
		if (i != size - 1)
			printf(", ");
	}
	printf("]\n");

	return 0;
}

int print_debug_array(char **arr, int size) 
{
	int i;
	printf("[");
	for (i = 0; i < size; i++) {
		printf("%s", arr[i]);
		if (i != size - 1)
			printf(", ");
	}
	printf("]\n");

	return 0;
}
#endif

struct play_measure_s glob_ideal[MAX_SAMPLE_ROW];
char dyna_list[MAX_SAMPLE_ROW][DYNA_SIZE]; /* Fictional hash corresponding to the above array */

char * trim(char *s); /* in configuration.cpp */

int byte_size(unsigned int n)
{
	int h = 0;
	int j = n;

	while (j > 0) {
		h++;
		j = j >> 8;
	}

	return (h > 0 ? h : 1);
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
int parse_sample(const char * filename)
{
	int res = 0;
	FILE* file = fopen(filename, "r");
	if (!file){
		perror("open");
		return -1;
	}
		
	char * line = NULL; // line read from file
	char * temp = (char *)malloc(sizeof(char) * DYNA_SIZE); // temporary string to store dyna
	size_t size = 0; 
	int n, csize, cnote, ctime, ctriplet; // current line params	
	
	printf("Parsing the sample file...\n");
		
	for (n = 0; (csize = getline(&line, &size, file)) > 0; n++) {
		/* avoid comments */
		strcpy(line, trim(line));
		if (line[0] == '#' || size <= 1) {
			n--;
			print_debug("Found comment.\n");
			continue;
		}
				
		/* Parse the CSV string */

		print_debug("Parsing line%d:\n", n);
		
		/* Set the basic fields of the play_measure */
		glob_ideal[n].id = 0;
		glob_ideal[n].musician_id = 0;
		
		print_debug("%s\n",line);
			
		strcpy(dyna_list[n], strtok(line, ",")); //TODO control
		
		glob_ideal[n].unchanged_fst = atoi(strtok(NULL, ","));
		glob_ideal[n].size = 0;
		/* For now we allocate the max size of a play_measure */
		/* again? 48?!! Check the malloc return too -> done */
		glob_ideal[n].measure = (struct notes_s *)malloc(sizeof(struct notes_s) * MAX_NOTES_MEASURE);
		
		do {
			/* Read in this weird way three by three the parameters (note,time,triplet flag) */
			printf("begin loop\n");
			if(temp = strtok(NULL, ",")){
				cnote = atoi(temp);
				printf("note: %d ", cnote);
			} else {
				break;
			}
			
			if(temp = strtok(NULL, ",")){
				ctime = atoi(temp);
				printf("time: %d ", ctime);
			} else {
				break;				
			}
			
			if(temp = strtok(NULL, ",")){
				ctriplet = atoi(temp);
				printf("trip: %d\n", ctriplet);
			} else {
				break;
			}
		    /* Set the note values (always just one note) */
			glob_ideal[n].measure[glob_ideal[n].size].tempo = ctime;
			glob_ideal[n].measure[glob_ideal[n].size].id = 0;
			glob_ideal[n].measure[glob_ideal[n].size].velocity = 100;
			glob_ideal[n].measure[glob_ideal[n].size].triplets = ctriplet;
			glob_ideal[n].measure[glob_ideal[n].size].chord_size = 1;
			glob_ideal[n].measure[glob_ideal[n].size].notes[0] = cnote; 
			glob_ideal[n].size++; // We're using the measure size as a counter.... risky?
			print_debug("\tGot note %d duration %d triplet %d\n", cnote, ctime, ctriplet);
		} while(1);
	}
	
	dyna_list[n][0] = -1; /* Terminator */
	printf("parse successful for %d lines!\n", n);
	return 0;	
}

/* This crawls the goal measures and gives the ones with the right dyna (output = list size) */
int get_goal_measures(struct play_measure_s **goal_ms, char *dyna)
{
	int n;
	int cnt = 0;

	(*goal_ms) = (struct play_measure_s *)calloc(MAX_SAMPLE_ROW, 
			sizeof(struct play_measure_s));

	if (*goal_ms == NULL) {
		fprintf(stderr, "Malloc error in get_goal_measures\n");
		return -1;
	}


	/* cycle on all the sample measures*/
	for (n=0; n < MAX_SAMPLE_ROW; n++) {
		
		/* Break if we checked the terminator */
		if (dyna_list[n][0] == -1)
			break;
		
		/* Get in if we got the right dyna */ 	
		if (!strcmp(dyna, dyna_list[n])) {
			
			(*goal_ms)[cnt] = glob_ideal[n];
			cnt++;
		}
	}
	print_debug("cnt %d\n", cnt);
	if (cnt < MAX_SAMPLE_ROW) {
		(*goal_ms) = (struct play_measure_s *)realloc((*goal_ms),
				cnt * sizeof(struct play_measure_s));
		
		if (*goal_ms == NULL) {
			fprintf(stderr, "realloc error in get_goal_measures\n");
			return -1;
		}
	}
	
	return cnt;
}

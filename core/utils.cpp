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

#define MAX_SAMPLE_ROW 32 /* Maximum samples in the file */

struct play_measure_s * glob_ideal;

char * trim(char *s); /* in configuration.cpp */

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
int parse_sample(char * filename){
	int res = 0;
	int file = fopen(filename, O_RDONLY);
	if (!file)
		return -1;
		
	char * line = NULL;
	int size = 0;
	int n, csize;
	
	glob_ideal = (struct play_measure_s *) malloc(sizeof(struct play_measure_s) * MAX_SAMPLE_ROW);	
		
	for (n = 0; (csize = getline(&line, &size, file)) > 0; n++) {
		/* avoid comments */
		if (trim(line)[0] == '#' || size <= 1)
		continue;
		
		/* Parse the CSV string TODO */
		
		
	}
	
	return 0;	
}

/* This crawls the goal measures and gives the ones with the right dyna (output = list size) */
int get_goal_measures(struct play_measure_s ** goal_ms, char * dyna, uint16_t key_note){
	return 0;
}

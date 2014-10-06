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

/* This allocs a list of "ideal" patterns and gives its size in output */
/* the list is extracted from the file given in input selecting the dynamics also given */
int parse_sample(char * filename, play_measure_s ** output, char * dyna, uint16_t key_note){
	int res = 0;
	int file = fopen(filename, O_RDONLY);
	
	
	
}

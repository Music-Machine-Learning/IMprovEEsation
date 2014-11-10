
/*****************************************************************************/
/* Utilities functions header                                                */
/* This header is a part of the IMprovEEsation suite.                       */
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

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define BPM_TO_ATOM(bpm) ((15 / bpm) * (1000000 / 3))
#define MAX_SAMPLE_ROW 32 /* Maximum samples in the file */
#define DYNA_SIZE 16 /* Max characters for the name of a dyna */
#define MAX_NOTES_MEASURE 24 /* Max notes in measure */

void shuffle_array(int *array, size_t n);
int split_tags(char *tags_str, char **tags);
int parse_sample(const char * filename);
int get_goal_measures(struct play_measure_s ** goal_ms, char * dyna);
int byte_size(unsigned int n);

#ifdef DEBUG
	int print_debug(const char *f, ...);
	int print_debug_note(struct notes_s *nt);
	int print_debug_array(int *arr, int size);
	int print_debug_array(char **arr, int size);
#else
	#define print_debug(f, ...) ({})
	#define print_debug_note(n) ({})
	#define print_debug_array(arr, n) ({})
#endif

#define RANDOM_FUZZY() ((float) ((float) rand() / (float) RAND_MAX))

#define concat_list_inplace(a,b)  ({\
	a.prev->next = b.next;\
	b.prev->next = &a;\
	b.next->prev = a.prev;\
	a.prev = b.prev;\
})

#endif /* improveesation/utils.h */

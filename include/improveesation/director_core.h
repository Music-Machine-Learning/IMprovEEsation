/*****************************************************************************/
/* Director's core facilities declarations                                   */
/* This header is a part of the IMprovEEsation suite.                        */
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

#ifndef DIRECTOR_CORE_H
#define DIRECTOR_CORE_H

#include <improveesation/structs.h>

#define CHANGE_TO_SUBGENRE				100 //we don't have any other working genres by now..

#define GENRE_CHANGE_THRESHOLD			0
#define GENRE_CHANGE_ON_ONE_THRESHOLD	30

#define MOOD_CHANGE_THRESHOLD			30  //testing purpose only, needs to be dropped to at most 1
#define MOOD_CHANGE_ON_ONE_THRESHOLD	10

#define CHORD_CHANGE_THRESHOLD			5
#define CHORD_CHANGE_ON_ONE_THRESHOLD	0

#define RANDOM_MULTI_CHORD_THRESHOLD    40  //FIXME: probably tends to lead to ugly results...

#define CHORD_MINOR						0b000010001001
#define CHORD_MAJOR						0b000010010001
#define CHORD_SEVENTH					0b010000000000
#define CHORD_DELTA						0b100000000000

// chord change thresholds should be genre-dependant
#define CHORD_CHANGE_TRITONE			30
#define CHORD_CHANGE_CADENZA			60
#define CHORD_CHANGE_TONAL_ZONE			80

#define BLUES_SCALE                     0b010011101001

#define SOLOER_NONE                     0

#define NOT_SOLO_MAX_SECTIONS           5

// initialize director core with given genre and subgenre
int init_director_core(char* gen, char* sub, uint32_t soloers_count, uint32_t *soloers_list, int measueres_count);

// free core structures and disconnect from db
void free_director_core();

/* update measure contents with next measure's decisions
 * current_measure_id is the measure number inside a section
 * return the total number of measures in one section of current genre */
int decide_next_measure(measure_s *measure, int current_measure_id);

#endif //DIRECTOR_CORE_H

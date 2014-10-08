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

struct pool_s {
	int id;
	struct play_measure_s measures[GENETIC_POOL_SIZE];
};

/* Initialize a popolation of measures starting from a certain measure */
int init_pool(struct pool_s *pool, int pool_size, 
		struct play_measure_s *start_ms, struct measure_s *info_ms)
{
	return 0;
}

int new_generation(struct pool_s *pool, int pool_size, int n_best,
			struct measure_s *info_ms)
{
	return 0;
}

int evolve_measure(struct play_measure_s *start_ms, struct measure_s *info_ms)
{
	int reference_size, g, goal_siz, goal_size;
	struct pool_s gpool;
	struct play_measure_s *reference_ms;
	
	goal_size = 1;//get_goal_measures(&goal_ms);
	
	if (goal_size < 0) {
		fprintf(stderr, "Failed to get goal measures");
		return -1;
	}

	init_pool(&gpool, GENETIC_POOL_SIZE, start_ms, info_ms);

	for (g = 0; g < GENETIC_ROUNDS; g++) {
		/* TODO */	
	}

	return 0;
}

/*****************************************************************************/
/* Database library for the improveesation components                        */
/* This library is a part of the IMprovEEsation suite.                       */
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

#include <improveesation/structs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int get_genres(char ***genres)
{
#if 1
	*genres = (char **) calloc(2, sizeof(char *));

	if (!*genres) {
		perror("malloc");
		return -1;
	}

	(*genres)[0] = (char *) calloc(strlen("blues")+1, sizeof(char));
	strcpy((*genres)[0], "blues");

	(*genres)[1] = NULL;

	return 1;
#endif
}

int get_subgenres(char *genre, char ***subgenres)
{
#if 1
	*subgenres = (char **) calloc(3, sizeof(char *));

	if (!*subgenres) {
		perror("malloc");
		return -1;
	}

	(*subgenres)[0] = (char *) calloc(strlen("bebop")+1, sizeof(char));
	if (!(*subgenres[0])) {
		perror("malloc");
		return -1;
	}

	strcpy((*subgenres)[0], "bebop");

	(*subgenres)[1] = (char *) calloc(strlen("base")+1, sizeof(char));
	if (!(*subgenres[1])) {
		perror("malloc");
		return -1;
	}

	strcpy((*subgenres)[1], "base");

	(*subgenres)[2] = NULL;

	return 2;
#endif
}

void free_genres(char **genre)
{
	int i;
	if (!genre) {
		return;
	}
	for (i = 0; genre[i]; i++) {
		free(genre[i]);
	}

	free(genre);
}

void get_pattern(char *genre, char *patternName, struct pattern_s **pp)
{
#if 1
	/* XXX this is only a test library */
	/* WARNING WARNING WARNING those are not memory safe. */
	/* NEED A HUGE REFACTOR WITH _A_LOT_ of memory checks and controls */
	#define LOAD_MEASURE_SINGLE(p,i,s,m,dyn) ({\
		p->measures[i].stepnumber = 1;\
		p->measures[i].steps =  (int *)calloc(1, sizeof(int));\
		p->measures[i].steps[0] = s;\
		p->measures[i].modes = (char **) calloc(2, sizeof(char *));\
		p->measures[i].modes[0] = (char *) calloc(3, sizeof(char));\
		strcpy(p->measures[i].modes[0], m);\
		p->measures[i].modes[1] = NULL;\
		p->measures[i].dynamics = (char *) calloc(10, sizeof(char));\
		strcpy(p->measures[i].dynamics, dyn);\
	})

	#define LOAD_MEASURE_DOUBLE(p,i,s1, s2,m1, m2,dyn) ({\
		p->measures[i].stepnumber = 2;\
		p->measures[i].steps = (int *) calloc(2, sizeof(int));\
		p->measures[i].steps[0] = s1;\
		p->measures[i].steps[1] = s1;\
		p->measures[i].modes = (char **) calloc(3, sizeof(char *));\
		p->measures[i].modes[0] = (char *) calloc(3, sizeof(char));\
		p->measures[i].modes[1] = (char *) calloc(3, sizeof(char));\
		strcpy(p->measures[i].modes[0], m1);\
		strcpy(p->measures[i].modes[1], m2);\
		p->measures[i].modes[2] = NULL;\
		p->measures[i].dynamics = (char *) calloc(10, sizeof(char));\
		strcpy(p->measures[i].dynamics, dyn);\
	})

	if (strcmp(genre, "blues")){
		fprintf(stderr, "genre %s not found\n", genre);
		return;
	}

	struct pattern_s *p;
	int i;
	/* STUB, bebop blues and base blues implemented (hardcoded) */
	p = (struct pattern_s *) malloc(sizeof(struct pattern_s));
	if (!p) {
		perror("malloc");
		return;
	}
	/* common fields */
	p->measures_count = 12;
	p->moods = (char **) calloc(3, sizeof(char *));
	p->moods[0] = (char *) calloc(strlen("pushed")+1, sizeof(char));
	p->moods[1] = (char *) calloc(strlen("slow")+1, sizeof(char));
	strcpy(p->moods[0], "pushed");
	strcpy(p->moods[1], "slow");
	p->moods[2] = NULL;

	p->measures = (struct measure_pattern_s *) calloc(p->measures_count,
			     sizeof(struct measure_pattern_s));

	if (!strcmp("base", patternName)) {
		/* variants selection */
		p->variants_size = 1;
		p->variants = (struct impro_variant_s *) calloc(1,
						sizeof(struct impro_variant_s));

		p->variants[0].first = 0;
		p->variants[0].last = 4;
		p->variants[0].variants = (char **) calloc(2,
						   sizeof(char *));
		p->variants[0].variants[0] = (char *) calloc(strlen("bebop")+1,
						   sizeof(char));
		strcpy(p->variants[0].variants[0], "bebop");

		p->variants[0].variants[1] = NULL;

		/* measures loading */
		for (i = 0; i < p->measures_count; i++)
			LOAD_MEASURE_SINGLE(p,i,0,"7", "groove");

		p->measures[4].steps[0] = p->measures[5].steps[0] = 5;
		p->measures[8].steps[0] = p->measures[9].steps[0] = 7;

		strcpy(p->measures[11].dynamics, "fill");
	} else if (!strcmp("bebop", patternName)) {
		p->variants_size = 0;

		LOAD_MEASURE_SINGLE(p, 0, 0, "7", "groove");
		LOAD_MEASURE_SINGLE(p, 1, 5, "7", "groove");
		LOAD_MEASURE_SINGLE(p, 2, 0, "7", "groove");
		LOAD_MEASURE_DOUBLE(p, 3, 7, 0, "m7", "7", "groove");
		LOAD_MEASURE_SINGLE(p, 4, 5, "7", "groove");
		LOAD_MEASURE_SINGLE(p, 5, 6, "m7", "groove");
		LOAD_MEASURE_SINGLE(p, 6, 0, "7", "groove");
		LOAD_MEASURE_DOUBLE(p, 7, 4, 9, "m7", "7", "groove");
		LOAD_MEASURE_SINGLE(p, 8, 2, "m7", "groove");
		LOAD_MEASURE_SINGLE(p, 9, 7, "7", "groove");
		LOAD_MEASURE_DOUBLE(p, 10, 4, 9, "m7", "7", "groove");
		LOAD_MEASURE_DOUBLE(p, 11, 2, 7, "m7", "7", "fill");

	} else {
		fprintf(stderr, "warning, pattern not found\n");
	}
	*pp = p;
	return;
#endif
}

void free_pattern(struct pattern_s *p)
{
	int i;
	if (!p)
		return;

	if (p->moods) {
		for (i = 0; p->moods[i]; i++)
			free (p->moods[i]);
		free (p->moods);
	}
	for (i = 0; i < p->variants_size; i++) {
		int j;
		for (j = 0; p->variants[i].variants[j]; j++)
			free(p->variants[i].variants[j]);
		free(p->variants[i].variants);
	}
	free(p->variants);

	for (i = 0; i < p->measures_count; i++) {
		int j;
		for (j = 0; j < p->measures[i].stepnumber; j++)
			free(p->measures[i].modes[j]);
		free(p->measures[i].modes);
		free(p->measures[i].steps);
		free(p->measures[i].dynamics);
	}

	free(p);
}

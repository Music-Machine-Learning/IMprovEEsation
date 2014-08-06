/*****************************************************************************/
/* Database test for the improveesation components                           */
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
#include <improveesation/db.h>
#include <stdio.h>
void printpattern(struct pattern_s *p)
{
	int i;
	printf("measures_count: %d\n", p->measures_count);
	for (i = 0; p->moods[i]; i++)
		printf("mood %d: %s\n", i, p->moods[i]);

	for (i = 0; i < p->variants_size; i++) {
		int j;
		printf("vairants %d\n\trange: %d %d\n", i,
		       p->variants[i].first, p->variants[i].last);
		for (j = 0; p->variants[i].variants[j]; j++)
			printf("\tname %d: %s \n", j,
				p->variants[i].variants[j]);
	}

	for (i = 0; i < p->measures_count; i++) {
		int j;
		printf("measure %d\n", i,
			p->measures[i].stepnumber);
		printf("\t%s\n", p->measures[i].dynamics);
		printf("\t[");
		for (j = 0; j < p->measures[i].stepnumber; j++) {
			printf(" %d c:%s ", p->measures[i].steps[j],
				p->measures[i].modes[j]);
		}
		printf("]\n");
	}
}

int main(int argc, char **argv)
{
	struct pattern_s *p;
	int i;
	get_pattern("base", &p);
	printf("base blues pattern:\n");
	printpattern(p);

	free_pattern(p);

	get_pattern("bebop", &p);
	printf("bebop blues pattern:\n");
	printpattern(p);

	free_pattern(p);
	return 0;
}

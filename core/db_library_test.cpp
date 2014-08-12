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

	printf("variants_size: %d\n", p->variants_size);

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
		printf("measure %d, (%d)\n", i,
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

int print_quarters(int *quarters)
{
	for (int i = 0; quarters[i]; i++)
		printf("quarter_id: %d\n", quarters[i]);

}

int main(int argc, char **argv)
{
	struct pattern_s *p;
	int i, genn, subgenn;
	char **genres;
	char **subgenres;
	int *quarters, quarters_size;
	PGconn *dbh = NULL;


	dbh = db_connect("griffin.dberardi.eu",
			"improveesation_experimental_testes",
			"read_only",
			"testiamo123");

	printf("connected\n");
	
	const char *args[8] = {"3", "1", "10", "137", "groove", "slow", 
		"blues", "669"};

	quarters_size = get_quarters(dbh, 0, args, &quarters);
	print_quarters(quarters);
	free_db_results(quarters);

	genn = get_genres(dbh, &genres);	
	for (i = 0; i < genn; i++) {
		printf("%s\n", genres[i]);
		int j;
		subgenn = get_subgenres(dbh, genres[i], &subgenres);
		for (j = 0; j < subgenn; j++) {
			printf("%s %s pattern:\n", genres[i], subgenres[j]);

			get_pattern(dbh, genres[i], subgenres[j], &p);
			if (!p) continue;
			printpattern(p);

			free_db_results(p);
		}
		free_db_results(subgenres);
	}
	free_db_results(genres);

	db_close(dbh);
	return 0;
}

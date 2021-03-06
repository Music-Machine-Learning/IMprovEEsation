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
#include <improveesation/configuration.h>
#include <errno.h>
#include <stdio.h>
#include <improveesation/const.h>
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

	printf("prioargs [ ");
	for (i = 0; i < QUARTER_QUERY_ARGS; i++)
		printf("%d, ", p->prioargs[i]);
	printf("]\n");
}

int print_quarters(int *quarters)
{
	for (int i = 0; quarters[i]; i++)
		printf("quarter_id: %d\n", quarters[i]);

}

int print_semiquaver(struct semiquaver_s *sq)
{
	int i;
	printf("--semiquaver--\n");
	printf("quarter %d pos %d vmin %d vmax %d\n"
		"pchange %.2f pchange_3qrt %.2f pchange_3qvr %.2f pchange_3smq "
		"%.2f\npnotes {", sq->quarter, sq->position, sq->velocity_min, 
		sq->velocity_max, sq->pchange, sq->pchange_3qrt, sq->pchange_3qvr,
		sq->pchange_3smq);
	
	for (i = 0; i < 13; i++){
		printf("%.2f", sq->pnote[i]);
		if (i != 12)
			printf(", ");
	}
	printf("}\n----\n");
	return 0;
}

void print_prioargs(int *p)
{
	int i;
	printf("[ ");
	for ( i = 0 ; i < QUARTER_QUERY_ARGS; i++)
		printf("%d %s", p[i], (i != QUARTER_QUERY_ARGS - 1? ", ": ""));
	printf("]\n");
}

int main(int argc, char **argv)
{
	struct pattern_s *p;
	int i, genn, subgenn;
	int *prioargs;
	char **genres;
	char **subgenres;
	int *quarters, quarters_size;
	struct rc_conf_s conf;
	PGconn *dbh = NULL;

	if (load_conf(DEFAULT_RC_PATH, &conf) < 4) {
		fprintf(stderr, "error while loading configuration (%s)\n",
			strerror(errno));
		return 1;
	}

	dbh = db_connect(conf.db_host,
			conf.db_name,
			conf.db_user,
			conf.db_passwd);

	printf("connected\n");
	
	char *args[9];
	asprintf(&args[0], "0");
	asprintf(&args[1], "33");
	asprintf(&args[2], "0");
	asprintf(&args[3], "145");
	asprintf(&args[4], "blues");
	asprintf(&args[5], "groove");
	asprintf(&args[6], "pushed");
	asprintf(&args[7], "669");
	asprintf(&args[8], "0");
	
	int args_prios[9] = {3, 7, 1, 2, 0, 5, 4, 6, 8};

	quarters_size = get_quarters(dbh, args, args_prios, 0, &quarters);
	
	if (quarters_size == 0){
		printf("no quarter found");
	}else if (quarters_size == -1){
		printf("some problem with get_quarters");
		return -1;
	}

	print_quarters(quarters);

	printf("\n###single semiquaver test\n");

	struct semiquaver_s *sq;
	int sq_size = 0;
	sq_size = get_semiquaver(dbh, quarters[0], 3, &sq);
	if (sq_size == 0) {
		printf("no semiquavers for the given quarter and position!\n");
	} else if (sq_size == -1) {
		printf("some problem in the get_semiquaver\n");
		return -1;
	} else {
		print_semiquaver(sq);
	}
	
	free_db_results(sq);

	printf("\n!###all semiquavers in quarter test\n");

	struct semiquaver_s **sqs;
	sq_size = get_semiquavers(dbh, quarters[0], &sqs);
	if (sq_size == 0) {
		printf("no semiquavers for the given quarter!\n");
	} else if (sq_size == -1){
		printf("some problem in the get_semiquavers\n");
		return -1;
	}

	for (i = 0; i < sq_size; i++)
		print_semiquaver(sqs[i]);
	
	printf("freeing semiquavers and quarters\n");
	free_db_results(sqs);
	free_db_results(quarters);


	printf("\n###scales test\n");
	uint16_t *scales;
	int scales_size;
	char gen[] = "blues";
	
	scales_size = get_scales(dbh, gen, &scales);
	if (scales_size == 0) {
		printf("no scales found for the given genre\n");
	} else if (scales_size == -1) {
		printf("some problem in the get_scales\n");
		return -1;
	}

	for (i = 0; i < scales_size; i++)
		printf("scale: %d\n", scales[i]);
		
	free_db_results(scales);
	printf("\n###genres, subgenres and patterns test\n");

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

	for (i = 0; i <= 127; i++) {
		if (get_fixed_prioargs(dbh, i, 0, &prioargs)) {
			printf("prioargs for %d\n", i);
			print_prioargs(prioargs);
			free_db_results(prioargs);
		}
		
		if (get_fixed_prioargs(dbh, i, 1, &prioargs)) {
			printf("prioargs for %d soloer\n", i);
			print_prioargs(prioargs);
			free_db_results(prioargs);
		}
	}

	db_close(dbh);

	free_conf(conf);
	return 0;
}

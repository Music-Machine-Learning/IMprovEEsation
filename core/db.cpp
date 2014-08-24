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

/* TODO non-hardcoded table and column names */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libpq-fe.h>

#include <improveesation/structs.h>
#include <improveesation/db.h>


void *sql_array_unload(const char *instr, const char *del, int outtype)
{
	char *s = NULL, *s1, *s2;
	int i = 0;
	void *out = NULL;
	int size;

	asprintf(&s, "%s", instr + 1);
	s[strlen(s) - 1] = '\0';

	if (outtype == CHAR_TYPE)
		size = sizeof(char *);
	else if (outtype == INT_TYPE)
		size = sizeof(int);
	else if (outtype == FLOAT_TYPE)
		size = sizeof(float);

	do {
		s1 = strtok_r(i ? NULL : s, del, &s2);
		out = realloc(out, (i + 1) * size);

		if (s1) {
			if (outtype == CHAR_TYPE) {
				((char **)out)[i] = (char *)calloc(strlen(s1) + 1,
							 sizeof(char));

				if (!((char **)out)[i]) {
					perror("malloc");
					return NULL;
				}

				strcpy(((char **)out)[i], s1);
			} else if (outtype == INT_TYPE) {
				((int *)out)[i] = atoi(s1);
			} else if (outtype == FLOAT_TYPE) {
				((float *)out)[i] = atof(s1);
			}
		} else {
			if (outtype == CHAR_TYPE)
				((char **)out)[i] = NULL;
			else if (outtype == INT_TYPE || outtype == FLOAT_TYPE)
				((int *)out)[i] = -1;
		}
		i++;
	} while (s1);

	return out;
}

/* XXX work only with matrix (bi-dimesional arrays) */
void **sql_array_unload_2(const char *instr, const char startdel,
			       const char enddel, const char *del, int outtype)
{
	void **out = NULL;
	char cp[strlen(instr)];

	strcpy(cp, instr + 1);
	cp[strlen(instr)] = '\0';

	int start, end, i, current = 0, level = 0;
	start = 0;

	for (i = 0; cp[i] != '\0'; i++) {

		if (cp[i] == startdel) {
			level++;
			/* XXX multilevel */
			start = i;
		}
		if (cp[i] == enddel) {
			end = i;

			if (level == 1) {
				char t[end - start + 1];

				out = (void **) realloc(out,
					      (current + 1) * sizeof(void *));

				if (!out) {
					perror("malloc");
					return NULL;
				}
				strncpy(t, cp + start, end - start + 2);
				t[end - start + 1] = '\0';

				((void **)out)[current++] = sql_array_unload(t,
								del, outtype);
			}

			level--;
		}
	}

	out[current] = NULL;
	return out;
}

PGconn *db_connect(const char *host, const char *dbname,
		  const char *user, const char *passwd)
{
	/* Warning! this function will transmit plaintext password! */
	/* DO NOT use it with rw privileged accounts. */
	char *connstring;
	asprintf(&connstring,
			      "host=%s dbname=%s user=%s password=%s",
			      host, dbname, user, passwd);

	PGconn *ret = PQconnectdb(connstring);

	if (PQstatus(ret) != CONNECTION_OK) {
		fprintf(stderr, "connection to db failed: %s\n",
			PQerrorMessage(ret));
		return NULL;
	}
	
	return ret;
}

void db_close(PGconn *c)
{
	PQfinish(c);
}
int get_genres(PGconn *dbh, char ***genres)
{
	int genre_num;
	int i;
	int size = 0;
	PGresult *res;

	res = PQexec(dbh, "SELECT DISTINCT name FROM genre");

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "genre SELECT failed %s",
			PQerrorMessage(dbh));
		return -1;
	}
	size = PQntuples(res);
	genre_num = PQfnumber(res, "name");

	*genres = (char **)calloc(size + 1, sizeof(char *));

	if (!*genres) {
		perror("malloc");
		return -1;
	}
	
	for (i = 0; i < size; i++) {
		char *cvalue = PQgetvalue(res, i, genre_num);
		(*genres)[i] = (char *)calloc(strlen(cvalue) + 1,
					      sizeof(char));

		if (!(*genres)[i]) {
			perror("malloc");
			return -1;
		}

		strcpy((*genres)[i], cvalue);
	}
	(*genres)[size] = NULL;

	PQclear(res);

	return size;
}

int get_subgenres(PGconn *dbh, char *genre, char ***subgenres)
{
	int subgenre_num;
	int i;
	int size = 0;
	PGresult *res;
	
	const char *query = "SELECT subgenre.name FROM subgenre, genre " 
		"WHERE genre.id = subgenre.id_genre AND genre.name = $1";

	res = PQexecParams(dbh, query, 1, NULL, &genre, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "subgenre SELECT failed %s",
			PQerrorMessage(dbh));
		return -1;
	}
	size = PQntuples(res);
	subgenre_num = PQfnumber(res, "name");

	*subgenres = (char **) calloc(size + 1, sizeof(char *));

	if (!*subgenres) {
		perror("malloc");
		return -1;
	}

	for (i = 0; i < PQntuples(res); i++) {
		char *cvalue = PQgetvalue(res, i, subgenre_num);
		
		printf("%s\n", cvalue);
		
		(*subgenres)[i] = (char *)calloc(strlen(cvalue) + 1,
					      sizeof(char));

		if (!(*subgenres)[i]) {
			perror("malloc");
			return -1;
		}

		strcpy((*subgenres)[i], cvalue);
	}

	(*subgenres)[size] = NULL;

	PQclear(res);

	return size;
}

/* Fill the quarters array of IDs with those that in the DB have some 
 * match with the given parameters. The args_mask tells which of those 
 * parameters should be considered. */
int get_quarters(PGconn *dbh, uint8_t args_mask, char **args, 
		int **quarters)
{
	int quarter_num;
	int i;
	int size = 0;
	PGresult *res;

	const char *query; 

	//TODO: do the argument filtering with args_mask

	query = "SELECT quarter.id FROM quarter, scale, genre, scale_genre "
		"where scale_genre.id = quarter.scale_genre and "
		"scale_genre.id_scale = scale.id and "
		"scale_genre.id_genre = genre.id and "
		"pos = $1 and instrument = $2 and chord_note = $3 and "
		"chord_mode = $4 and genre.name = $5 and tag_dyna = $6 and "
		"tag_mood = $7 and scale.id = $8";

	res = PQexecParams(dbh, query, 8, NULL, args, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "subgenre SELECT failed %s",
			PQerrorMessage(dbh));
		return -1;
	}
	size = PQntuples(res);
	quarter_num = PQfnumber(res, "id");

	*quarters = (int *)calloc(size + 1, sizeof(int));

	if (!*quarters) {
		perror("malloc");
		return -1;
	}

	for (i = 0; i < PQntuples(res); i++) {
		char *cvalue = PQgetvalue(res, i, quarter_num);
		
		(*quarters)[i] = atoi(cvalue);
	}

	PQclear(res);

	return size;
}

/* Fill the semiquaver structure with the ith row result data 
 * which is pointed by res. */
int fill_semiquaver_result(struct semiquaver_s **semiquaver, 
					PGresult *res,	int irow)
{
	int pos_fn, velocity_min_fn, velocity_max_fn, pchange_fn, 
	    pchange_3qrt_fn, pchange_3qvr_fn, pchange_3smq_fn, pnote_fn;

	char *pnote_str;

	struct semiquaver_s *sq;

	(*semiquaver) = (struct semiquaver_s *) 
		malloc(sizeof(struct semiquaver_s));

	if (!*semiquaver) {
		perror("malloc");
		return -1;
	}

	/*TODO these PQfnumber should be done before the for loop in some way */
	pos_fn = PQfnumber(res, "pos");
	velocity_min_fn = PQfnumber(res, "velocity_min");
	velocity_max_fn = PQfnumber(res, "velocity_max");
	pchange_fn = PQfnumber(res, "pchange");
	pchange_3qrt_fn = PQfnumber(res, "pchange_3qrt");
	pchange_3qvr_fn = PQfnumber(res, "pchange_3qvr");
	pchange_3smq_fn = PQfnumber(res, "pchange_3smq");
	pnote_fn = PQfnumber(res, "pnote");

	sq = (*semiquaver);
	if (pos_fn != -1)
		sq->position = atoi(PQgetvalue(res, irow, pos_fn));
	sq->velocity_min = atoi(PQgetvalue(res, irow, velocity_min_fn));
	sq->velocity_max = atoi(PQgetvalue(res, irow, velocity_max_fn));
	sq->pchange = atof(PQgetvalue(res, irow, pchange_fn));
	sq->pchange_3qrt = atof(PQgetvalue(res, irow, pchange_3qrt_fn));
	sq->pchange_3qvr = atof(PQgetvalue(res, irow, pchange_3qvr_fn));
	sq->pchange_3smq = atof(PQgetvalue(res, irow, pchange_3smq_fn));

	pnote_str = PQgetvalue(res, irow, pnote_fn);
	sq->pnote = (float *)sql_array_unload_def(pnote_str, FLOAT_TYPE);
	return 0;
}

/* Get all the semiquavers contained in the specified quarter. */
int get_semiquavers(PGconn *dbh, int quarter, 
		struct semiquaver_s ***semiquavers)
{
	int i, r, size = 0;
	PGresult *res;
	char *loadv;
	const char *query;

	asprintf(&loadv, "%d", quarter);

	query = "select * from semiquaver where quarter = $1";

	res = PQexecParams(dbh, query, 1, NULL, &loadv, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "subgenre SELECT failed %s",
			PQerrorMessage(dbh));
		return -1;
	}
	
	size = PQntuples(res);

	/* Allocate the array of semiquavers */
	if (size > 0) {
		(*semiquavers) = (struct semiquaver_s **)
					calloc((size_t)size + 1, 
					sizeof(struct semiquaver_s *));
		
		/* For each semiquaver cointained in the quarter push their 
		 * retrieved fields from the db into their corresponding 
		 * data structures */
		for (i = 0; i < size; i++) {
			
			r = fill_semiquaver_result(&((*semiquavers)[i]), res, i);
			if (r != 0) 
				return -1;
			
			(*semiquavers)[i]->quarter = quarter;
		}
		
		(*semiquavers)[size] = NULL;
	}
	
	PQclear(res);
	return size;
}

/* Get the semiquaver contained in the specified quarter that matches the 
 * given position. (i.e. the second semiquaver of some quarter) */
int get_semiquaver(PGconn *dbh, int quarter, int pos, 
		struct semiquaver_s **semiquaver)
{
	int i, r, size = 0;
	PGresult *res;
	char *loadv[2];
	const char *query;

	asprintf(&(loadv[0]), "%d", quarter);
	asprintf(&(loadv[1]), "%d", pos);

	query = "select * from semiquaver where quarter = $1 and pos = $2";

	res = PQexecParams(dbh, query, 2, NULL, loadv, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "subgenre SELECT failed %s",
			PQerrorMessage(dbh));
		return -1;
	}
	
	size = PQntuples(res);

	if (size > 0){
		if (fill_semiquaver_result(semiquaver, res, 0) != 0){
			return -1;
		}
		
		(*semiquaver)->quarter = quarter;
		(*semiquaver)->position = pos;
	}

	PQclear(res);

	return size;
}

int null_array_size(void *a, int type)
{
	int i;

	if (!a)
		return 0;

	if (type == INT_TYPE) {
		for (i = 0; ((int*)a)[i] != -1; i++)
			;
	}
	if (type == CHAR_TYPE) {
		for (i = 0; ((char*)a)[i]; i++)
			;
	}
	return i;
}

int get_var_meas(PGconn *dbh, struct pattern_s *p, char *var_meas)
{
	int i;
	int start_meas_fn, end_meas_fn, variants_fn;
	char *variants;
	int *var_ids;
	PGresult *res;

	var_ids = (int *) sql_array_unload_def(var_meas, INT_TYPE);
	if (!var_ids) {
		fprintf(stderr,
		       "Error in the parsing of the variable measures");
		return -1;
	}

	p->variants = NULL;
	

	/* TODO: it's possible to make a single query for all the
	 * variations. */
	for (i = 0; var_ids[i] != -1; i++) {
		char *char_id;
		const char *query;
		p->variants_size++;
		p->variants = (struct impro_variant_s *) realloc(
					p->variants,
					(p->variants_size + 1) *
					sizeof(struct impro_variant_s));

		if (!p->variants) {
			perror("malloc");
			return -1;
		}

		asprintf(&char_id, "%d", var_ids[i]);
		printf("%s - charid\n", char_id);
		query = "select start_meas, end_meas, array_agg(subgenre.name) "
			"as variants from var_measure, subgenre where "
			"var_measure.id = $1 and subgenre.id = any(variants) "
			"group by start_meas, end_meas";

		res = PQexecParams(dbh, query,
			     1, NULL, &char_id, NULL, NULL, 0);

		if (PQresultStatus(res) != PGRES_TUPLES_OK) {
			fprintf(stderr, "variants SELECT failed %s\n",
				PQerrorMessage(dbh));
			return -1;
		}
		start_meas_fn = PQfnumber(res, "start_meas");
		end_meas_fn = PQfnumber(res, "end_meas");
		variants_fn = PQfnumber(res, "variants");

		
		p->variants[i].first = atoi(PQgetvalue(res, 0, start_meas_fn));
		p->variants[i].last = atoi(PQgetvalue(res, 0, end_meas_fn));
		variants = PQgetvalue(res, 0, variants_fn);
		
		p->variants[i].variants = (char **) sql_array_unload_def(
							variants, CHAR_TYPE);
	
		printf("%s\n", p->variants[i].variants[0]);
		
		if (!p->variants[i].variants) {
			perror("malloc");
			return -1;
		}
	}

	return i;
}


void get_pattern(PGconn *dbh, char *genre, char *patternName,
		 struct pattern_s **pp)
{
	int i;
	struct pattern_s *p;
	PGresult *res;

	/* Field Position */
	int measure_count_fn, moods_fn, steps_fn,
	    modes_fn, dynamics_fn, var_meas_fn;

	/* Raw parameters */
	char *measure_count, *moods, *steps,
	     *modes, *dynamics, *var_meas;
	
	char **tmp_dynamics;
	char ***tmp_modes;
	int **tmp_steps;

	char *loadv[2];

	const char *query;

	*pp = NULL;

	loadv[0] = genre;
	loadv[1] = patternName;

	query = "SELECT measure_count, moods, steps, modes, dynamics, "
		"variable_measure FROM measures AS m, patterns AS p "
		"WHERE p.id = (SELECT subgenre.id FROM subgenre, genre "
		"WHERE subgenre.id_genre = genre.id and genre.name = $1 "
		"AND subgenre.name = $2) AND m.id = p.measure";


	res = PQexecParams(dbh, query,
			   2, NULL, loadv, NULL, NULL, 0);

	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf(stderr, "measure SELECT failed %s\n",
			PQerrorMessage(dbh));
		return;
	}

	if (PQntuples(res) < 1) {
		fprintf(stderr,
			"Warn: no results\n");
		// !!!! SEGFAULT WARNING URGENT TODO !!!!: 
		// maybe it's better if we handle this error return
		// (or it tryes to get the patterns and the measure count is 
		// is some random value) !!
		return; 
	}

	if (PQntuples(res) > 1) {
		fprintf(stderr,
			"Warn: more than one result, parsing value 0 only\n");
	}

	moods_fn = PQfnumber(res, "moods");
	steps_fn = PQfnumber(res, "steps");
	modes_fn = PQfnumber(res, "modes");
	dynamics_fn = PQfnumber(res, "dynamics");
	var_meas_fn = PQfnumber(res, "variable_measure");
	measure_count_fn = PQfnumber(res, "measure_count");

	moods = PQgetvalue(res, 0, moods_fn);
	steps = PQgetvalue(res, 0, steps_fn);
	modes = PQgetvalue(res, 0, modes_fn);
	dynamics = PQgetvalue(res, 0, dynamics_fn);
	var_meas = PQgetvalue(res, 0, var_meas_fn);
	measure_count = PQgetvalue(res, 0, measure_count_fn);

	/* Consinstency check */
	if (!moods || !steps || !modes || !dynamics || !measure_count) {
		fprintf(stderr,
			"Warn: one of the requested fields where NULL\n");
		return;
	}

	p = (struct pattern_s *) malloc(sizeof(struct pattern_s));
	if (!p) {
		perror("malloc");
		return;
	}

	/* array parsing */
	p->measures_count = atoi(measure_count);
	p->moods = (char **) sql_array_unload_def(moods, CHAR_TYPE);

	tmp_dynamics = (char **) sql_array_unload_def(dynamics, CHAR_TYPE);
	tmp_steps = (int **) sql_array_unload_2_def(steps, INT_TYPE);
	tmp_modes = (char ***) sql_array_unload_2_def(modes, CHAR_TYPE);

	p->variants_size = 0;
	p->variants = NULL;

	if (!p->moods || !tmp_steps || !tmp_modes
	    || !tmp_dynamics || !p->measures_count) {
		fprintf(stderr,
			"Warn: one of the parsed field where NULL\n");
		return;
	}

	p->measures = (struct measure_pattern_s *) calloc(p->measures_count,
			     sizeof(struct measure_pattern_s));

	for (i = 0; i < p->measures_count; i++) {
		int j;
		p->measures[i].steps = tmp_steps[i];
		p->measures[i].modes = tmp_modes[i];
		p->measures[i].dynamics = tmp_dynamics[i];
		p->measures[i].stepnumber = null_array_size(p->measures[i].steps,
							    INT_TYPE);
	}

	if (var_meas && strlen(var_meas) > 1 && strcmp(var_meas,"{}")) {
		p->variants_size = get_var_meas(dbh, p, var_meas);
	}

	PQclear(res);

	*pp = p;
	return;
}

/* TODO what about templates for these free functions? */
void free_db_results(int *results)
{
	free(results);
}

void free_db_results(char **results)
{
	int i;
	if (!results) {
		return;
	}
	for (i = 0; results[i]; i++) {
		free(results[i]);
	}

	free(results);
}

void free_db_results(struct pattern_s *p)
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

	if (p->variants) ///XXX:why don't we just free? ("If ptr is a null pointer, no action occurs.)"
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

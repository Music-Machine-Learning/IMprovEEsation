/*****************************************************************************/
/* Database library for the improveesation components header file            */
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

#ifndef IMPROVEESATION_DB_H
#define IMPROVEESATION_DB_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libpq-fe.h>

#include <improveesation/structs.h>

/* SQL array parsers */
enum {
	CHAR_TYPE,
	INT_TYPE,
	VAR_MEAS_TYPE,
	FLOAT_TYPE
};

void *sql_array_unload(const char *instr, const char *del, int outtype,
			int *arr_size);

void **sql_array_unload_2(const char *instr, const char startdel,
			  const char enddel, const char *del, int outtype);

#define sql_array_unload_def(i,o,s) sql_array_unload(i, ",", o, s)
#define sql_array_unload_2_def(i,o) sql_array_unload_2(i, '{', '}', ",", o)

PGconn *db_connect(const char *host, const char *dbname,
		  const char *user, const char *passwd);
void db_close(PGconn *c);
int get_genres(PGconn *dbh, char ***genres);
int get_subgenres(PGconn *dbh, char *genre, char ***subgenres);
int get_var_meas(PGconn *dbh, struct pattern_s *p, char *var_meas);
int get_quarters(PGconn *dbh, uint8_t args_mask, char **args, int **quarters);
void get_pattern(PGconn *dbh, char *genre, char *patternName,
		 struct pattern_s **pp);
int get_semiquavers(PGconn *dbh, int quarter, struct semiquaver_s ***sqs);
int get_semiquaver(PGconn *dbh, int quarter, int pos, struct semiquaver_s **sqs);

void free_db_results(int *results);
void free_db_results(char **results);
void free_db_results(struct pattern_s *p);

#endif /* improveesation/db.h */

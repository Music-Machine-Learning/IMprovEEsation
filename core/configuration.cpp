/*****************************************************************************/
/* RC file functions for the improveesation components                       */
/* This is a part of the IMprovEEsation suite.                               */
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

/* Note:
 * Unfortunately C/C++ doesn't support reflection out of the box.
 * So this parser is merely a big mess of ifs.
 */

#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <improveesation/configuration.h>

#define SET_STRING_FIELD(r,f,p) ({\
	r->f = (char *) malloc(sizeof(char) * (strlen(p) + 1));\
	strcpy(r->f, p);\
})

#define SET_INT_FIELD(r,f,p) ({\
	r->f = (int *) malloc(sizeof(int));\
	*(r->f) = atoi(p);\
})

/* Check required fields */
#define CHECK_FIELD(r, m, f) ({\
	if (!r->f) {\
		fprintf(stderr, "Required Field " #f " were missing\n");\
		m = 1;\
	}\
})

/* Parse options and put in the right field in the struct */
int option_dispatcher(char *n, char *p, struct rc_conf_s *r)
{
	/* Database Hostname */
	if (!strcmp(n, "db_host"))
		SET_STRING_FIELD(r, db_host, p);

	/* Database Name */
	else if (!strcmp(n, "db_name"))
		SET_STRING_FIELD(r, db_name, p);

	/* Database User */
	else if (!strcmp(n, "db_user"))
		SET_STRING_FIELD(r, db_user, p);

	/* Database password */
	else if (!strcmp(n, "db_passwd"))
		SET_STRING_FIELD(r, db_passwd, p);

	/* Director subgenre change rate */
	else if (!strcmp(n, "dir_change_subgenre"))
		SET_INT_FIELD(r, dir_change_subgenre, p);
	
	/* Director genre change rate on a measure */
	else if (!strcmp(n, "dir_change_genre_on_one"))
		SET_INT_FIELD(r, dir_change_genre_on_one, p);

	/* Director genre change rate on any measure */
	else if (!strcmp(n, "dir_change_genre_on_any"))
		SET_INT_FIELD(r, dir_change_genre_on_any, p);

	/* Director mood change rate on a measure */
	else if (!strcmp(n, "dir_change_mood_on_one"))
		SET_INT_FIELD(r, dir_change_mood_on_one, p);

	/* Director mood change rate on any measure */
	else if (!strcmp(n, "dir_change_mood_on_any"))
		SET_INT_FIELD(r, dir_change_mood_on_any, p);

	/* Director chord change rate on a measure */
	else if (!strcmp(n, "dir_change_chord_on_one"))
		SET_INT_FIELD(r, dir_change_chord_on_one, p);

	/* Director chord change rate on any measure */
	else if (!strcmp(n, "dir_change_chord_on_any"))
		SET_INT_FIELD(r, dir_change_chord_on_any, p);

	/* Director multi chord rate */
	else if (!strcmp(n, "dir_random_multi_chord"))
		SET_INT_FIELD(r, dir_random_multi_chord, p);

	/* Director tritone rate */
	else if (!strcmp(n, "dir_chord_tritone"))
		SET_INT_FIELD(r, dir_chord_tritone, p);

	/* Director "cadenza" of chords. */
	else if (!strcmp(n, "dir_chord_cadenza"))
		SET_INT_FIELD(r, dir_chord_cadenza, p);

	/* Director tonal zone */
	else if (!strcmp(n, "dir_chord_tonal_zone"))
		SET_INT_FIELD(r, dir_chord_tonal_zone, p);
	
	/* Option not implemented */
	else
		return 1;

	return 0;
}

/* Remove spaces and tabs from start of a string */
char *trim(char *s)
{
	int i;
	for (i = 0; s[i] && (s[i] == ' ' || s[i] == '\t'); i++)
		;
	return s + i;
}

int load_conf(const char *path, struct rc_conf_s *r)
{
	int n;
	int missing_field = 0;
	size_t size = 0;
	ssize_t csize = 0;

	char *wpath = NULL;
	char *par_string;
	char *line = NULL;

	FILE *f;

	char conf_name[MAX_CONFIGURATION_SIZE];
	char conf_value[MAX_CONFIGURATION_SIZE];

	memset(r, 0, sizeof(struct rc_conf_s));

	if (path[0] == '~') {
		struct passwd *p = getpwuid(getuid());
		asprintf(&wpath, "%s%s", p->pw_dir, path + 1);
	} else {
		asprintf(&wpath, "%s", path);
	}

	f = fopen(wpath, "r");

	if (!f)
		return -1;

	/* Avoid buffer overflows */
	asprintf(&par_string, "%%%ds %%%ds",
			MAX_CONFIGURATION_SIZE, MAX_CONFIGURATION_SIZE);

	for (n = 0;
	    (csize = getline(&line, &size, f)) > 0;
	     n++) {

		size = csize;

		/* comment */
		if (trim(line)[0] == '#' || size <= 1)
			continue;

		sscanf(trim(line), par_string, conf_name, conf_value);
		if (option_dispatcher(conf_name, conf_value, r))
			fprintf(stderr, "warning, unknown conf: %s\n", conf_name);
	}

	free(line);

	fclose(f);

	/* With that we have some granularity on the error printf. */
	CHECK_FIELD(r, missing_field, db_name);
	CHECK_FIELD(r, missing_field, db_host);
	CHECK_FIELD(r, missing_field, db_user);
	CHECK_FIELD(r, missing_field, dir_change_subgenre);
	CHECK_FIELD(r, missing_field, dir_change_genre_on_one);
	CHECK_FIELD(r, missing_field, dir_change_genre_on_any);
	CHECK_FIELD(r, missing_field, dir_change_mood_on_one);
	CHECK_FIELD(r, missing_field, dir_change_mood_on_any);
	CHECK_FIELD(r, missing_field, dir_change_chord_on_one);
	CHECK_FIELD(r, missing_field, dir_change_chord_on_any);
	CHECK_FIELD(r, missing_field, dir_random_multi_chord);
	CHECK_FIELD(r, missing_field, dir_chord_tritone);
	CHECK_FIELD(r, missing_field, dir_chord_cadenza);
	CHECK_FIELD(r, missing_field, dir_chord_tonal_zone);

	if (missing_field)
		n = -1;

	return n;
}

void free_conf(struct rc_conf_s r)
{
	if (r.db_host)
		free(r.db_host);
	if (r.db_name)
		free(r.db_name);
	if (r.db_passwd)
		free(r.db_passwd);
	if (r.db_user)
		free(r.db_user);
	if(r.dir_change_subgenre)
		free(r.dir_change_subgenre);
	if(r.dir_change_genre_on_one)
		free(r.dir_change_genre_on_one);
	if(r.dir_change_genre_on_any)
		free(r.dir_change_genre_on_any);
	if(r.dir_change_mood_on_one)
		free(r.dir_change_mood_on_one);
	if(r.dir_change_mood_on_any)
		free(r.dir_change_mood_on_any);
	if(r.dir_change_chord_on_one)
		free(r.dir_change_chord_on_one);
	if(r.dir_change_chord_on_any)
		free(r.dir_change_chord_on_any);
	if(r.dir_random_multi_chord)
		free(r.dir_random_multi_chord);
	if(r.dir_chord_tritone)
		free(r.dir_chord_tritone);
	if(r.dir_chord_cadenza)
		free(r.dir_chord_cadenza);
	if(r.dir_chord_tonal_zone)
		free(r.dir_chord_tonal_zone);
}

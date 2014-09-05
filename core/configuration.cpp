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

/* parse options and put in the right field in the struct */
int option_dispatcher(char *n, char *p, struct rc_conf_s *r)
{
	if (!strcmp(n, "db_host"))
		SET_STRING_FIELD(r, db_host, p);
	else if (!strcmp(n, "db_name"))
		SET_STRING_FIELD(r, db_name, p);
	else if (!strcmp(n, "db_user"))
		SET_STRING_FIELD(r, db_user, p);
	else if (!strcmp(n, "db_passwd"))
		SET_STRING_FIELD(r, db_passwd, p);
	else
		return 1;

	return 0;
}

/* remove spaces from start of a string */
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

	/* avoid buffer overflows */
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


	fclose(f);

	/* check required fields */
	if (!r->db_name || !r->db_host || !r->db_passwd || !r->db_user) {
		fprintf(stderr,
		        "some of the required fields (db_name,db_host,db_passwd,db_user) were missing\n");
		return -1;
	}

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
}

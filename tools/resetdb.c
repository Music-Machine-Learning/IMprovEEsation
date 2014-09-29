/*****************************************************************************/
/* Database dump/reset tool                                                  */
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

#include <sys/types.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>

#define DEFAULT_PSQL_ROOT "postgres"
#define DEFAULT_DATABASE "improveesation"

/* Build an update query with mean values */
char *get_update_query( float pchange, float p3qrt, float p3qvr, float p3smq,
			float pnote, float ppause)
{
	char *local, *out;
	int i;
	asprintf(&local, "UPDATE semiquaver SET pchange= %f"
			 ", pchange_3qrt = %f"
			 ", pchange_3qvr = %f"
			 ", pchange_3smq = %f"
			 ", pnote = '{ ",
			 pchange, p3qrt, p3qvr, p3smq);

	for (i = 0; i < 12; i++)
		asprintf(&local, "%s%f,", local, pnote);

	asprintf(&local, "%s%f}' WHERE id in "
		 "(SELECT sq.id FROM semiquaver AS sq,"
		 " quarter AS q WHERE q.id = sq.quarter"
		 " AND solo = 't');",
		 local, ppause);

	out = malloc((strlen(local) + 1) * sizeof(char));
	if (!out)
		return NULL;

	if (!memcpy(out, local, (strlen(local) + 1) * sizeof(char)))
		return NULL;

	return out;
}

/* reset the db to mean values */
int pg_db_reset(char *db_name, int dump)
{
	char *update_query, *gquery;

	gquery = get_update_query(0.5, 0, 0, 0, 1.0/13.0, 0.088);

	if (!gquery)
		return 1;

	asprintf(&update_query, "%s", gquery);
	free(gquery);

	/* Do not print "UPDATE ##" if we need the db dump */
	if (dump) {
		int fd = open("/dev/null", O_WRONLY);
		if (dup2(fd, 1) == -1) {
			perror("dup2");
			return 1;
		}
	}

	execlp("psql", "psql", "-c", update_query, db_name, NULL);

	perror("exec");
	return 1;
}

/* dump the database */
int pg_dump_exec(char *db_name)
{
	execlp("pg_dump", "pg_dump", "-d", db_name, NULL);

	perror("exec");
	return 1;
}

int main(int argc, char **argv)
{
	struct passwd *passwd_entry;
	char *user = NULL, *db_name = NULL;
	int db_dump = 0;

	int opt;

	while ((opt = getopt(argc, argv, "d:u:b")) != -1) {
		switch (opt) {
		/* backup */
		case 'b':
			db_dump = 1;
			break;
		/* database name */
		case 'd':
			db_name = optarg;
			break;
		/* username */
		case 'u':
			user = optarg;
			break;
		default:
			fprintf(stderr,
				"Usage: %s [-b] [-d database] [-u user]\n",
				argv[0]);
		}
	}

	if (!user)
		asprintf(&user, DEFAULT_PSQL_ROOT);

	if (!db_name)
		asprintf(&db_name, DEFAULT_DATABASE);

	passwd_entry = getpwnam(user);

	if (!passwd_entry) {
		perror("getpwnam");
		return 1;
	}

	if (setuid(passwd_entry->pw_uid) || setgid(passwd_entry->pw_gid)) {
		perror("set[u|g]id");
		return 1;
	}

	if (db_dump) {
		if (fork())
			return pg_dump_exec(db_name);
		else
			wait(NULL);
	}

	if (fork())
		return pg_db_reset(db_name, db_dump);
	else
		wait(NULL);

	return 0;
}

/*****************************************************************************/
/* RC files function headers for the improveesation components               */
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
#define DEFAULT_RC_PATH "~/.improveesation.rc"
#define MAX_CONFIGURATION_SIZE 1024

struct rc_conf_s {
	char *db_host;
	char *db_name;
	char *db_user;
	char *db_passwd;
};

int load_conf(const char *path, struct rc_conf_s *r);
void free_conf(struct rc_conf_s r);

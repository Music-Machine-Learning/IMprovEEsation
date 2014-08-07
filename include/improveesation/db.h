/*****************************************************************************/
/* Database library for the improveesation components                        */
/* header file.                                                              */
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

#include <improveesation/structs.h>

int get_genres(char ***genres);
int get_subgenres(char *genre, char ***subgenres);
void free_genres(char **genre);
void get_pattern(char *genre, char *patternName, struct pattern_s **p);
void free_pattern(struct pattern_s *p);

#endif /* improveesation/db.h */

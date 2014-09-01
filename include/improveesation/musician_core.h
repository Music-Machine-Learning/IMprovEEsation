/*****************************************************************************/
/* Musician's core facilities declarations                                   */
/* This header is a part of the IMprovEEsation suite.                        */
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

#ifndef MUSICAN_CORE_H
#define MUSICAN_CORE_H

#include <improveesation/structs.h>
#include <improveesation/db.h>

int compose_note(struct note_s *snote, int id);
int count_semiquavers(struct tempo_s time_signature);
int compose_measure(struct play_measure_s *pm, struct measure_s *minfo, 
			int musician_id, int soloist, PGconn *dbh);
int musician_init(PGconn **dbh);
#endif //MUSICAN_CORE_H

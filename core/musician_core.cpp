/*****************************************************************************/
/* Musician's core facilities                                                */
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

#include <improveesation/musician_core.h>
#include <improveesation/communication.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

class end_of_improvisation_exception eoi_ex;

/* Assign the composed note into the given structure
   and return its length as the number of semiquivers */
int compose_note(struct notes_s *snote, int id)
{
	snote->note = 60;
	snote->tempo = 2;
	snote->id = id;
	snote->triplets = 0;
	return 0;
}

int count_semiquivers(struct tempo_s time_signature)
{
	/* TODO: calculate the number of semiquivers according to the 
	   time signature */
	return 16;
}

int compose_measure(struct play_measure_s *pm, struct measure_s *minfo)
{
	uint8_t i, sqcount, max_sqcount, ntcount;
	
	max_sqcount = sqcount = count_semiquivers(minfo->tempo);
		
	/* Allocates the array of notes with the max count of notes as size.
	 * It will be truncated later if the notes are lesser than the max. */
	pm->measure = (struct notes_s *)calloc((size_t)max_sqcount,
			sizeof(struct notes_s));

	struct notes_s new_note;
	for (i = 0; sqcount > 0; i++) {
	    	memset(&new_note, 0, sizeof(new_note));
		compose_note(&new_note, i);
		sqcount -= new_note.tempo;
		pm->measure[i] = new_note;
		/* TODO: what if it goes under 0? and what about triplets?*/
	}

	ntcount = i;

	/* Re-alloc the array of notes according to the notes count */
	if (ntcount < max_sqcount){
		pm->measure = (struct notes_s *)realloc((void *)pm->measure, 
				sizeof(struct notes_s) * (size_t)ntcount);
		if (pm->measure == NULL){
			fprintf(stderr, "Realloc error: %s\n", strerror(errno));
		
			throw eoi_ex;
		}
		pm->size = ntcount;
	}

	return 0;
}

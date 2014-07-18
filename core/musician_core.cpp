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
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
	int i, sqcount, mlength;
	
	sqcount = count_semiquivers(minfo->tempo);

	for (i = 0; sqcount > 0; i++) {
		struct notes_s new_note;
		compose_note(&new_note, i);
		sqcount -= new_note.tempo; /* TODO: what if it goes under 0? */
		/* TODO: where do I store the new note? */
	}

	/* Allocates the array of notes now, cause we didn't know the size before */
	pm->measure = (struct notes_s *)calloc((size_t)i, sizeof(struct notes_s));

	/* TODO: Push the notes in the array, maybe it's better to alloc first
	 a big array and then realloc */
		
	return 0;
}

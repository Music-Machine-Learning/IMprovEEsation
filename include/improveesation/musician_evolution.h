/*****************************************************************************/
/* Genetic loop to evolve an improvised melody                               */
/* This program is a part of the IMprovEEsation suite.                       */
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

#ifndef MUSICIAN_EVOLUTION_H
#define MUSICIAN_EVOLUTION_H

#define MEASURE_NUM 32
#define MELODY_LENGTH (MEASURE_NUM * 24) /* FIXME MEASURE_NUM is temporary, it shouldn't be hardcoded */
#define NUM_CHANGE(size) (size/20 + 1)
#define NOTES_CHANGE_DIST 11 /* How a note can change in distance during an evolutionary step */
#define NOTES_CHANGE_RANGE (NOTES_CHANGE_DIST * 2 + 1)
#define TEMPO_CHANGE_DIST 4 /* How many semiquavers we can add or sutract during an evolutionary step */
#define TEMPO_CHANGE_RANGE (TEMPO_CHANGE_DIST * 2 + 1)
#define MIDDLE_NOTE 64 /* FIXME characterize instrument */
#define MAX_TEMPO 16 /* More is too much */
#define MAX_NOTE 128 /* Maximum pitch available */

struct bigram {
	uint8_t fst;
	uint8_t snd;
};

struct BigramComparer{
	bool operator()(const struct bigram & Left, 
					const struct bigram & Right) const {
			if (Left.fst == Right.fst)
				return (Left.snd < Right.snd);
			else return (Left.fst < Right.fst);
		}
};

typedef map<uint8_t, int>::iterator it_uni;

int genetic_loop(struct piece_s *ginitial, struct piece_s *ggoal);

#endif

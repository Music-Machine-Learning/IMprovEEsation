/*****************************************************************************/
/* Improveesation general constants header file.                             */
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
#ifndef _CONST_H
#define _CONST_H

#define DB_CONN_TIMEOUT "10"

#define NSEMITONES 12
#define PROB_ARR_SIZE NSEMITONES + 1
#define OCTAVE_MAX_JUMP 2
#define MIDI_NOCTAVES  8 /* From A0 to C8 */
#define MIDI_FIRST_NOTE 21 /* A0 */
#define MIDI_LAST_NOTE 108 /* C8 */
#define MIDI_REST_NOTE	128

#define NO_COUPLING 0
#define MAX_CHORD_SIZE 8
#define MIN_CHORD_SIZE 2
#define CHORD_LOOP_DECISIONS MAX_CHORD_SIZE * 3

enum {
	RYTHMIC,
	SOLO
};

enum {
	QUARTER_ARG_POS = 0,
	QUARTER_ARG_INSTR,
	QUARTER_ARG_CNOTE,
	QUARTER_ARG_CMODE,
	QUARTER_ARG_GENRE,
	QUARTER_ARG_DYNA,
	QUARTER_ARG_MOOD,
	QUARTER_ARG_SCALE,
	QUARTER_ARG_SOLO,
	QUARTER_ARG_LAST
};

#define QUARTER_ARG_FIRST QUARTER_ARG_POS
#define QUARTER_QUERY_ARGS (QUARTER_ARG_LAST - QUARTER_ARG_FIRST)

#endif /* _CONST_H */

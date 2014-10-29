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
#define MIDI_FIRST_NOTE 12 /* C0 */
#define MIDI_LAST_NOTE 108 /* C8 */
#define MIDI_REST_NOTE	128
#define MIDI_MAX_VELOCITY 127

#define NO_COUPLING 0
#define MAX_CHORD_SIZE 8
#define MIN_CHORD_SIZE 2
#define SQS_IN_Q 4 /* number of semiquavers in one quarter */

#define N_TAGS 3

#define TRUE 1
#define FALSE 0

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

#define DIR_DEFAULT_PORT 50000
#define PLA_DEFAULT_PORT 50001

#define MEASURE_BPM_EOI 0

#define FLAG_MUSICIAN_NONE      0b00000000
#define FLAG_MUSICIAN_SOLOIST   0b00000001
#define FLAG_MUSICIAN_GENETIC   0b00000010

#define GENETIC_ROUNDS 1500
#define GENETIC_POOL_SIZE 512 // Watch out! 1/4 of this must be an even number

#define PIECE_START_SIZE 32

/* Drums */

/* This is here only to help to keep things in order */
enum {
	/* Cymbals */
	DRUM_CLOSED_HIHAT = 0,
	DRUM_OPEN_HIHAT,
	DRUM_CRASH,
	DRUM_RIDE,
	DRUM_SPLASH,
	DRUM_SNARE,
	DRUM_COWBELL,

	/* Drums */
	DRUM_HIGH_TOM,
	DRUM_MID_TOM,
	DRUM_FLOOR_TOM,
	DRUM_RIMSHOT,
	DRUM_BASS,
	DRUM_UNUSED,
	DRUM_LAST
};

/* Drum Translation (timidity standard) */
#define MIDI_DRUM_CLOSED_HIHAT 42
#define MIDI_DRUM_OPEN_HIHAT 46
#define MIDI_DRUM_CRASH 28
#define MIDI_DRUM_RIDE 51
#define MIDI_DRUM_SPLASH 55
#define MIDI_DRUM_SNARE 38
#define MIDI_DRUM_COWBELL 56
#define MIDI_DRUM_HIGH_TOM 48
#define MIDI_DRUM_MID_TOM 47
#define MIDI_DRUM_FLOOR_TOM 43
#define MIDI_DRUM_RIMSHOT 37
#define MIDI_DRUM_BASS 35

/* Compute drum size */
#define DRUM_FIRST DRUM_CLOSED_HIHAT
#define DEFAULT_DRUM_SIZE (DRUM_LAST - DRUM_FIRST)

/* A basical drumset */
#define DEFAULT_DRUMSET {\
	MIDI_DRUM_CLOSED_HIHAT,\
	MIDI_DRUM_OPEN_HIHAT,\
	MIDI_DRUM_CRASH,\
	MIDI_DRUM_RIDE,\
	MIDI_DRUM_SPLASH,\
	MIDI_DRUM_SNARE,\
	MIDI_DRUM_COWBELL,\
	MIDI_DRUM_HIGH_TOM,\
	MIDI_DRUM_MID_TOM,\
	MIDI_DRUM_FLOOR_TOM,\
	MIDI_DRUM_RIMSHOT,\
	MIDI_DRUM_BASS,\
}

#endif /* _CONST_H */

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

#define CHORD_MODE_MAXSIZE 10

/* IF USING TIMIDITY - now by default (so no ifdef) */
/* The instrument classification is located in /etc/timidity/freepats.cfg 
 * even the drum key code */
#define GRANDPIANO 0
#define BRITEPIANO 1
#define ELPIANO 2
#define RHODEPIANO 4
#define YAMAHA 5
#define HARPSICORD 6
#define CLAVINET 7
#define CELESTA 8
#define GLOCKENSPIEL 9
#define DRUMS 10 // THIS IS FALSE, just for parallelism
#define XYLOPHONE 13
#define TBELLS 14
#define DULCIMER 15
#define HAMMOND 16
#define CHURCH 19
#define ACCORDION 21
#define TANGO 23
#define NYLONGTR 24
#define STEELGTR 25
#define JAZZGTR 26
#define GUITAR 27
#define MELGTR 28
#define OVERDRIVER 29
#define DISTORTION 30
#define AQBASS 32
#define BASS 33
#define PICKBASS 34
#define FRETLESS 35
#define SLAP1 36
#define SLAP2 37
#define SYNTHBASS 38
#define VIOLIN 40
#define CELLO 42
#define TREMOLO 44
#define PIZZICATO 45
#define HARP 46
#define TIMPANI 47
#define MARCATO 48
#define OOH 53
#define TRUMPET 56
#define TROMBONE 57
#define TUBA 58
#define MUTEDTRMP 59
#define FRENCHHORN 60
#define BRASS 61
#define SOPRANOSAX 64
#define ALTOSAX 65
#define TENORSAX 66
#define BARITONESAX 67
#define OBOE 68
#define ENGLISHHORN 69
#define BASSOON 70
#define CLARINET 71
#define PICCOLO 72
#define FLUTE 73
#define RECORDER 74
#define PANFLUTE 75
#define BOTTLE 76
#define OCARINA 79
#define SQUAREWAVE 80
#define CHARANG 84
#define NEWAGE 88
#define HALOPAD 94
#define SWEEPPAD 95
#define CRYSTAL 98
#define GOBLINSUNICORN 101
#define ECHOVOICE 102
#define SITAR 104
#define STEELDRUMS 114
#define WOODBLOCK 115
#define GUITARFRET 120
#define SEASHORE 122
#define HELICOPTER 125

#define NSEMITONES 12
#define PROB_ARR_SIZE NSEMITONES + 1
#define MIDI_NOCTAVES  8 /* From A0 to C8 */
#define MIDI_FIRST_NOTE 21 /* A0 */
#define MIDI_LAST_NOTE 108 /* C8 */
#define MIDI_REST_NOTE	128

#define NO_COUPLING 0

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

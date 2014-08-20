/*****************************************************************************/
/* Improveesation general structs header file.                               */
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
#ifndef _IMPROVEESATION_STRUCTS_H
#define _IMPROVEESATION_STRUCTS_H

#include <linux/list.h>
#include <stdint.h>

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
#define MIDI_NOCTAVES  7
#define MIDI_FIRST_NOTE 24
#define MIDI_REST_NOTE	128

enum {
	RYTHMIC,
	SOLO
};

struct subscription_s
{
	uint32_t coupling;
	uint8_t instrument_class;
	uint8_t soloer;

	/* For the storage of the subscriptions */
	int connection;
	struct list_head list;
};

/* measure substructs */
struct tempo_s {
	uint8_t upper;
	uint8_t lower;
};
/**
 * The successions of tonal zones in a measure
 * The mode bitmap (11th...0th) identifies the scale */
struct tonal_zone_s {
	uint16_t note;
	uint16_t scale;
};

/**
 * The successions of chords in a measure.
 * The mode bitmap (11th...0th) identifies the chord mode */
struct chord_s {
	uint16_t note; 
	uint16_t mode;
};

/**
 * A string of 3 tags parted by a ; char
 * (one field per each tag level dynamin, genre, mood). */
struct tags_s {
	unsigned int size;  /* string size */
	char *payload; /* "genre;dynamics;mood\0" */
};

struct measure_s {
	uint8_t bpm;
	uint32_t soloist_id;

	/**
	 * The succession of chords in a measure.
	 * The bitmap goes from the 11th to 0th bit,
	 * where each bit defines the chord grade */
	struct tempo_s tempo;

	/**
	 * The successions of tonal zones in a measure.
	 * The mode bitmap (11th...0th) identifies the scale */
	struct tonal_zone_s  *tonal_zones;
	/* size of the current time signature upper value */

	/**
	 * The successions of chords in a measure.
	 * The mode bitmap (11th...0th) identifies the chord mode */
	struct chord_s *chords;
	/*size of the current time signature upper value */

	/**
	 * A string of 3 tags parted by a ; char
	 * (one field per each tag level dynamin, genre, mood). */
	struct tags_s tags;
};

struct notes_s {
	/* the actual note height expressed in midi format */
	uint8_t note;
	/* the duration of the note */
	uint8_t tempo;
	/**
	 * incremental number of notes inside the measure
	 * used to create chords:
	 * some notes with the same id start at the same time) */
	uint8_t id;
	/* boolean: is this note a part of a triplet? */
	uint8_t triplets;
};

struct play_measure_s {
	uint32_t id;
	uint32_t size;
	uint32_t musician_id;
	struct notes_s *measure;
};

struct measure_pattern_s {
	int stepnumber;
	int *steps;
	char **modes;
	char *dynamics;
};

struct impro_variant_s {
	int first;
	int last;
	char **variants; /* NULL Terminated */
};

struct pattern_s {
	int measures_count;
	char **moods; /* NULL-terminated */
	struct measure_pattern_s *measures;
	int variants_size;
	struct impro_variant_s *variants;
};

struct semiquaver_s{
	uint8_t position;
	uint8_t quarter;
	uint8_t velocity_min;
	uint8_t velocity_max;
	float pchange; /* change probability */
	float pchange_3qrt; /* triplets pchange quarters */
	float pchange_3qvr; /* triplets pchange quavers */
	float pchange_3smq; /* triplets pchange semiquavers */
	float *pnote; /* Probability of note selection p[0] is a rest */
};

#endif /* _IMPROVEESATION_STRUCTS_H */

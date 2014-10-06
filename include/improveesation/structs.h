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

#include <improveesation/const.h>
#include <linux/list.h>
#include <stdint.h>

#include <map>

using namespace std;

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

	/*size of the current time signature upper value */
	struct tempo_s tempo;

	/* Arguments priorities for quarter selection.
	 * Higher is the index, higher is the priority. */
 	int prioargs[QUARTER_QUERY_ARGS]; 
	
	/**
	 * The successions of tonal zones in a measure.
	 * The mode bitmap (11th...0th) identifies the scale */
	struct tonal_zone_s  *tonal_zones;
	/* size of the current time signature upper value */

	/**
	 * The successions of chords in a measure.
	 * The mode bitmap (11th...0th) identifies the chord mode */
	struct chord_s *chords;

	/**
	 * A string of 3 tags parted by a ; char
	 * (one field per each tag level dynamin, genre, mood). */
	struct tags_s tags;
};

struct notes_s {
	/* the duration of the note */
	uint8_t tempo;
	/**
	 * incremental number of notes inside the measure
	 * used to create chords:
	 * some notes with the same id start at the same time) */
	uint8_t id;
	/* boolean: is this note a part of a triplet? */
	uint8_t triplets;
	/* how many notes the chord contains */
	uint8_t chord_size;
	/* the notes array that should be played simultaneously */
	uint8_t notes[MAX_CHORD_SIZE]; 
};

struct play_measure_s {
	uint32_t id;
	uint32_t size;
	uint32_t musician_id;
	uint8_t unchanged_fst;
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

struct musician_fields_s{
	int octave_min;
	int octave_max;
	int prev_octave;
	int play_chords;
};

#endif /* _IMPROVEESATION_STRUCTS_H */

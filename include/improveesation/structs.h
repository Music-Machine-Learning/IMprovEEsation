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

/* XXX Remove that and include midi headers! (ALSA?) */
#define GUITAR 1

enum {
	RYTHMIC,
	SOLO
};

struct subscription
{
	uint32_t coupling;
	uint8_t instrument_class;
	uint8_t soloer;

	/* For the storage of the subscriptions */
	int connection;
	struct list_head list;
};

/* measure substructs */
struct tempo_t {
	uint8_t upper;
	uint8_t lower;
};
/**
 * The successions of tonal zones in a measure
 * The mode bitmap (11th...0th) identifies the scale */
struct tonal_zone_t {
	uint16_t note;
	uint16_t scale;
};

/**
 * The successions of chords in a measure.
 * The mode bitmap (11th...0th) identifies the chord mode */
struct chord_t{
	uint16_t note; 
	uint16_t mode;
};

/**
 * A string of 3 tags parted by a ; char
 * (one field per each tag level dynamin, genre, mood). */
struct tags_t {
	unsigned int size;  /* string size */
	char *payload; /* "genre;dynamics;mood\0" */
};

struct measure
{
	uint8_t bpm;
	uint32_t soloist_id;

	/**
	 * The succession of chords in a measure.
	 * The bitmap goes from the 11th to 0th bit,
	 * where each bit defines the chord grade */
	struct tempo_t tempo;

	/**
	 * The successions of tonal zones in a measure.
	 * The mode bitmap (11th...0th) identifies the scale */
	struct tonal_zone_t  *tonal_zones;
	/* size of the current time signature upper value */

	/**
	 * The successions of chords in a measure.
	 * The mode bitmap (11th...0th) identifies the chord mode */
	struct chord_t *chords;
	/*size of the current time signature upper value */

	/**
	 * A string of 3 tags parted by a ; char
	 * (one field per each tag level dynamin, genre, mood). */
	struct tags_t tags;
};

struct notes {
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

struct play_measure
{
	uint32_t id;
	struct notes measure[4];
};


#endif /* _IMPROVEESATION_STRUCTS_H */

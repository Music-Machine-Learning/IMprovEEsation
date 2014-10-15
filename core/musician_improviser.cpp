/*****************************************************************************/
/* Musician's decision functions                                             */
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

#include <stdio.h>
#include <stdlib.h>
#include <improveesation/structs.h>
#include <improveesation/utils.h>
#include <improveesation/musician_core.h>
#include <improveesation/const.h>

/* Decide a note scanning an array of 13 probability values (one is the rest) */
int decide_note(float *pnote)
{
	/* Create a prefix sums array starting from the pnote array.
	 * Then get a random number and scan the prefix sums array until a 
	 * value lesser or equal than the random number is found. 
	 * The index of the found value will be the index of the 
	 * decided note in the pnote array. */
	int i;
	float r, pref[PROB_ARR_SIZE];
	
	pref[0] = pnote[0];
	for (i = 1; i < PROB_ARR_SIZE; i++) 
		pref[i] = pref[i - 1] + pnote[i];
	
	r = (float)rand() / (float)(RAND_MAX);
	for (i = 0; r > pref[i]; i++);
	
	return i;
}

int decide_octave(int octave_min, int octave_max)
{
	int octave;

	int ojump, i;
	float r;
	/* 15% 20% 30% 20% 15% */
	/* TODO find a way to mathematically weight the array */
	float pref[5] = {0.15, 0.35, 0.65, 0.85, 1.0};
	
	printf("omax, omin (%d, %d)\n", octave_max, octave_min);

	if (mfields.prev_octave == -1) {
		/* Start from around the middle octave. 
		   TODO let the user choose the start octave */
		octave = (octave_max - octave_min) / 2 + octave_min;
	} else {
		octave = mfields.prev_octave;
	}

	r = (float)rand() / (float)(RAND_MAX);
	for (i = 0; r > pref[i]; i++);

	/* -2, -1, 0, +1, +2*/
	ojump = i - 2;
	octave += ojump; 
	
	printf("octave_jump %d\n", ojump);
	
	if (octave > octave_max) 
		octave = octave_max;
	
	if (octave < octave_min) 
		octave = octave_min;
	printf(" octave %d\n", octave);

	mfields.prev_octave = octave;

	return octave;
}
/* Decide a chord reading the chord information from the director. */
int decide_chord(struct notes_s *chord, struct measure_s *minfo, int q_idx)
{
	int chord_note, chord_mode, key_note, i, max, n_done, 
	    octave, nidx, chord_width;
	int chord_mode_arr[NSEMITONES], tmp_notes[MAX_CHORD_SIZE];

	key_note = minfo->tonal_zones[q_idx].note;
	chord_note = minfo->chords[q_idx].note;
	chord_mode = minfo->chords[q_idx].mode;	
	chord_width = 1;


	/* Convert the chord mask into an array, each value of the array 
	   corresponds to a possible note that can be present in the chord */
	for (i = 0, n_done = 0; i < NSEMITONES; i++) {
		nidx = chord_mode >> i;
		if (nidx & 1) {
			chord_mode_arr[n_done] = i;
			n_done++;
		}
	}
	chord_mode_arr[n_done] = -1;
	
	shuffle_array(chord_mode_arr, (size_t)n_done);

	/* Randomly decide the size of the chord, but it shouln't be bigger than
	   the given chord mask or a pre-choosen max size */
	max = MAX_CHORD_SIZE;
	
	if (n_done < MAX_CHORD_SIZE)
		max = n_done;

	chord->chord_size = (rand() % (max - MIN_CHORD_SIZE)) + MIN_CHORD_SIZE;

	octave = decide_octave(mfields.octave_min, mfields.octave_max);
	

	/* From the set of the given chords note decide the note to put in the chord */
	for (i = 0; i < chord->chord_size; i++) {
		chord->notes[i] = 0;
		tmp_notes[i] = chord_mode_arr[i];
		if (i > 0) {
			if (tmp_notes[i] < tmp_notes[i - 1]) 
				chord->notes[i] = (NSEMITONES - tmp_notes[i - 1]);
			chord->notes[i] += chord->notes[i - 1] + tmp_notes[i];
		} else { 
			chord->notes[i] = tmp_notes[i];
			chord->notes[i] += key_note + chord_note + MIDI_FIRST_NOTE 
				+ (octave * NSEMITONES);
		}
	}
	
	int max_dist = 0;
	int move_direction = 0;
	int move_octave = 0;
	for (i = 0; i < chord->chord_size; i++) {
		int dist = 0;
		if (chord->notes[i] > MIDI_LAST_NOTE) {
			move_direction = 1;
			dist = chord->notes[i] - MIDI_LAST_NOTE;
			if (dist > max_dist)
				max_dist = dist;
		} else if (chord->notes[i] < MIDI_FIRST_NOTE) {
			move_direction = -1;
			dist = MIDI_FIRST_NOTE - chord->notes[i];
			if (dist > max_dist)
				max_dist = dist;
		}
	}
	
	if (max_dist > 0){
		move_octave = max_dist / NSEMITONES + 1;
		if (move_direction == 1)
			octave -= move_octave;
		else if (move_direction == -1)
			octave += move_octave;
	
		for (i = 0; i < chord->chord_size; i++) {
			if (move_direction == 1)
				chord->notes[i] -= NSEMITONES * move_octave;
			else if (move_direction == -1)
				chord->notes[i] += NSEMITONES * move_octave;
		}
	}
	
	mfields.prev_octave = octave;

	if (n_done > MAX_CHORD_SIZE || chord->chord_size > MAX_CHORD_SIZE || 
	    n_done < MIN_CHORD_SIZE || chord->chord_size < MIN_CHORD_SIZE) {
		fprintf(stderr, "Something wrong in decide_chord\n");
		return -1;
	}
}

/* Decide the velocity value between the range limits */
int decide_velocity(int vmin, int vmax)
{
	int velocity;

	velocity = (rand() % (vmax - vmin)) + vmin;

	if (velocity > MIDI_MAX_VELOCITY || velocity < 0) {
		fprintf(stderr, "Velocity out of range\n");
	 	return -1;
	}

	return velocity;
}

/*****************************************************************************/
/* Midi files outputting library                                             */
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

#ifndef MIDI_WRITER_H

#include <stdint.h>
#include <improveesation/player_core.h>

#define MIDI_WRITE_DEV_ONLY     0b01
#define MIDI_WRITE_FILE_ONLY    0b10
#define MIDI_WRITE_BOTH         0b11

#define MIDI_EVENT_SIZE         3

/*
 * initialize midi file
 * returns TRUE if something bad happened
 */
int initFile(char *fname, int **instruments, uint8_t bpm, int *midiDev, struct list_head *musicians, uint32_t musiciansCount);

/*
 * write midi note to file, event represents note, meta are STM metadata infos and sysex are System Exclusive Events
 * returns TRUE if something bad happened
 */
int writeNote(unsigned int atom, unsigned char* event, uint8_t mode=MIDI_WRITE_BOTH, uint8_t size=MIDI_EVENT_SIZE);

/*
 * close and save midi file
 * returns TRUE if something bad happened
 */
int closeFile();

#define MIDI_WRITER_H

#endif // MIDI_WRITER_H

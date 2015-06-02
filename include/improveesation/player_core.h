/*****************************************************************************/
/* Player's core facilities declarations                                   */
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

#ifndef PLAYER_CORE_H
#define PLAYER_CORE_H

/* IF USING TIMIDITY - now by default (so no ifdef) */
/* The instrument classification is located in /etc/timidity/freepats.cfg 
 * even the drum key code */
#define SELECT_INSTRUMENT(c) (0xC0 + c)
#define KEY_DOWN(c)	(0x90 + c)
#define KEY_UP(c)	(0x80 + c)

#include <improveesation/structs.h>

void fill_test_musician(subscription_s *new_musician, int prog);
void fill_test_measure(struct play_measure_s *note_list, int prog, uint32_t musicians_num);
int midi_init(struct list_head *musicians, uint32_t musicians_num, int * fd, char * dev, char *fname);
void play_measure(struct play_measure_s *note_list, struct list_head *musicians, uint32_t musicians_num, int fd);
void smorza_incosa(int fd);

#endif //PLAYER_CORE_H

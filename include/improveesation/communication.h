/*****************************************************************************/
/* Communication library for the improveesation components                   */
/* header file.                                                              */
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
#ifndef IMPROVEESATION_COMMUNICATION_H
#define IMPROVEESATION_COMMUNICATION_H

#include <improveesation/structs.h>
#include <linux/list.h>
#include <sys/uio.h>
#include <stdint.h>

#include <exception>

#define PLAYER_ID -1

/* Musician */
uint32_t send_subscription(int director, struct subscription_s *proposal);

#ifdef __cplusplus /* polymorphism */
uint32_t send_subscription(int director, uint32_t coupling,
			   uint8_t instrument_class, uint8_t soloer);
#else /* C retrocompatibilty */
uint32_t send_subscription_fields(int director, uint32_t coupling,
			   uint8_t instrument_class, uint8_t soloer);
#endif

void recv_measure(int director, struct measure_s *newMeasure);

void send_to_play(int player, int director,
		  struct play_measure_s *measure);

/* Midi player */
uint32_t recv_num_of_musicians(int net_handler);
void recv_to_play(struct play_measure_s *note_list, struct list_head *sources);
void send_ack(int conn_socket);

/* Director */
uint32_t recv_player(int conn_socket);
void recv_subscription(int conn_socket, struct subscription_s *new_musician);
void send_num_of_musicians(int player_addr, uint32_t musicians_count);
void send_id(int musician_conn, uint32_t id);
void broadcast_measure(struct measure_s *next_measure, struct list_head *dests);
void free_play_measure(struct play_measure_s *nl, int size);
void sync_all(struct list_head *dests);

/* Exceptions */

/* network exception (i.e. connection error) */
class network_exception: public std::exception
{
};

/* End of communication exception, the improvisation ended. */
class end_of_improvisation_exception: public network_exception
{
};

#endif /* improveesation/communication.h */

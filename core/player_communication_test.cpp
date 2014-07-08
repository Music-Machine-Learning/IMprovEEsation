/*****************************************************************************/
/* Test MIDI player program.                                                 */
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
/** TODO
 * real improveesation implementation
 * parameters from the cli */
#include <stdio.h>
#include <improveesation/communication.h>
#include <improveesation/structs.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/list.h>

#include <time.h>

int director_socket;
int net_handler;
LIST_HEAD(musicians);

void cleanup(void)
{
	struct subscription_s *curr_musician, *tmp_musician;
	/* Cleanup and exit */
	list_for_each_entry_safe(curr_musician, tmp_musician,
				 &musicians, list) {
		list_del(&curr_musician->list);
		close(curr_musician->connection);
		free(curr_musician);
	}

	close(director_socket);
	close(net_handler);
}

int net_init(int port, const char *addr)
{
	int gsocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sin;

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	/* XXX parse address */
	sin.sin_addr.s_addr = INADDR_ANY;

	int t = 1;

	setsockopt(gsocket, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t));

	if (bind(gsocket, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)))
	{
		perror("bind");
		return 1;
	}

	if (listen(gsocket, 255))
	{
		perror("listen");
		return 1;
	}
	return gsocket;
}

void easter_egg_print(void)
{
	/* let's get obfuscated! */
	switch (rand() % 20) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 17:
			printf("I'm just kidding! I'm not playing anything\n");
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 18:
			printf("do you expect some midi from me?\n");
			break;
		case 19:
		{
			char a[] = {0x54, 0x72, 0x6f, 0x6c, 0x6f, 0x6c, 0x6f,
				   0x6c, 0x6f, 0x6c, 0x6f, 0x6c, 0x6c, 0x6f,
				   0x6c, 0x6f, 0x6c, 0x6e, 0x25, 0x73,
				   0x0};
			char b[] = {0x6f, 0x6c, 0x6c, 0x6f,
				   0x6c, 0x6f, 0x6c, 0x6f, 0x27, 0x21, 0x0d,
				   0x0a, 0x0};
			printf(a, b);
			break;
		}
	}
}

int main(int argc, char **argv)
{
	struct sockaddr_in sout;
	int i = 0;
	struct play_measure_s *note_list = NULL;

	srand(time(NULL));

	sout.sin_family = AF_INET;
	sout.sin_port = htons(50000);
	inet_pton(AF_INET, "127.0.0.1", &sout.sin_addr.s_addr);

	director_socket = socket(AF_INET, SOCK_STREAM, 0);

	net_handler = net_init(50001, "127.0.0.1");

	if (connect(director_socket, (struct sockaddr *) &sout,
				  sizeof(struct sockaddr_in))) {
		perror("connect");
		return 1;
	}

	send_subscription(director_socket, PLAYER_ID, 0, 0);
	printf("connected to director\n");
	uint32_t musicians_num = recv_num_of_musicians(director_socket);
	printf("initializing a %d components improvisation.\n", musicians_num);

	note_list = (struct play_measure_s *) calloc(musicians_num, sizeof(struct play_measure_s));

	for (i = 0; i < musicians_num; i++) {
		struct subscription_s *new_musician = (struct subscription_s *) malloc(sizeof(struct subscription_s));
		printf("waiting for a musician\n");
		recv_subscription(net_handler, new_musician);
		
		printf("player: got a new musician\n\tcoupling: %d\n\tinstrument_class: %d\n\tsoloer: %d\n\tconnection :%d\n",
		       new_musician->coupling, new_musician->instrument_class,
		       new_musician->soloer, new_musician->connection);
		list_add_tail(&new_musician->list, &musicians);

		send_ack(new_musician->connection);
	}

	atexit(cleanup);
	/* main loop */
	printf("main loop\n");
	for (i = 0;; i++) {
		try {
			recv_to_play(note_list, &musicians);
		} catch (end_of_improvisation_exception e) {
			break;
		}

		printf("%d ", i);
		easter_egg_print();
	}

	
	return 0;
}

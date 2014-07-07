/*****************************************************************************/
/* Test Director program.                                                    */
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
/** TODO:
 *	real improveesation implementation (main loop)
 *	parameters from the cli
 */	
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>

#include <improveesation/structs.h>
#include <improveesation/communication.h>

#include <linux/list.h>

#include <time.h>
#include <string.h>

int net_handler;
int player;
LIST_HEAD(musicians);

void cleanup(void)
{
	struct subscription *curr_musician, *tmp_musician;
	/* Cleanup and exit */
	list_for_each_entry_safe(curr_musician, tmp_musician,
				 &musicians, list) {
		list_del(&curr_musician->list);
		close(curr_musician->connection);
		free(curr_musician);
	}

	close(player);
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

int main(int argc, char **argv)
{
	uint32_t musicians_num;
	int i;

	if (argc < 2) {
		fprintf(stderr, "%s <number of musicians>\n", argv[0]);
		return 1;
	}

	musicians_num = atoi(argv[1]);

	net_handler = net_init(50000, "127.0.0.1");

	player = get_player(net_handler);

	printf("got midi player (%d)\n", player);
	send_num_of_musicians(player, musicians_num);

	/* Build musicians list */
	for (i = 0; i < musicians_num; i++) {
		struct subscription *new_musician = (struct subscription *) malloc(sizeof(struct subscription));
		printf("waiting for a musician\n");
		get_subscription(net_handler, new_musician);

		printf("director: got a new musician\n\tcoupling: %d\n\tinstrument_class: %d\n\tsoloer: %d\n\tconnection :%d\n",
		       new_musician->coupling, new_musician->instrument_class,
		       new_musician->soloer, new_musician->connection);

		list_add_tail(&new_musician->list, &musicians);

		/* TODO real call */
		send_id(new_musician->connection, new_musician->coupling + 1);
	}

	atexit(cleanup);

	/* main loop */
	printf("main loop\n");
	/* For now do it for 5 times */
	for (i = 0; i < 5; i++) {
		struct measure nm;

		memset(&nm, 0, sizeof(struct measure));
		
		nm.bpm = 200;
		nm.soloist_id = 2;

		nm.tags.payload = (char *) calloc(100, sizeof(char));
		strcpy(nm.tags.payload, "blues;ciao;test");
		nm.tags.size = strlen(nm.tags.payload);
		printf("picking measure\n");

		try {
			broadcast_measure(&nm, &musicians);
			sync_all(&musicians);
		} catch (end_of_improvisation_exception e) {
			break;
		}

		printf("musicians syncronized.\n");
		/* sleep to see if things block properly */
		sleep(1);
	}

	return 0;
}

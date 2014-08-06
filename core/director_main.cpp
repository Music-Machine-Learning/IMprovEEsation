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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>

#include <improveesation/structs.h>
#include <improveesation/communication.h>

#include <improveesation/director_core.h>

#include <linux/list.h>

#include <time.h>
#include <string.h>

int net_handler;
int player;
LIST_HEAD(musicians);
uint32_t *soloers;

class end_of_improvisation_exception eoi_ex;

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

/* TODO: the real call */
uint32_t calculateID(subscription_s *entry)
{
    return entry->coupling + entry->instrument_class;
}

int main(int argc, char **argv)
{
    /* For now do a full round of blues by default (12 measures)*/
    int measures_count = 12;
    uint32_t musicians_num, soloers_num = 0;
    int i, current_measure_num = 0, measures_per_section;

	if (argc < 2) {
        fprintf(stderr, "%s <number of musicians> [number of measures]\n", argv[0]);
		return 1;
	}

	musicians_num = atoi(argv[1]);

    soloers = new u_int32_t[musicians_num];

    if (argc > 2)
        measures_count = atoi(argv[2]);

	net_handler = net_init(50000, "127.0.0.1");

	player = recv_player(net_handler);

	printf("got midi player (%d)\n", player);

	/* Build musicians list */
	for (i = 0; i < musicians_num; i++) {
		struct subscription_s *new_musician = (struct subscription_s *) malloc(sizeof(struct subscription_s));
		printf("waiting for a musician\n");
		recv_subscription(net_handler, new_musician);

		printf("director: got a new musician\n\tcoupling: %d\n\tinstrument_class: %d\n\tsoloer: %d\n\tconnection :%d\n",
		       new_musician->coupling, new_musician->instrument_class,
		       new_musician->soloer, new_musician->connection);

        uint32_t newId = calculateID(new_musician);

        list_add_tail(&new_musician->list, &musicians);
        if(new_musician->soloer == SOLO){
            soloers[soloers_num] = newId;
            soloers_num ++;
        }

        send_id(new_musician->connection, newId);
	}
    printf("registered %d musicians, of wich %d are soloers\n", musicians_num, soloers_num);

    send_num_of_musicians(player, musicians_num);

    atexit(cleanup);

    init_director_core("blues", "standard", soloers_num, soloers);

	/* main loop */
	printf("main loop\n");

    for (i = 0; i < measures_count; i++) {
		struct measure_s nm;

		memset(&nm, 0, sizeof(struct measure_s));
		
        printf("decide next measure:\n");
        measures_per_section = decide_next_measure(&nm, current_measure_num);

        if(current_measure_num < measures_per_section)
            current_measure_num = (current_measure_num+1) % measures_per_section;
        else
            current_measure_num = 0;

        printf("broadcasting measure...\n");
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

    //FIXME: ugly solution
    try{
        throw eoi_ex;
    } catch (end_of_improvisation_exception e){
        printf("we have come to an end\n");
    }

	return 0;
}

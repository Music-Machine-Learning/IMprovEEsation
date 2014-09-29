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


#include <stdio.h>
#include <improveesation/communication.h>
#include <improveesation/structs.h>

#include <improveesation/player_core.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/list.h>

#include <time.h>

#include <map>

using namespace std;

int director_socket;
int net_handler;
int fd;
/* The single column contains instrument class / channel */
uint8_t instrument_db[16];
LIST_HEAD(musicians);


void cleanup(void)
{
	struct subscription_s *curr_musician, *tmp_musician;
	/* Cleanup and exit */
	
	smorza_incosa(fd);
	list_for_each_entry_safe(curr_musician, tmp_musician,
				 &musicians, list) {
		list_del(&curr_musician->list);
		close(curr_musician->connection);
		free(curr_musician);
	}
	close(director_socket);
	close(net_handler);
	close(fd);
}

/* Configure the connection listening for the director */
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

	/* Verify if it's possible to listen */
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

/* Main Flow */
int main(int argc, char **argv)
{
	
	struct sockaddr_in sout;
	int i = 0; // gp counter
	struct play_measure_s *note_list = NULL;
	bool test_flag = FALSE;
	uint32_t musicians_num;
	
	/* Check if the test flag is active */
	if (argc <= 2){
		printf("./player <midi_dev> <seconds-to-sleep>(-1 if we want to wait the impro to end) [--test]\nThe device's name is located is usually similar to /dev/midi1 or /dev/snd/midiC2D0.\nRemember to use -D DEBUG in compiling to obtain very verbose output.\n");
		exit(0);
	} else if ((argc > 3) && !strcmp(argv[3], "--test")){
		test_flag = TRUE;
		printf("Starting testset...\n");
	}

	/* Initiate the random support */
	srand(time(NULL));

	/* Configure the parameters for the socket player-director */
	sout.sin_family = AF_INET;
	sout.sin_port = htons(50000);
	inet_pton(AF_INET, "127.0.0.1", &sout.sin_addr.s_addr);

	director_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	if (!test_flag){
		/* Obtain the listener socket for musicians */
		net_handler = net_init(50001, "127.0.0.1");

		/* Connect the socket to the director */
		if (connect(director_socket, (struct sockaddr *) &sout,
					  sizeof(struct sockaddr_in))) {
			perror("connect");
			return 1;
		}
		
		/* PROTOCOL: send the subscription to the director */
		send_subscription(director_socket, PLAYER_ID, 0, 0);
		printf("Connected to director!\n");
		/* PROTOCOL: block until the number of musicians is obtained */
		musicians_num = recv_num_of_musicians(director_socket);
	} else {
		musicians_num = 3;
	}
	printf("Initializing a %d components improvisation.\n", musicians_num);

	/* Give space for the notes to be played by each musician */
	note_list = (struct play_measure_s *) calloc(musicians_num, sizeof(struct play_measure_s));

	for (i = 0; i < musicians_num; i++) {
		struct subscription_s *new_musician = (struct subscription_s *) malloc(sizeof(struct subscription_s));
		printf("Waiting for a musician... ");
		
		if (!test_flag){
			/* PROTOCOL: from the listener socket wait for musicians subscriptions */
			recv_subscription(net_handler, new_musician);
		} else {
			fill_test_musician(new_musician, i);
		}
		
		
		printf("got a new musician!\n\tcoupling: %d\n\tinstrument_class: %d\n\tsoloer: %d\n\tconnection :%d\n",
		       new_musician->coupling, new_musician->instrument_class,
		       new_musician->soloer, new_musician->connection);
		list_add_tail(&new_musician->list, &musicians);
		
		if (!test_flag){
			send_ack(new_musician->connection);
		}
	}
	
	printf("\n");
	if (!midi_init(&musicians, musicians_num, &fd, argv[1])){
		return 1;
	};
		

	/* Execute a cleanup of the structures at the exit */
	atexit(cleanup);
	if(argv[2] >= 0)
		usleep(atoi(argv[2])*1000000);
	
	/* Main loop */
	printf("Main loop initiated!\n");
	for (i = 0;; i++) {
		try {
			if (!test_flag){
				/* PROTOCOL: get notes to play, stored by musician in the list */
				recv_to_play(note_list, &musicians);
			} else {
				fill_test_measure(note_list, i, musicians_num);
				if (i > 100){
					class end_of_improvisation_exception eoi_ex;
					throw eoi_ex;
					}
			}
		} catch (end_of_improvisation_exception e) {
			break;
		}

		printf("\tPlaying measure %d\n ", i);
		play_measure(note_list, &musicians, musicians_num, fd);
	}
	return 0;
}

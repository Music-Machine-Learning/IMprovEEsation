/*****************************************************************************/
/* Main Musician program.                                                    */
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
#include <improveesation/musician_core.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include <fcntl.h>

int director_socket;
int player_socket;

void cleanup(void)
{
	close(director_socket);
	close(player_socket);
}

void exit_usage(char *usage)
{
	fprintf(stderr, usage);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int i, j, coupling, midi_class, soloer;
	struct sockaddr_in sout_director, sout_player;
	struct notes_s *nt;

	char *usage; 
	asprintf(&usage, "%s <coupling> <midi-class> <soloer>\n", argv[0]);
	
	if (argc < 4){
		exit_usage(usage);
	} else {
		coupling = atoi(argv[1]);
		midi_class = atoi(argv[2]);
		soloer = atoi(argv[3]);
	}

	srand(time(NULL));

	sout_director.sin_family = AF_INET;
	sout_director.sin_port = htons(50000);
	inet_pton(AF_INET, "127.0.0.1", &sout_director.sin_addr.s_addr);

	sout_player.sin_family = AF_INET;
	sout_player.sin_port = htons(50001);
	inet_pton(AF_INET, "127.0.0.1", &sout_player.sin_addr.s_addr);

	director_socket = socket(AF_INET, SOCK_STREAM, 0);
	player_socket = socket(AF_INET, SOCK_STREAM, 0);

	atexit(cleanup);

	if (connect(director_socket, (struct sockaddr *) &sout_director,
				  sizeof(struct sockaddr_in))) {
		perror("connect director");
		return 1;
	}

	if (connect(player_socket, (struct sockaddr *) &sout_player,
				  sizeof(struct sockaddr_in))) {
		perror("connect player");
		return 1;
	}
	
	/* XXX build it with an appropiate method */
	uint32_t myid = send_subscription(director_socket,
				coupling, midi_class, soloer);

	printf("connected to director (%d)\n", myid);

	send_subscription(player_socket, myid, midi_class, soloer);

	/* set the socket to non blocking */
	fcntl(player_socket, F_SETFL, 
			fcntl(player_socket, F_GETFL, 0) | O_NONBLOCK);

	printf("connected to player\n");

	/* main loop */
	for (i = 0; ;i++) {
		/* TODO load the struct */
		struct measure_s nm;
		/* TODO load the struct */
		struct play_measure_s pm;
		
		
		memset(&nm, 0, sizeof(struct measure_s));
		memset(&pm, 0, sizeof(struct play_measure_s));

		try {
			recv_measure(director_socket, &nm);

			printf("new measure got,\
			       playing the random note:\n\tid: %d (%s)\n", i, 
			       nm.tags.payload);
			
			compose_measure(&pm, &nm);
			pm.id = i;
			pm.musician_id = myid;
			printf("Measure: id: %d, size: %d, musid %d\n", 
					pm.id, pm.size, pm.musician_id);
			
			printf("Note\tidx\tlength\tmidi\ttriplets\n");
			for (j = 0; j < pm.size; j++){
				nt = &(pm.measure[j]);
				printf("\t%d\t%d\t%d\t%d\n",
					nt->id, 
					nt->tempo, 
					nt->note, 
					nt->triplets);
			}
			
			send_to_play(player_socket, director_socket, &pm);
		} catch (end_of_improvisation_exception e) {
			fprintf(stderr, "EOI exception catched: exiting\n");
			break;
		}
	}

	return 0;
}

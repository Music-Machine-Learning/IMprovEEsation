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
#include <improveesation/db.h>
#include <improveesation/const.h>
#include <improveesation/musician_genetic.h>
#include <improveesation/musician_evolution.h>

#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#include <fcntl.h>

#include <getopt.h>
#include <netdb.h>
#include <errno.h>

class end_of_improvisation_exception eoi_ex;

int director_socket;
int player_socket;

void cleanup(void)
{
	close(director_socket);
	close(player_socket);
}

void separate_port(char *d, char **a, char **p)
{
	int i;

	for (i = 0; d[i] != ':' && d[i] != '\0'; i++)
		;

	*a = (char *) malloc(sizeof(char) * (i + 1));
	strncpy(*a, d, i);
	(*a)[i] = 0;

	if (d[i] == ':') {
		*p = (char *) malloc(sizeof(char) * (strlen(d + i) + 1));
		strcpy(*p, d + i + 1);
	}
}

void dns_query(char *dns_string, struct sockaddr_in *out,
		in_port_t default_port)
{
	struct addrinfo *hostInfo;
	char *port_string, *address_string;
	in_addr_t inaddr;
	in_port_t inport = default_port;

	out->sin_family = AF_INET;
	if (dns_string == NULL) {
		inaddr = htonl(INADDR_LOOPBACK);
		inport = htons(default_port);
		out->sin_port = inport;
		out->sin_addr.s_addr = inaddr;
		return;
	}

	separate_port(dns_string, &address_string, &port_string);

	if (strncasecmp(address_string, "none", 4) == 0) {
		inaddr = htonl(INADDR_NONE);
	} else if (strncasecmp(address_string, "localhost", 9) == 0) {
		inaddr = htonl(INADDR_LOOPBACK);
	} else if (strncasecmp(address_string, "any", 9) == 0) {
		inaddr = htonl(INADDR_ANY);
	} else if (getaddrinfo(address_string, NULL, NULL, &hostInfo) >= 0) {		
		while (hostInfo->ai_next) hostInfo = hostInfo->ai_next;

		inaddr = ((struct sockaddr_in *)(hostInfo->ai_addr))->sin_addr.s_addr;
		if (hostInfo)
			freeaddrinfo(hostInfo);

	} else {
		fprintf(stderr, "Error in main getaddrinfo (%s)",
			strerror(errno));
	}

	if (port_string) {
		inport = htons(atoi(port_string));
		free(port_string);
	}

	if (address_string)
		free(address_string);

	out->sin_port = inport;
	out->sin_addr.s_addr = inaddr;
}

void exit_usage(char *usage)
{
	fprintf(stderr, "%s", usage);
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int i, j, k;
	int coupling, midi_class, soloist, play_chords, genetic;
	int res, randfd;
	int c;
	int digit_optind = 0;
	int default_player = 1, default_director = 1;
	unsigned seed;
	struct sockaddr_in sout_director, sout_player;
	struct notes_s *nt;
	PGconn *dbh;
	char *usage, *samples_file_path;
	struct timeval tv;
 

	coupling = soloist = play_chords = genetic = 0;
	midi_class = -1; /* required arg */
	samples_file_path = NULL;

	asprintf(&usage, "Usage: %s [OPTION...] INSTRUMENT\n\n"
		"  -h, --help                print this help\n"
		"  -c, --coupling=COUPLING   set the coupling id\n"
		"  -C, --chords              enable the chord playing mode\n"
		"  -s, --soloist             enable the soloist playing mode\n"
		"  -p, --player=hostname     specify a remote player\n"
		"  -d, --director=hostname   specify a remote director\n\n"
		"  -g, --genetic=samplefile  enable the genetic loop mode\n",
			argv[0]);
	
	for (;;) {
		int current_optind = (optind ? optind : 1);
		int option_index = 0;

		static struct option long_options[] = {
			{ "help", no_argument, 0, 'h' },
			{ "coupling", required_argument, 0, 'c' },
			{ "chords", no_argument, 0, 'C'},
			{ "soloist", no_argument, 0, 's' },
			{ "player", required_argument, 0, 'p' },
			{ "director", required_argument, 0, 'd' },
			{ "genetic", required_argument, 0, 'g' },
			{ 0, 0, 0, 0 },
		};
		c = getopt_long(argc, argv, "hc:Csp:d:g:",
				long_options, &option_index);
		if (c == -1)
			break;
		switch(c) {
			case 'h':
				exit_usage(usage);
			case 'c':
				coupling = atoi(optarg);
				break;
			case 'C':
				play_chords = 1;
				break;
			case 's':
				soloist = 1;
				break;
			case 'p':
				default_player = 0;
				dns_query(optarg, &sout_player,
					  PLA_DEFAULT_PORT);
				printf("player: %s\n", optarg);
				break;
			case 'd':
				default_director = 0;
				dns_query(optarg, &sout_director,
					  DIR_DEFAULT_PORT);
				printf("director: %s\n", optarg);
				break;
			case 'g':
				genetic = 1;		
				asprintf(&samples_file_path, "%s", optarg);
				printf("Genetic mode activated\n");
				break;
			default:
				exit_usage(usage);
		}
	}

	if (default_director)
		dns_query(NULL, &sout_director,
			  DIR_DEFAULT_PORT);
	if (default_player)
		dns_query(NULL, &sout_player,
			  PLA_DEFAULT_PORT);

	printf("optind: %d\n", optind);
	if (argc - optind < 1) {
		printf("Which instrument do you want to play?\n");
		exit_usage(usage);
	} else {
		midi_class = atoi(argv[optind]);
	}


	printf("coup inst solo chords\n");
	printf("%d    %d     %d    %d\n", coupling, midi_class, soloist, play_chords);

	randfd = open("/dev/urandom", O_RDONLY);
	if (read(randfd, &seed, sizeof(seed)) < sizeof(seed)) {
		fprintf(stderr, "error in urandom read\n");
		return -1;
	}
	
	close(randfd);
	fprintf(stderr, "%u\n", seed);	
	if (gettimeofday(&tv, NULL) != -1)
		seed *= tv.tv_usec;
	
	srand(seed);

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
				coupling, midi_class, soloist);

	printf("connected to director (%d)\n", myid);

	send_subscription(player_socket, myid, midi_class, soloist);

	/* set the socket to non blocking */
	fcntl(player_socket, F_SETFL, 
			fcntl(player_socket, F_GETFL, 0) | O_NONBLOCK);

	printf("connected to player\n");
	
	if (musician_init(&dbh, coupling, midi_class, soloist, 
					myid, play_chords, genetic) == -1) {
		fprintf(stderr, "Initialization failed\n");
		exit(EXIT_FAILURE);
	}

	/* Initialize the genetic world if the genetic mode is enabled */
	if (musician_init_genetic(genetic, samples_file_path) == -1) {
		fprintf(stderr, "Initialization failed\n");
		exit(EXIT_FAILURE);
	}


	struct measure_s nm;
	struct play_measure_s pm, prev_pm;
	
	prev_pm.measure = pm.measure = NULL;
	memset(&pm, 0, sizeof(struct play_measure_s));
	memset(&nm, 0, sizeof(struct measure_s));	
	/* main loop */
	for (i = 0; ;i++) {
		
		free(prev_pm.measure);
		memset(&prev_pm, 0, sizeof(struct play_measure_s));
		
		prev_pm = pm;
		memset(&pm, 0, sizeof(struct play_measure_s));

		try {
			recv_measure(director_socket, &nm);

			printf("new measure got,\
			       playing the random note:\n\tid: %d (%s)\n", i, 
			       nm.tags.payload);
			
			res = compose_measure(&pm, &prev_pm,
					&nm, myid, soloist, dbh);
			if (res == -1){
				printf("exiting\n");
				return -1;
			}
				
			pm.id = i;
			pm.musician_id = myid;
			printf("Measure: id: %d, size: %d, musid %d\n", 
					pm.id, pm.size, pm.musician_id);
			
			int temposum = 0;	
			printf("Note\tidx\tlength\ttripl\tvelocity\tch_size\tnotes\n");
			for (j = 0; j < pm.size; j++){
				nt = &(pm.measure[j]);
				printf("\t%d\t%d\t%d\t%d\t%d\t[",
					nt->id, 
					nt->tempo, 
					nt->triplets,
					nt->velocity,
					nt->chord_size);
				temposum += nt->tempo;
				for (k = 0; k < nt->chord_size; k++)
					printf("%d, ", nt->notes[k]);
				printf("]\n");
			}
			printf("Tempo SUM: %d\n", temposum);	
			if (genetic) {
				store_gmeasure(&pm, &nm);
				send_sync_ack(director_socket);
			}
			else  {
				send_to_play(player_socket, director_socket, &pm);
			}

		} catch (end_of_improvisation_exception e) {
			fprintf(stderr, "EOI exception catched: exiting\n");
			break;
		}
		
		clear_measure(&nm);
	}
	
	if (genetic) {
		if (genetic_loop(&(mfields.ginitial), &(mfields.ggoal)) == -1) {
			fprintf(stderr, "Genetic loop failed\n");
			//exit(EXIT_FAILURE);
		}
		/* Clean the measure structure and re-fill it  
		 * with the new genetic stuff */
		free(pm.measure);	
		memset(&pm, 0, sizeof(struct play_measure_s));
		
		pm.id = 0;
		pm.size = mfields.ggoal.count;
		pm.musician_id = myid;
		pm.measure = (struct notes_s *)malloc(sizeof(struct notes_s) * 
			     pm.size);

		if (pm.measure == NULL) {
			fprintf(stderr, "Malloc error in main loop\n");
			exit(EXIT_FAILURE);
		}
		memcpy(pm.measure, mfields.ggoal.notes, 
		       sizeof(struct notes_s) * pm.size);

		/* TODO: send ggoal.notes to the player */
		send_to_play(player_socket, director_socket, &pm);	
		free(mfields.ginitial.notes);
		free(mfields.ggoal.notes);
	}


	return 0;
}

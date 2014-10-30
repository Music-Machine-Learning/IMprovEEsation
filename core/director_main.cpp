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
#include <improveesation/const.h>
#include <improveesation/utils.h>

#include <improveesation/director_core.h>

#include <linux/list.h>

#include <time.h>
#include <string.h>

#include <getopt.h>
#include <errno.h>

struct musician_registration_s {
	subscription_s *subscritpion;
	uint32_t id;
};

int net_handler;
int player;
LIST_HEAD(musicians);
LIST_HEAD(genetics);
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
		if(!list_empty(&musicians))  //FIXME: dirty fix, valgrind won't be pleased
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

uint32_t calculateID(subscription_s *entry, musician_registration_s *subs, int received)
{
	int i;
	uint32_t id = (1 << 8) + entry->instrument_class;
	for(i = 0; i < received; i++){
		if(entry->coupling != NO_COUPLING
				&& subs[i].subscritpion->coupling == entry->coupling
				&& subs[i].subscritpion->instrument_class == entry->instrument_class){
			id = subs[i].id;
			break;
		}
		if(subs[i].id == id)
			id += 1<<8;
		i++;
	}
	return id;
}

void copyMeasure(struct measure_s *dest, struct measure_s *src){
	int i;
	dest->bpm = src->bpm;
	dest->soloist_id = src->soloist_id;
	dest->tempo.upper = src->tempo.upper;
	dest->tempo.lower = src->tempo.lower;
	for(i = 0; i < QUARTER_QUERY_ARGS; i++){
		dest->prioargs[i] = src->prioargs[i];
	}
	dest->tonal_zones = (struct tonal_zone_s *) calloc(dest->tempo.upper, sizeof(struct tonal_zone_s));
	dest->chords = (struct chord_s *) calloc(dest->tempo.upper, sizeof(struct chord_s));
	for(i = 0; i < dest->tempo.upper; i++){
		dest->tonal_zones[i].note = src->tonal_zones[i].note;
		dest->tonal_zones[i].scale = src->tonal_zones[i].scale;
		dest->chords[i].note = src->chords[i].note;
		dest->chords[i].mode = src->chords[i].mode;
	}
	dest->tags.size = src->tags.size;
	dest->tags.payload = (char *) calloc(dest->tags.size, sizeof(char));
	strcpy(dest->tags.payload, src->tags.payload);
}

void secondLoop(struct measure_s *measure_list, int measures_count){
	int i;
	for (i = 0; i < measures_count; i++) {
		printf("broadcasting measure...\n");
		fflush(stdout);
		try {
			broadcast_measure(&(measure_list[i]), &musicians);
			sync_all(&musicians);
		} catch (end_of_improvisation_exception e) {
			break;
		}

		printf("musicians syncronized.\n");
		/* sleep to see if things block properly */
		sleep(1);
	}
}

int listLen(struct list_head *list){
	struct subscription_s *it, *tmp;
	int count = 0;
	list_for_each_entry_safe(it, tmp, list, list){
		count++;
	}
	return count;
}

int main(int argc, char **argv)
{
	/* For now do a full round of blues by default (12 measures)*/
	int measures_count = 12;
	uint32_t musicians_num, soloers_num = 0;
	int c, i, current_measure_num = 0, measures_per_section;
	musician_registration_s *registrations;
	struct measure_s nm, *stored_impro;
	bool genetic_process = false;
	struct subscription_s *new_musician, *tmp_musician;
	in_port_t port = DIR_DEFAULT_PORT;

	for (;;) {
		int option_index = 0;

		static struct option long_options[] = {
			{ "port", required_argument, 0, 'P' },
			{ 0, 0, 0, 0 },
		};
		c = getopt_long(argc, argv, "P:",
				long_options, &option_index);
		if (c == -1)
			break;
		switch(c) {
			case 'P':
				port = atoi(optarg);
				break;
			default:
				printf("option not recognized\n");
		}
	}
	if (argc - optind + 1 < 2) {
		fprintf(stderr, "%s <number of musicians> [number of measures]\n", argv[0]);
		return 1;
	}

	musicians_num = atoi(argv[optind]);

	soloers = new u_int32_t[musicians_num];
	registrations = (musician_registration_s*) calloc(musicians_num, sizeof(musician_registration_s));

	if (argc - optind + 1 > 2)
		measures_count = atoi(argv[optind + 1]);

	net_handler = net_init(port, "127.0.0.1");

	player = recv_player(net_handler);

	printf("got midi player (%d)\n", player);

	/* Build musicians list */
	for (i = 0; i < musicians_num; i++) {
		new_musician = (struct subscription_s *) malloc(sizeof(struct subscription_s));
		printf("waiting for a musician\n");
		recv_subscription(net_handler, new_musician);

		printf("director: got a new musician\n\tcoupling: %d\n\tinstrument_class: %d\n\tflags: %d\n\tconnection :%d\n",
				new_musician->coupling, new_musician->instrument_class,
				new_musician->flags, new_musician->connection);

		uint32_t newId = calculateID(new_musician, registrations, i);

		registrations[i].subscritpion = new_musician;
		registrations[i].id = newId;

		if(new_musician->flags & FLAG_MUSICIAN_GENETIC){
			list_add_tail(&new_musician->list, &genetics);
			genetic_process = true;
		} else {
			list_add_tail(&new_musician->list, &musicians);
		}
		if(new_musician->flags & FLAG_MUSICIAN_SOLOIST){
			soloers[soloers_num] = newId;
			soloers_num++;
		}

		send_id(new_musician->connection, newId);
		printf("\tregistered with id: %d\n", newId);
	}
	printf("registered %d musicians, of wich %d are soloers\n", musicians_num, soloers_num);

	free(registrations);

	send_num_of_musicians(player, musicians_num);

	if(genetic_process){
		stored_impro = new struct measure_s[measures_count];
	}

	fflush(stdout);

	if(!init_director_core("blues", "bebop", soloers_num, soloers, measures_count)){
		fprintf(stderr, "failed to initialize director core\n");
		cleanup();
		return 1;
	}

	/* main loop */
	printf("main loop\n");

	for (i = 0; i < measures_count; i++) {
		printf("decide next measure:\n");
		measures_per_section = decide_next_measure(&nm, current_measure_num);

		if(current_measure_num < measures_per_section)
			current_measure_num = (current_measure_num+1) % measures_per_section;
		else
			current_measure_num = 0;

		printf("broadcasting measure...\n");
		fflush(stdout);
		try {
			broadcast_measure(&nm, (genetic_process ? &genetics : &musicians));
			if(genetic_process){
				copyMeasure(&(stored_impro[i]), &nm);
			}
			sync_all(genetic_process ? &genetics : &musicians);
        } catch (end_of_improvisation_exception e) {
            break;
        }

		printf("musicians syncronized.\n");
		/* sleep to see if things block properly */
		/* sleep(1); UNBELIEVABLE!!! */

		clear_measure(&nm);
	}

    if(genetic_process){
        void *tmp_list;
        nm.bpm = MEASURE_BPM_EOI;
        broadcast_measure(&nm, &genetics);
        sync_all(&genetics);

        print_debug("musicians len before: %d\n", listLen(&musicians));

        /* concat musicians and genetics */
	concat_list_inplace(musicians, genetics);

        print_debug("musicians len after: %d\n", listLen(&musicians));

        secondLoop(stored_impro, measures_count);
    }

	free_director_core();
	list_for_each_entry_safe(new_musician, tmp_musician, &musicians, list){
		free(new_musician);
	}

	printf("we have come to an end\n");
	cleanup();
	return 0;
}

/*****************************************************************************/
/* Communication library for the improveesation components                   */
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
#include <linux/list.h>
#include <improveesation/structs.h>
#include <improveesation/communication.h>

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/uio.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>

/* ACKs, standard and sync */
#define SYNC_ACK 0x42
#define ACK 0x0

#define isSACK(payload) (payload == SYNC_ACK)
#define isACK(payload) (payload == ACK)

/* Communication error */
class network_exception net_ex;

/* Someone intentionally closed the communication. */
class end_of_improvisation_exception eoi_ex;

/* Load the iovec. */
#define LOAD_IOVEC(iov, i, field)\
	({iov[i].iov_base = &field;\
	 iov[i].iov_len = sizeof(field);})

/*** Endianess ***/

/* Launch htons on the firsts s fields of the iovec. */
#define IOVEC_HTONS(b, s) ({\
	struct iovec *a = (b);\
	int i;\
	for (i = 0; i < (s); i++)\
		*((uint16_t *)a[i].iov_base) = htons(*((uint16_t *)a[i].iov_base));\
})

/* Launch ntohs on the firsts s fields of the iovec. */
#define IOVEC_NTOHS(b, s) ({\
	struct iovec *a = (b);\
	int i;\
	for (i = 0; i < (s); i++)\
		*((uint16_t *)a[i].iov_base) = ntohs(*((uint16_t *)a[i].iov_base));\
})

/* Launch htonl on the firsts s fields of the iovec. */
#define IOVEC_HTONL(b, s) ({\
	struct iovec *a = (b);\
	int i;\
	for (i = 0; i < (s); i++)\
		*((uint32_t *)a[i].iov_base) = htonl(*((uint32_t *)a[i].iov_base));\
})

/* Launch ntohl on the firsts s fields of the iovec. */
#define IOVEC_NTOHL(b, s) ({\
	struct iovec *a = (b);\
	int i;\
	for (i = 0; i < (s); i++)\
		*((uint32_t *)a[i].iov_base) = ntohl(*((uint32_t *)a[i].iov_base));\
})

/*** Musician ***/
/* Send a subscription to the director */
uint32_t send_subscription(int director, uint32_t coupling,
        uint8_t instrument_class, uint8_t flags)
{
	uint32_t id;
	struct iovec comm_vec[3];

	LOAD_IOVEC(comm_vec, 0, coupling);
	LOAD_IOVEC(comm_vec, 1, instrument_class);
    LOAD_IOVEC(comm_vec, 2, flags);

	IOVEC_HTONL(comm_vec, 1);

	if (writev(director, comm_vec, 3) <= 0) {
		perror("send subscription write");
		throw net_ex;
		return -1;
	}

	IOVEC_NTOHL(comm_vec, 1);

	if (read(director, &id, sizeof(id)) <= 0) {
		perror("send subscription read");
		throw net_ex;
		return -1;
	}

	id = ntohl(id);

	return id;
}

/* Send a subscription struct to the director */
#ifdef __cplusplus
uint32_t send_subscription(int director, struct subscription_s *s)
#else /* C retrocompatibilty */
uint32_t send_subscription_struct(int director, struct subscription_s *s)
#endif
{
	return send_subscription(director, s->coupling, s->instrument_class,
            s->flags);
}

/* Receive a measure from the director */
void recv_measure(int director, struct measure_s *new_measure)
{
	int i, retval;
	struct subscription_s *cmusician;

	struct iovec safe_to_read[5], *tmp_iov;

	LOAD_IOVEC(safe_to_read, 0, new_measure->bpm);
	LOAD_IOVEC(safe_to_read, 1, new_measure->soloist_id);
	LOAD_IOVEC(safe_to_read, 2, new_measure->tempo.upper);
	LOAD_IOVEC(safe_to_read, 3, new_measure->tempo.lower);

	/* AAA: prioargs are allocated as an array IN the struct
	 * so sizeof() will return sizeof(int) * array_size */
	LOAD_IOVEC(safe_to_read, 4, new_measure->prioargs);

	retval = readv(director, safe_to_read, 5);

	if(retval < 0) {
		perror("readv");
		throw net_ex;
		return;
	} else if(retval == 0) {
		/* connection ended. */
		throw eoi_ex;
		return;
	}

	/* Endianess realigning */
	IOVEC_NTOHL(safe_to_read + 1, 1);

	for (i = 0; i <  sizeof(new_measure->prioargs) / sizeof(*new_measure->prioargs);
	     i++) {
		new_measure->prioargs[i] = ntohl(new_measure->prioargs[i]);
	}

	new_measure->tonal_zones = (struct tonal_zone_s *) realloc(
			new_measure->tonal_zones,
			sizeof(new_measure->tonal_zones) *
			new_measure->tempo.upper);

	tmp_iov = new struct iovec[new_measure->tempo.upper * 2]();

	if (!new_measure || !tmp_iov) {
		perror("malloc");
		throw net_ex;
		return;
	}

	for (i = 0; i < new_measure->tempo.upper; i++) {
		LOAD_IOVEC(tmp_iov, i * 2,
				new_measure->tonal_zones[i].note);
		LOAD_IOVEC(tmp_iov, 1 + i * 2,
				new_measure->tonal_zones[i].scale);
	}

	retval = readv(director, tmp_iov, new_measure->tempo.upper * 2);

	if(retval < 0) {
		perror("recv measure readv (1)");
		throw net_ex;
		return;
	} else if(retval == 0 && new_measure->tempo.upper > 0) {
		/* connection ended. */
		throw eoi_ex;
		return;
	}

	IOVEC_NTOHS(tmp_iov, new_measure->tempo.upper * 2);

	new_measure->chords = (struct chord_s *) realloc(new_measure->chords,
			new_measure->tempo.upper *
			sizeof(struct chord_s));

	for (i = 0; i < new_measure->tempo.upper; i++) {
		LOAD_IOVEC(tmp_iov, i * 2, new_measure->chords[i].note);
		LOAD_IOVEC(tmp_iov, 1 + i * 2, new_measure->chords[i].mode);
	}
	retval = readv(director, tmp_iov, new_measure->tempo.upper * 2);

	if(retval < 0) {
		perror("recv measure readv (2)");
		throw net_ex;
		return;
	} else if(retval == 0 && new_measure->tempo.upper > 0) {
		/* connection ended. */
		throw eoi_ex;
		return;
	}

	IOVEC_NTOHS(tmp_iov, new_measure->tempo.upper * 2);

	retval = read(director, &new_measure->tags.size,
			sizeof(new_measure->tags.size));
	if(retval < 0) {
		perror("recv measure readv (3)");
		throw net_ex;
		return;
	} else if(retval == 0) {
		/* connection ended. */
		throw eoi_ex;
		return;
	}

	new_measure->tags.size = ntohl(new_measure->tags.size);

	new_measure->tags.payload = (char *) realloc(new_measure->tags.payload,
			new_measure->tags.size *
			sizeof(char));
	if (!new_measure->tags.payload) {
		perror("realloc");
		throw net_ex;
		return;
	}

	retval = read(director, new_measure->tags.payload,
			sizeof(char) * new_measure->tags.size);
	if(retval < 0) {
		perror("readv");
		throw net_ex;
		return;
	} else if(retval == 0 && new_measure->tags.size > 0) {
		/* connection ended. */
		throw eoi_ex;
		return;
	}
	delete []tmp_iov;
}

/* Send syncronization ack to the director
 * (End of composing the current measure!)
 */
void send_sync_ack(int director)
{
	uint8_t sack = SYNC_ACK;
	if (write(director, &sack, sizeof(sack)) <= 0) {
		perror("send sync ack write");
		throw net_ex;
		return;
	}
}

/* Send the composed measure to the player */
void send_to_play(int player, struct play_measure_s *measure)
{
	int j;
	struct iovec iov[measure->size * 6 + 5];
	uint32_t measure_size_bck = measure->size;

	/* player send */
	LOAD_IOVEC(iov, 0, measure->id);
	LOAD_IOVEC(iov, 1, measure->size);
	LOAD_IOVEC(iov, 2, measure->musician_id);
	LOAD_IOVEC(iov, 3, measure->unchanged_fst);
	LOAD_IOVEC(iov, 4, measure->bpm);

	for (j = 0; j < measure->size; j++) {
		LOAD_IOVEC(iov, j * 6 + 5, measure->measure[j].tempo);
		LOAD_IOVEC(iov, j * 6 + 6, measure->measure[j].id);
		LOAD_IOVEC(iov, j * 6 + 7, measure->measure[j].triplets);
		LOAD_IOVEC(iov, j * 6 + 8, measure->measure[j].velocity);
		LOAD_IOVEC(iov, j * 6 + 9, measure->measure[j].chord_size);
		LOAD_IOVEC(iov, j * 6 + 10, measure->measure[j].notes);
	}

	IOVEC_HTONL(iov, 3);

	if (writev(player, iov, measure_size_bck * 6 + 5) < 0) {
		perror("send to play writev");
		throw net_ex;
		return;
	}


	IOVEC_NTOHL(iov, 3);
}

/* Send the composed measure to the player (auto ack version) */
void send_to_play(int player, int director, struct play_measure_s *measure)
{
	send_to_play(player, measure);

	/* director ack */
	send_sync_ack(director);
}

/* Midi player */

/* Receive the number of musicians from the director */
uint32_t recv_num_of_musicians(int net_handler)
{
	uint32_t retval = 0;
	if (read(net_handler, &retval, sizeof(retval)) < 0) {
		perror("recv num of musicians read");
		throw net_ex;
		return -1;
	}

	return ntohl(retval);
}

/* Receive a list of notes to play */
void recv_to_play(struct play_measure_s *note_list, struct list_head *musicians)
{
	struct subscription_s *cmusician;
	struct epoll_event *epevs;
	int	size = 0,
		psize = 0,
		pm_count = 0,
		retval;

	/* Create an epoll to wait every musician (network ordering) */
	int efd = epoll_create1(0);

	/* Add every musician to the epoll watcher */
	list_for_each_entry(cmusician, musicians, list) {
		struct epoll_event epev;

		epev.events = EPOLLIN;
		epev.data.fd = cmusician->connection;

		if (epoll_ctl(efd, EPOLL_CTL_ADD, cmusician->connection,
					&epev)){
			perror("epoll_ctl EPOLL_CTL_ADD");
			throw net_ex;
			return;
		}

		size++;
	}

	epevs = new struct epoll_event[size];
	if (!epevs) {
		perror("calloc");
		throw net_ex;
		return;
	}

	while (psize < size) {
		int i;
		
		/* Wait some musicians */
		int cprocessed = epoll_wait(efd, epevs, size, -1);

		if (epoll_wait < 0) {
			perror("epoll_wait");
			throw net_ex;
			return;
		}

		/* Load the structs from the net. */
		for(i = 0; i < cprocessed; i++) {
			int j;
			struct iovec safe_iov[5], *iov = NULL;
			if (!epevs[i].events & EPOLLIN) {
				throw net_ex;
				return;
			}

			/* player send */
			LOAD_IOVEC(safe_iov, 0,
				   note_list[pm_count].id);
			LOAD_IOVEC(safe_iov, 1,
				   note_list[pm_count].size);
			LOAD_IOVEC(safe_iov, 2,
				   note_list[pm_count].musician_id);
			LOAD_IOVEC(safe_iov, 3,
				   note_list[pm_count].unchanged_fst);
			LOAD_IOVEC(safe_iov, 4,
				   note_list[pm_count].bpm);

			retval = readv(epevs[i].data.fd, safe_iov, 5);
			if (retval < 0) {
				perror("recv to play readv (1)");
				throw net_ex;
				return;
			} else if (retval == 0) {
				throw eoi_ex;
				return;
			}

			IOVEC_NTOHL(safe_iov, 3);

			note_list[pm_count].measure = (struct notes_s*) realloc(
					note_list[pm_count].measure,
					sizeof(struct notes_s) *
					note_list[pm_count].size);

			if (!note_list[pm_count].measure) {
				perror("realloc");
				throw net_ex;
				return;
			}

			/* Dynamic fields */
			iov = new struct iovec[note_list[pm_count].size * 6];
			for (j = 0; j < note_list[pm_count].size; j++) {
				LOAD_IOVEC(iov, j * 6,
				   note_list[pm_count].measure[j].tempo);
				LOAD_IOVEC(iov, j * 6 + 1,
				   note_list[pm_count].measure[j].id);
				LOAD_IOVEC(iov, j * 6 + 2,
				   note_list[pm_count].measure[j].triplets);
				LOAD_IOVEC(iov, j * 6 + 3,
				   note_list[pm_count].measure[j].velocity);
				LOAD_IOVEC(iov, j * 6 + 4,
				   note_list[pm_count].measure[j].chord_size);
				LOAD_IOVEC(iov, j * 6 + 5,
				   note_list[pm_count].measure[j].notes);
			}

			retval = readv(epevs[i].data.fd, iov,
					note_list[pm_count].size * 6);

			delete[] iov;

			if (retval < 0) {
				perror("recv to play readv (2)");
				throw net_ex;
				return;
			} else if (retval == 0 &&
				   note_list[pm_count].size != 0) {
				throw eoi_ex;
				return;
			}

			/* Eventually delete the musician
			 * from the waiting queue */
			retval = epoll_ctl(efd, EPOLL_CTL_DEL,
					   epevs[i].data.fd, NULL);

			if (retval < 0) {
				perror("epoll_ctl");
				throw net_ex;
				return;
			}

			pm_count++;
		}

		if (cprocessed > 0) {
			psize += cprocessed;
		} else {
			/* No musician processed */
			break;
		}
	}

	delete [] epevs;
	close(efd);
}

/* Memory management */
void free_play_measures(struct play_measure_s *nl, int size)
{
	int i;
	if (nl) {
		for (i = 0; i < size; i++){
			if (nl[i].measure)
				free(nl[i].measure);
		}
		free (nl);

		nl = NULL;
	}
}

/*** Director ***/

/* Receive subscription from musician */
void recv_subscription(int conn_socket, struct subscription_s *new_musician)
{
	struct iovec iov[3];
	int retval;
	int i;

	int mus_connection = accept(conn_socket, NULL, NULL);

	if (mus_connection < 0) {
		perror("accept");
		throw net_ex;
		return;
	}

	LOAD_IOVEC(iov, 0, new_musician->coupling);
	LOAD_IOVEC(iov, 1, new_musician->instrument_class);
    LOAD_IOVEC(iov, 2, new_musician->flags);
	new_musician->connection = mus_connection;

	retval = readv(mus_connection, iov, 3);
	if(retval < 0) {
		perror("recv subscription readv");
		throw net_ex;
		return;
	} else if (retval == 0) {
		throw eoi_ex;
		return;
	}

	IOVEC_NTOHL(iov, 1);
}

/* Receive player subscription */
uint32_t recv_player(int conn_socket)
{
	int retval = -1;
	struct subscription_s *tmp_pl = new struct subscription_s;

	if (!tmp_pl) {
		perror("malloc");
		throw net_ex;
		return -1;
	}

	recv_subscription(conn_socket, tmp_pl);

	if (tmp_pl->coupling == PLAYER_ID) {
		retval = tmp_pl->connection;
		send_ack(retval);
	} else {
		retval = -1;
	}
	delete tmp_pl;

	return retval;
}

/* Send the num of musicians to the player */
void send_num_of_musicians(int player_conn, uint32_t musicians_count)
{
	uint32_t tosend = htonl(musicians_count);
	if (write(player_conn, &tosend, sizeof(tosend)) <= 0) {
		perror("send num of musician write");
		throw net_ex;
	}
}

/* Send an id to the connection endpoint (usually a musician) */
void send_id(int musician_conn, uint32_t id)
{
	uint32_t tosend = htonl(id);
	if (write(musician_conn, &tosend, sizeof(tosend)) <= 0) {
		perror("write");
		throw net_ex;
	}
}

/* send an ack to the connection endpoint (usually a musician) */
void send_ack(int musician_conn)
{
	send_id(musician_conn, ACK);
}

/* Broadcast the measure to all the musicians. */
void broadcast_measure(struct measure_s *next_measure, struct list_head *dests)
{
	struct subscription_s *cmusician;
	struct iovec safe_iov[5], *tmp_iov;
	int retval;

	list_for_each_entry(cmusician, dests, list) {
		int i;

		/* Fixed fields */
		LOAD_IOVEC(safe_iov, 0, next_measure->bpm);
		LOAD_IOVEC(safe_iov, 1, next_measure->soloist_id);
		LOAD_IOVEC(safe_iov, 2, next_measure->tempo.upper);
		LOAD_IOVEC(safe_iov, 3, next_measure->tempo.lower);

		/* AAA: prioargs are allocated as an array IN the struct
		 * so sizeof() will return sizeof(int) * array_size */
		LOAD_IOVEC(safe_iov, 4, next_measure->prioargs);

		IOVEC_HTONL(safe_iov + 1, 1);

		for (i = 0;
		     i < sizeof(next_measure->prioargs) / sizeof(*next_measure->prioargs);
		     i++) {
			next_measure->prioargs[i] = htonl(next_measure->prioargs[i]);
		}

		retval = writev(cmusician->connection, safe_iov, 5);
		if (retval <= 0) {
			perror("broadcast measure writev (1)");
			throw net_ex;
			return;
		}

		IOVEC_NTOHL(safe_iov + 1, 1);

		for (i = 0;
		     i < sizeof(next_measure->prioargs) / sizeof(*next_measure->prioargs);
		     i++) {
			next_measure->prioargs[i] = ntohl(next_measure->prioargs[i]);
		}

		/* Dynamic fields */
		tmp_iov = new struct iovec [next_measure->tempo.upper * 2 + 2];

		for (i = 0; i < next_measure->tempo.upper; i++) {
			LOAD_IOVEC(tmp_iov, i * 2,
				   next_measure->tonal_zones[i].note);
			LOAD_IOVEC(tmp_iov, i * 2 + 1,
				   next_measure->tonal_zones[i].scale);
		}

		IOVEC_HTONS(tmp_iov, next_measure->tempo.upper * 2);

		retval = writev(cmusician->connection, tmp_iov,
				next_measure->tempo.upper * 2);
		if (retval < 0) {
			perror("broadcast measure writev (2)");
			throw net_ex;
			return;
		}

		IOVEC_NTOHS(tmp_iov, next_measure->tempo.upper * 2);

		/* Dynamic fields */
		for (i = 0; i < next_measure->tempo.upper; i++) {
			LOAD_IOVEC(tmp_iov, i * 2,
				   next_measure->chords[i].note);
			LOAD_IOVEC(tmp_iov, i * 2 + 1,
				   next_measure->chords[i].mode);
		}

		IOVEC_HTONS(tmp_iov, next_measure->tempo.upper * 2);

		retval = writev(cmusician->connection, tmp_iov,
				next_measure->tempo.upper * 2);
		if (retval < 0) {
			perror("broadcast measure writev (3)");
			throw net_ex;
			return;
		}

		IOVEC_NTOHS(tmp_iov, next_measure->tempo.upper * 2);

		/* Tags */

		/* Fixed size */
		next_measure->tags.size = htonl(next_measure->tags.size);

		retval = write(cmusician->connection,
				&next_measure->tags.size,
				sizeof(next_measure->tags.size));
		if (retval <= 0) {
			perror("broadcast measure write (1)");
			throw net_ex;
			return;
		}
		next_measure->tags.size = ntohl(next_measure->tags.size);

		/* Dynamic field, tags payload */
		retval = write(cmusician->connection,
				next_measure->tags.payload,
				sizeof(char) * next_measure->tags.size);
		if (retval < 0) {
			perror("write");
			throw net_ex;
			return;
		}

		delete [] tmp_iov;
	}
}

/* Sync barrier, every musician should reach this point
 * before composing the next measure */
/* WARNING: completely blocking! */
void sync_all(struct list_head *musicians)
{
	struct subscription_s *cmusician;

	int size = 0,
	    psize = 0,
	    retval;

	uint8_t ack = 0;

	struct epoll_event *epevs;

	/* Epoll pool to get every musician sync (network order) */
	int efd = epoll_create1(0);

	if (efd < 0) {
		perror("epoll_create");
		throw net_ex;
		return;
	}

	list_for_each_entry(cmusician, musicians, list) {
		struct epoll_event epev;

		epev.events = EPOLLIN;
		epev.data.fd = cmusician->connection;

		if (epoll_ctl(efd, EPOLL_CTL_ADD,
			      cmusician->connection, &epev)){
			perror("epoll_ctl");
			throw net_ex;
			return;
		}

		size++;
	}

	epevs = new struct epoll_event[size];

	if (!epevs) {
		perror("calloc");
		throw net_ex;
		return;
	}

	while (psize < size) {
		int i;
		/* Wait for every musician */
		int cprocessed = epoll_wait(efd, epevs, size, -1);

		if (cprocessed < 0) {
			perror("epoll_wait");
			throw net_ex;
			return;
		} else if (cprocessed == 0 ){
			throw eoi_ex;
			return;
		}

		/* Process received acks. */
		for(i = 0; i < cprocessed; i++) {
			int j;
			if (!epevs[i].events & EPOLLIN) {
				throw net_ex;
				return;
			}
			retval = read(epevs[i].data.fd, &ack, sizeof(ack));

			if (retval < 0) {
				perror("epoll_wait");
				throw net_ex;
				return;
			} else if (retval == 0 ){
				throw eoi_ex;
				return;
			}

			if (epoll_ctl(efd, EPOLL_CTL_DEL,
						epevs[i].data.fd, NULL) < 0) {
				perror("epoll_wait");
				throw net_ex;
				return;
			}
		}

		psize += cprocessed;
	}

	delete [] epevs;
	close(efd);
}

/* Memory management */
void clear_measure(struct measure_s *m)
{
	if(m->tonal_zones != NULL)
		free(m->tonal_zones);
	if(m->chords != NULL)
		free(m->chords);
	if(m->tags.payload != NULL)
		free(m->tags.payload);
	memset(m, 0, sizeof(struct measure_s));
}

/*****************************************************************************/
/* MIDI player program.		                                                 */
/* This is the core program which executes the whole melody                  */
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
#include <fcntl.h>
#include <improveesation/communication.h>
#include <improveesation/structs.h>

#include <improveesation/player_core.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/list.h>

#include <time.h>

void fill_test_musician(subscription_s *new_musician, int prog){
	switch (prog){
		case 0:
			new_musician->coupling = 0;
			new_musician->instrument_class = GRANDPIANO;
			new_musician->soloer = SOLO;
			new_musician->connection = 0;
			break;
		case 1:
			new_musician->coupling = 0;
			new_musician->instrument_class = BASS;
			new_musician->soloer = RYTHMIC;
			new_musician->connection = 1;
			break;
		case 2:	
			new_musician->coupling = 0;
			new_musician->instrument_class = DRUMS;
			new_musician->soloer = RYTHMIC;
			new_musician->connection = 10;
			break;
		default:
			printf("It's nothing!\n");
			break;
		}
	}
	
void fill_test_measure(struct play_measure_s *note_list, int prog, uint32_t musicians_num){

	int i, j;

	for (i=0; i<musicians_num; i++){
		struct play_measure_s current;
		current.id = i; // hardcode -- not relevant
		switch(i){
			default:
				current.size = 8;
				
				current.musician_id = i; //hardcode FIXME
				if (i == 2){ // che merda di espediente
					current.musician_id = 10;
				}
				
				current.measure = (struct notes_s *) malloc(sizeof (struct notes_s)*current.size);
				for (j=0; j<current.size; j++){
					struct notes_s cnote;
					switch(i){
						case 0:
							cnote.note = 60 + (j * 4); // hardcode
							break;
						case 1:
							cnote.note = 40 + (j * 4); // hardcode
							break;
						case 2:
							cnote.note = 40 + (j * 4); // snare
							break;
					}
					cnote.tempo = 2;
					cnote.id = j;
					cnote.triplets = FALSE;
					current.measure[j] = cnote;
				}
				break;
		}
		note_list[current.id] = current;
	}
	
}

int midi_init(struct list_head *musicians, uint32_t musicians_num, int * fd, uint8_t * instrument_db){
	
	int i;
	unsigned char data[3];
	struct subscription_s *cmusician;
	
	*fd = open("/dev/midi1", O_WRONLY);
	if (*fd < 0){
		perror("open");
		return 0;
	};
	
	/* Assign musicians to each channel and setup */
	list_for_each_entry(cmusician, musicians, list) {
		
		/* Drums are assigned by default to channel 10 and it shouldn't be selected */
		if(cmusician->instrument_class != DRUMS){
			data[0] = SELECT_INSTRUMENT(cmusician->connection);
			data[1] = cmusician->instrument_class;
			data[2] = 120; // this is useless, just for parallelism
			/* Send the instrument setup to midi (we need just 2 params) */
			write(*fd, data, 2);
		}
		
		printf("Instrment %d in channel %d selected!\n", cmusician->instrument_class, cmusician->connection);
	}
	
	return 1;
}


/** 
 * This is the main function which aims to play a whole measure given in input:
 * note_list is a pointer to an array of measures to play each identified by the instruent ID (obviously all different)
 * musicians is the pointer to the array of musicians involved
 * musicians_num contains the number of musicians envolved in the impro */
void play_measure(struct play_measure_s *note_list, struct list_head *musicians, uint32_t musicians_num, int fd, uint8_t * instrument_db){
	int i, j;
	int null_counter = 0;
	/* This is a matrix which hosts the notes ordered by instrument (will be -1 terminated) */
	struct notes_s notes[musicians_num][25];
	/* This is an array which stores the pointers for every execution, the countdown to the end and the channel */
	int note_pointer[musicians_num][3];
	/* Data to be written to midi */
	unsigned char data[musicians_num][3];
	/* NULL NOTE */
	struct notes_s nullnote = {-1,0,0,0};
	
	/* FIXME bpm are set to 120 by default. I can't obtain them. bpms is the duration of a semiquiver in mus */
	double bpm = 120;
	double bpms = (15 / bpm) * 1000000;
	printf("%f\n", bpms);
	
	for(i=0; i<musicians_num; i++){
		note_pointer[i][0] = 0;
		note_pointer[i][1] = 0;
		note_pointer[i][2] = 0;
		data[i][0] = KEY_UP(i);
		data[i][1] = 0;
		data[i][2] = 120;
	}
	
	for(i=0; i<musicians_num; i++){
		note_pointer[i][2] = note_list[i].musician_id;
		
		/* Fill the arrays of notes and make them -1 terminated */
		for(j=0; j<25; j++){
			if(j<note_list[i].size)
				notes[i][j] = note_list[i].measure[j];
			else
				notes[i][j] = nullnote;
			printf("%d ",notes[i][j].note);
		} 
		printf("\n");
	}
	
	/* For every note step (limit is max value + 1) */
	for(i=0; i<25; i++){
		printf("loop %d\n",i);
		/* For every instrument */
		for(j=0; j<musicians_num; j++){
			
			/* We are checking only playing channels (if all of them are -1 we're not playing) */
			if(note_pointer[j][0] >= 0){
				
				/* We are about to play a new note */
				if(note_pointer[j][1] == 0){
					
					write(fd, data[j], 3); // key up the previous
					// extract the new note
					data[j][0] = KEY_DOWN(note_pointer[j][2]);
					if (notes[j][note_pointer[j][0]].note != 255)
						data[j][1] = notes[j][note_pointer[j][0]].note;
					else { // or enter in the depths of hell
						note_pointer[j][0] = -1;
						null_counter++;
						continue;
					}
					
					write(fd, data[j], 3); // key down the new note
					printf("\tMusician id %d playing %d on channel %d\n", j, notes[j][note_pointer[j][0]].note, note_pointer[j][2]);
					data[j][0] = KEY_UP(note_pointer[j][2]);// set a key up for the next step
					
					note_pointer[j][1] = notes[j][note_pointer[j][0]].tempo - 1; // set the new countdown (-1 cause the first semiquiver is consumed)
					note_pointer[j][0]++; // increase the note pointer
					
				} else { /* subtract from countdown */
					note_pointer[j][1]--;
				}
							
			} 	
		}
		
		/* Is everybody over? if so get out! */
		if (null_counter != musicians_num)
			usleep(bpms);
		else break;
	}
}
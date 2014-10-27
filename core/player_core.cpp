/**************************************-***************************************/
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
#include <improveesation/utils.h>
#include <improveesation/player_core.h>
#include <improveesation/midi_writer.h>

#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <linux/list.h>

#include <time.h>

#define DEBUG

using namespace std;

/* Data to be written to midi (16 channels X 3 params per every note in chord) */
unsigned char data[16][MAX_CHORD_SIZE][3];
/* Channel reference */
int *Channels;
/* Used by the midifile for progression */
int atom_counter = 0;

int findMidiChannel(uint8_t instrumentClass){
    int i;
    for(i = 0; i < MIDI_CHANNELS; i++){
        if(Channels[i] == instrumentClass)
            return i;
    }
    return -1;
}

/* TEST This creates a test set of 3 musicians */
void fill_test_musician(subscription_s *new_musician, int prog){
	switch (prog){
		case 0:
			new_musician->coupling = 0;
			new_musician->instrument_class = GRANDPIANO;
            new_musician->flags = FLAG_MUSICIAN_SOLOIST;
			new_musician->connection = 0;
			break;
		case 1:
			new_musician->coupling = 0;
			new_musician->instrument_class = BASS;
            new_musician->flags = FLAG_MUSICIAN_NONE;
			new_musician->connection = 1;
			break;
		case 2:	
			new_musician->coupling = 0;
			new_musician->instrument_class = DRUMS;
            new_musician->flags = FLAG_MUSICIAN_NONE;
			new_musician->connection = 10;
			break;
		default:
			printf("It's nothing!\n");
			break;
		}
    }
	
/* TEST Stupid and frightening cycling scale */	
void fill_test_measure(struct play_measure_s *note_list, int prog, uint32_t musicians_num){

	int i, j;

	for (i=0; i<musicians_num; i++){
		struct play_measure_s current;
		current.id = i; // hardcode -- not relevant
		switch(i){
			default:
				current.size = 9;
				
				if (i == 0){ // che merda di espediente
					current.musician_id = 0;
				}
				if (i == 1){ // che merda di espediente
					current.musician_id = 33;
				}
				if (i == 2){ // che merda di espediente
					current.musician_id = 10;
				}
				
				current.measure = (struct notes_s *) malloc(sizeof (struct notes_s)*current.size);
				for (j=0; j<current.size; j++){
					struct notes_s cnote;
					switch(i){
						case 0:
							cnote.notes[0] = 60 + (j * 4); // hardcode
							break;
						case 1:
							cnote.notes[0] = 40 + (j * 4); // hardcode
							break;
						case 2:
							cnote.notes[0] = 40 + (j * 4); // snare
							break;
					}
					cnote.tempo = 2;
					cnote.id = j;
					cnote.chord_size = 1;
					if(j>=6) // let's go with the triplet ya! 
						cnote.triplets = TRUE;
					else 
						cnote.triplets = FALSE;
					current.measure[j] = cnote;
				}
				break;
		}
		note_list[current.id] = current;
	}
	
}

int midi_init(struct list_head *musicians, uint32_t musicians_num, int * fd, char * dev, char *fname)
{
	
	int i,j;
	
	*fd = open(dev, O_WRONLY);
	//*fd = open("/dev/snd/midiC2D0", O_WRONLY); // FIXME
	if (*fd < 0){
		perror("open");
		return 0;
	}
	
	/* Init every dataset to a pointless keyup */
	for (i=0; i<16; i++){ 
		for(j=0; j<MAX_CHORD_SIZE; j++){
			data[i][j][0] = KEY_UP(i);
			data[i][j][1] = 0;
			data[i][j][2] = 120;
		}
	}

    Channels = (int *)calloc(MIDI_CHANNELS, sizeof(int));

    /* Parameters for the MIDI file */
    initFile(fname, &Channels, 120, fd, musicians, musicians_num); // FIXME hardcode

	return 1;
}


/** 
 * This is the main function which aims to play a whole measure given in input:
 * note_list is a pointer to an array of measures to play each identified by the instruent ID (obviously all different)
 * musicians is the pointer to the array of musicians involved
 * musicians_num contains the number of musicians envolved in the impro */
void play_measure(struct play_measure_s *note_list, struct list_head *musicians, uint32_t musicians_num, int fd){
	int i, j, k;
	int null_counter = 0;
	/* This is a matrix which hosts the notes ordered by instrument (will be -1 terminated) */
	struct notes_s notes[musicians_num][25];
	/* This is an array which stores the pointers for every execution, the countdown to the end, the channel and the triplet flag */
	int note_pointer[musicians_num][4];
	/* NULL NOTE */
	struct notes_s nullnote = {0,0,0,0,1,{-1,0,0,0,0,0,0,0}};
	//~ struct notes_s nullnote = {
		//~ .tempo = 0,
		//~ .id = 0,
		//~ .triplets = 0,
		//~ .velocity = 0,
		//~ .chord_size = 1,
		//~ .notes = {-1,0,0,0,0,0,0,0}
	//~ };
	
	/* FIXME bpm are set to 120 by default. I can't obtain them. bpms is the duration of half a semiquiver triplet unit in mus */
	double bpm = 120;
	double bpms = BPM_TO_ATOM(bpm); // the factor *3 is for the triplet
	
	/* Init note pointer for the execution */
	for(i=0; i<musicians_num; i++){
		note_pointer[i][0] = 0;
		note_pointer[i][1] = 0;
		note_pointer[i][2] = 0;
		note_pointer[i][3] = FALSE;
	}

	for(i=0; i<musicians_num; i++){
        note_pointer[i][2] = findMidiChannel(note_list[i].musician_id & 0xff);

        if(note_pointer[i][2] < 0)
            fprintf(stderr, "shit happened, unable to find musician %d midi channel\n", note_list[i].musician_id);
		
        print_debug("[debug] Instrument %d set up in channel %d!\n", note_list[i].musician_id & 0xff, findMidiChannel(note_list[i].musician_id & 0xff));
		print_debug("[debug] Notes for %d: {", i);
		
		/* Fill the arrays of notes and make them -1 terminated */
		for(j=0; j<25; j++){
			if(j<note_list[i].size){
				notes[i][j] = note_list[i].measure[j];
				notes[i][j].tempo = (notes[i][j].tempo * (2 + (1 - notes[i][j].triplets)));// commute the duration in a third of a semiquiver with a trick for the triplet
			} else
				notes[i][j] = nullnote;
			
			#ifdef DEBUG
			print_debug(" [", i);
			for(k=0;k<notes[i][j].chord_size;k++){
				print_debug(" %d",notes[i][j].notes[k]);
			}
			print_debug(" ]");
			#endif
			
		} 
		print_debug(" }\n");
	}
	
	/* For every note step (limit is max value + 1 like there would be all smallest triplets) */
	for(i=0; i<49; i++){
		print_debug("[debug] Loop %d\n",i);
		/* For every instrument */
		for(j=0; j<musicians_num; j++){
			
			/* We are checking only playing channels (if all of them are -1 we're not playing) */
			if(note_pointer[j][0] != -1){
				
				/* We are about to play a new note */
				if(note_pointer[j][1] == 0){
					
					if (notes[j][note_pointer[j][0]].notes[0] != 255) { // if the note is not -1
						for(k=0; k<MAX_CHORD_SIZE; k++){
                            //write(fd, data[j][k], 3); // key up the previous set
                            writeNote(atom_counter, data[j][k]);
						}
						/* Notes may remain but they are set to keyup, so no bad things should happen */
					
						// extract the new note set and stack them onto data array
						for(k=0; k<notes[j][note_pointer[j][0]].chord_size; k++){
							data[j][k][0] = KEY_DOWN(note_pointer[j][2]);
							data[j][k][1] = notes[j][note_pointer[j][0]].notes[k];
							data[j][k][2] = notes[j][note_pointer[j][0]].velocity;
						} 
						
					} else { // or enter in the depths of hell (execution terminated for the instrument)
						note_pointer[j][0] = -1;
						null_counter++;
						continue;
					}
					
					if(notes[j][note_pointer[j][0]].notes[0] < 128){ //actual note (skip if silence)
						for(k=0; k<notes[j][note_pointer[j][0]].chord_size; k++){
                            //write(fd, data[j][k], 3); // key down the new note set
                            writeNote(atom_counter, data[j][k]);
						}
						
						print_debug("\tMusician id %d playing [", j);
						
						for(k=0; k<notes[j][note_pointer[j][0]].chord_size; k++){ // set a key up for the next step
							data[j][k][0] = KEY_UP(note_pointer[j][2]);
							print_debug(" %d ", notes[j][note_pointer[j][0]].notes[k]);
						}
						print_debug("] on channel %d\n", note_pointer[j][2]);
					} else {
						print_debug("\tMusician id %d playing silence on channel %d\n", j, note_pointer[j][2]);
						data[j][0][0] = KEY_UP(note_pointer[j][2]);// set a key up for the next step (not cycled because we used only the first)
					}
					
					note_pointer[j][1] = notes[j][note_pointer[j][0]].tempo - 1; // set the new countdown (-1 cause the first semiquaver is consumed)
					note_pointer[j][0]++; // increase the note pointer
					
				} else { /* subtract from countdown */
					note_pointer[j][1]--;
				}
							
			} 	
		}
		
		/* Is everybody over? if so get out! */
		if (null_counter != musicians_num){
			usleep(bpms);
			atom_counter++;
		} else break; 
	}
}

/* Sudden order to shut up! */
void smorza_incosa(int fd){
	int i,j;
	for(i=0; i<16; i++){
        for(j=0;j<MAX_CHORD_SIZE;j++){
            //write(fd, data[i][j], 3); // key up everything
            writeNote(atom_counter, data[i][j]);
		}
	}
	
    free(Channels);

	/* Close the MIDI file */
	closeFile();
	print_debug("[debug] Smorzat!\n");
	printf("End Of Improvisation exception reached!\n");
}

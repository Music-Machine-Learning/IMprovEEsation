/*****************************************************************************/
/* Midi files outputting library                                             */
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

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <improveesation/midi_writer.h>
#include <improveesation/utils.h>
#include <improveesation/const.h>
#include <improveesation/structs.h>

#include <linux/list.h>

#define BUFFER_INIT_SIZE            1024
#define ATOM_TO_PPQN                12
#define ATOM_LEN                    80
#define VARIABLE_VALUES_MAX_SIZE    6
#define MIDI_HEADER_SIZE            14

#define SPECIAL_TRACK_SIZE          (7+8+6+9+4)

#define MIDI_WRITE_FILE(m)  (m&MIDI_WRITE_FILE_ONLY)
#define MIDI_WRITE_DEV(m)   (m&MIDI_WRITE_DEV_ONLY)

#define writetrack(t,s,n)   ({fwrite(s,sizeof(char),n,tracks[t]); trackPointer[t] += n;})

int *dev;
char *midifilename;
FILE *midifile;

/* maps midi channel to tracks buffer number */
int instrumentsNumber;
FILE **tracks;
char **tracksTmpNames;
unsigned int *trackPointer;
uint32_t *lastAtom;

uint8_t timeref;

int variabilize_val(unsigned int v, char str[]){
    int s, i;

    s = byte_size(v);
    if(s <= 1 && !(v & 1<<7)){
        str[0] = v & 0xFF;
        return 1;
    }
    s += 1 + (s/8);

    for(i = 0; i < s; i++){
        str[(s-i-1)] = ((v >> (i*7)) & 0x7F) | (i==0 ? 0 : 0x80);
    }
    return s;
}

void writeSpecialTrack(){
    char data[9];
    unsigned int t;

    fwrite("MTrk", sizeof(char), 4, midifile);

    data[0] = (char) (SPECIAL_TRACK_SIZE >> 24) & 0xFF;
    data[1] = (char) (SPECIAL_TRACK_SIZE >> 16) & 0xFF;
    data[2] = (char) (SPECIAL_TRACK_SIZE >> 8) & 0xFF;
    data[3] = (char) SPECIAL_TRACK_SIZE & 0xFF;
    fwrite(data, sizeof(char), 4, midifile);

    /* set tempo */
    t = 60000000 / timeref;
    data[0] = (char) 0;
    data[1] = (char) 0xFF;
    data[2] = (char) 0x51;
    data[3] = (char) 3;
    data[4] = (char) (t>>16)&0xFF;
    data[5] = (char) (t>>8)&0xFF;
    data[6] = (char) t&0xFF;
    fwrite(data, sizeof(char), 7, midifile);

    /* time signature FIXME:hardcoded */
    data[0] = (char) 0;
    data[1] = (char) 0xFF;
    data[2] = (char) 0x58;
    data[3] = (char) 4;
    data[4] = (char) 4;
    data[5] = (char) 4;
    data[6] = (char) 24;
    data[7] = (char) 8;
    fwrite(data, sizeof(char), 8, midifile);

    /* sequence number */
    data[0] = (char) 0;
    data[1] = (char) 0xFF;
    data[2] = (char) 0;
    data[3] = (char) 2;
    data[4] = (char) 0;
    data[5] = (char) 1;
    fwrite(data, sizeof(char), 6, midifile);

    /* SMPTE offset */
    data[0] = (char) 0;
    data[1] = (char) 0xFF;
    data[2] = (char) 0x54;
    data[3] = (char) 5;
    data[4] = (char) 0;
    data[5] = (char) 0;
    data[6] = (char) 0;
    data[7] = (char) 0;
    data[8] = (char) 0;
    fwrite(data, sizeof(char), 9, midifile);


    /* end of track message */
    data[0] = (char) 0;
    data[1] = (char) 0xFF;
    data[2] = (char) 0x2F;
    data[3] = (char) 0;
    fwrite(data, sizeof(char), 4, midifile);

}

int initFile(char *fname, int **instruments, uint8_t bpm, int *midiDev, struct list_head *musicians, uint32_t musiciansCount){
    unsigned int i, t;
    struct subscription_s *cmusician;
    int chcounter = 0;
    unsigned char mdata[3];

    dev = midiDev;
    instrumentsNumber = musiciansCount;
    timeref = bpm;

    if(fname != NULL){
        midifilename = (char*)malloc(strlen(fname)*sizeof(char));
        strcpy(midifilename, fname);

        midifile = fopen(midifilename, "wb");
        if(midifile == NULL){
            midifilename = NULL;
        } else {
            fclose(midifile);
        }
    } else {
    //disable file writing
        midifilename = NULL;
    }

    tracks = (FILE **)calloc(MIDI_CHANNELS, sizeof(FILE *));
    tracksTmpNames = (char **)calloc(MIDI_CHANNELS, sizeof(char*));
    trackPointer = (unsigned int *)calloc(MIDI_CHANNELS, sizeof(unsigned int));
    lastAtom = (uint32_t *)calloc(MIDI_CHANNELS, sizeof(uint32_t));

    i = 0;

    /* Assign musicians to each channel and setup */
    list_for_each_entry(cmusician, musicians, list) {

        /* Drums are assigned by default to channel 16 and it shouldn't be selected */
        if(cmusician->instrument_class != INSTRUMENT_DRUM){
	    if (chcounter == MIDI_DRUM_CHANNEL - 1) {
		    /* A non drum can't have this channel.
		     * It should have been jumped in the previous assignment. */
		    fprintf(stderr, "Error in midi channel assignment\n");
		    return FALSE;
	    } else if (chcounter == MIDI_DRUM_CHANNEL - 2) {
		    chcounter += 2;
	    } else { 
		    chcounter++;
	    }
	    t = chcounter; /* XXX: what if it will be bigger than MIDI_CHANNELS? */

        } else {
	    t = MIDI_DRUM_CHANNEL - 1;
        }
        (*instruments)[t] = cmusician->instrument_class;

        mdata[0] = SELECT_INSTRUMENT(t);
        mdata[1] = cmusician->instrument_class;
        mdata[2] = 120; // this is useless, just for parallelism

        if(midifilename != NULL){
            tracksTmpNames[t] = (char *)calloc(strlen(fname)+1, sizeof(char));
            strcpy(tracksTmpNames[t], fname);
            tracksTmpNames[t][strlen(fname)-1] = t & 0xFF;
            printf("tmp file: %s\n", tracksTmpNames[t]);

            tracks[t] = fopen(tracksTmpNames[t], "w+b");

            writetrack(t, "MTrk", 4);

            fseek(tracks[t], 4, SEEK_CUR);

            trackPointer[t] = 0;
            lastAtom[t] = 0;

            writeNote(0, mdata, MIDI_WRITE_FILE_ONLY, 2); /* not properly a write NOTE */
        }

        /* Send the instrument setup to midi (we need just 2 params) */
        write(*dev, mdata, 2);


        printf("Instrument %d binded to channel %d!\n", cmusician->instrument_class, t);
    }

    return(midifilename != NULL ? TRUE : FALSE);
}

int writeNote(unsigned int atom, unsigned char* event, uint8_t mode, uint8_t size){
    unsigned int chnum, v_size, i;
    char v_time[VARIABLE_VALUES_MAX_SIZE];
    /* write to midi dev */
    if(MIDI_WRITE_DEV(mode))
        write(*dev, event, size);

    /* write to midi buffer */
    if(midifilename != NULL && MIDI_WRITE_FILE(mode)){
        chnum = event[0] & 0xF;

        if(tracks[chnum] != NULL){
            i = (atom - lastAtom[chnum]) * ATOM_LEN;

            v_size = variabilize_val(i, v_time);
            if(v_size == 0)
                fprintf(stderr, "shit happened variabilizing %d..\n", i);

            writetrack(chnum, v_time, v_size);

            writetrack(chnum, event, size);

            lastAtom[chnum] = atom;
        }
    }
}

int closeFile(){
    int i, c;
    unsigned int s;

    if(midifilename != NULL){
        midifile = fopen(midifilename, "w");
        if(midifile == NULL){
            fprintf(stderr, "midi file %s is not valid\n", midifilename);
        } else {
            char header[MIDI_HEADER_SIZE];
            memset(header, 0, MIDI_HEADER_SIZE*sizeof(char));

            memcpy(header, "MThd", 4);
            /* header size */
            header[7] = 6;
            /* midi format */
            header[9] = 1;
            /* number of tracks (plus the special track) */
            header[11] = instrumentsNumber+1;
            /* time division */
            header[12] = ((ATOM_TO_PPQN * ATOM_LEN) >> 8) & 0xFF;
            header[13] = (ATOM_TO_PPQN * ATOM_LEN) & 0xFF;

            fwrite(header, sizeof(char), MIDI_HEADER_SIZE, midifile);

            writeSpecialTrack();

            for(i = 0; i < MIDI_CHANNELS; i++){
                if(tracks[i] != NULL){
                    char dataLen[4];

                    /*end of track message*/
                    dataLen[0] = (char) 0;
                    dataLen[1] = (char) 0xFF;
                    dataLen[2] = (char) 0x2F;
                    dataLen[3] = (char) 0;
                    writetrack(i,dataLen,4);

                    fseek(tracks[i], 4, SEEK_SET);

                    s = trackPointer[i];
                    dataLen[0] = (char) (s>>24)&0xFF;
                    dataLen[1] = (char) (s>>16)&0xFF;
                    dataLen[2] = (char) (s>>8)&0xFF;
                    dataLen[3] = (char) s&0xFF;
                    writetrack(i,dataLen,4);

                    fseek(tracks[i], 0, SEEK_SET);
                    while((c = getc(tracks[i])) != EOF){
                        putc(c, midifile);
                    }

                    if(fclose(tracks[i])){
                        fprintf(stderr, "error while closing midi tmp file for channel %d\n", i);
                    }

                    if(remove(tracksTmpNames[i])){
                        fprintf(stderr, "error while removing midi tmp file %s\n", tracksTmpNames[i]);
                    }
                }
            }

            if(fclose(midifile)){
                fprintf(stderr, "error while closing midi file %s\n", midifilename);
            }
        }
    }
    for(i = 0; i < MIDI_CHANNELS; i++){
        if(tracksTmpNames[i] != NULL){
            free(tracksTmpNames[i]);
        }
    }
    free(tracks);
    free(tracksTmpNames);
    free(trackPointer);
    free(lastAtom);
}

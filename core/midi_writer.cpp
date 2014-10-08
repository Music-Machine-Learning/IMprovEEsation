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
#define VARIABLE_VALUES_MAX_SIZE    6
#define MIDI_HEADER_SIZE            14

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

int initFile(char *fname, int **instruments, uint8_t bpm, int *midiDev, struct list_head *musicians, uint32_t musiciansCount){
    char *tempoEvent;
    unsigned int i, t;
    struct subscription_s *cmusician;
    int chcounter = 0, s;
    unsigned char mdata[3];

    dev = midiDev;
    instrumentsNumber = musiciansCount;

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
        if(cmusician->instrument_class != DRUMS){
            t = chcounter++;
        } else {
            t = MIDI_CHANNELS - 1;
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

            // move buffer pointer to the beginning of the midi event sequence
            trackPointer[t] = 8;
            lastAtom[t] = 0;

            writeNote(0, mdata, MIDI_WRITE_FILE_ONLY, 2); /* not properly a write NOTE */
        }

        /* Send the instrument setup to midi (we need just 2 params) */
        write(*dev, mdata, 2);


        printf("Instrument %d binded to channel %d!\n", cmusician->instrument_class, chcounter);
    }

    if(midifilename != NULL){
        if(tracks[0]){
            t = 60000000 / bpm;
            s = byte_size(t);
            tempoEvent = (char *) calloc(3+s, sizeof(char));
            tempoEvent[0] = 0xFF;
            tempoEvent[1] = 0x51;
            tempoEvent[2] = s;
            for(i=0, s-=1; s >= 0; i++, s--)
                tempoEvent[i] = (t >> (s * 8)) & 0xFF;
            writetrack(0, tempoEvent, MIDI_EVENT_SIZE);
            free(tempoEvent);
        }
        return TRUE;
    }
    return FALSE;
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
            i = atom - lastAtom[chnum];

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
            /* number of tracks */
            header[11] = instrumentsNumber;
            /* time division */
            header[13] = ATOM_TO_PPQN;

            fwrite(header, sizeof(char), MIDI_HEADER_SIZE, midifile);

            for(i = 0; i < MIDI_CHANNELS; i++){
                if(tracks[i] != NULL){
                    char dataLen[4];
                    fseek(tracks[i], 4, SEEK_SET);
                    dataLen[0] = (((uint32_t)(trackPointer[i]-8))<<24)&0xFF;
                    dataLen[1] = (((uint32_t)(trackPointer[i]-8))<<16)&0xFF;
                    dataLen[2] = (((uint32_t)(trackPointer[i]-8))<<8)&0xFF;
                    dataLen[3] = ((uint32_t)(trackPointer[i]-8))&0xFF;
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

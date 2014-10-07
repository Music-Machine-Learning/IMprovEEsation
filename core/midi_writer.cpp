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
#include <improveesation/player_core.h>

#include <linux/list.h>

#define MIDI_CHANNELS       16
#define BUFFER_INIT_SIZE    1024
#define MIDI_EVENT_SIZE     3
#define ATOM_TO_PPQN        12

int *dev;
char *midifilename;
FILE *midifile;

using namespace std;

/* maps midi channel to tracks buffer number */
int instrumentsNumber;
unsigned char **tracks;
unsigned int *trackPointer;
uint32_t *lastAtom;
char header[14];

int variabilize_val(unsigned int v, char **str){
    int s, i;

    s = byte_size(v);
    if(s <= 1 && !(v & 1<<7)){
        *str = (char *)malloc(sizeof(char));
        **str = v & 0xFF;
        return 1;
    }
    s += 1 + (s/8);
    *str = (char *)calloc(s, sizeof(char));

    for(i = 0; i < s; i++){
        (*str)[(s-i-1)] = ((v >> (i*7)) & 0x7F) | (i==0 ? 0 : 0x80);
    }
    return s;
}

void printMap(map<int, int> *amap){
    map<int, int>::iterator it;
    int i;

    printf("map size: %d\n", amap->size());

    for(i = 0, it = amap->begin(); i < amap->size(); it++, i++){

        printf("el: <%d> -> %d\n", it._M_node->_M_left, it._M_node->_M_right);
    }
}

int initFile(char *fname, map<int, int> *instruments, uint8_t bpm, int *midiDev, struct list_head *musicians, uint32_t musiciansCount){
    char *tempoEvent;
    unsigned int i, t;
    struct subscription_s *cmusician;
    int chcounter = 0, s;
    unsigned char mdata[3];

    dev = midiDev;
    instrumentsNumber = musiciansCount;

    midifilename = (char*)malloc(strlen(fname)*sizeof(char));
    strcpy(midifilename, fname);

    midifile = fopen(midifilename, "wb");
    if(midifile == NULL){
        midifilename = NULL;
        return TRUE;
    } else {
        fclose(midifile);
    }

    tracks = (unsigned char **)calloc(MIDI_CHANNELS, sizeof(unsigned char *));
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
        (*instruments)[cmusician->instrument_class] = t;

        mdata[0] = SELECT_INSTRUMENT(t);
        mdata[1] = cmusician->instrument_class;
        mdata[2] = 120; // this is useless, just for parallelism

        tracks[t] = (unsigned char *)calloc(BUFFER_INIT_SIZE, sizeof(unsigned char));
        memcpy(tracks[t], "MTrk", 4);

        // move buffer pointer to the beginning of the midi event sequence
        trackPointer[t] = 8;
        lastAtom[t] = 0;

        /* Send the instrument setup to midi (we need just 2 params) */
        write(*dev, mdata, 2);
        writeNote(0, mdata, true); /* not properly a write NOTE */

        printf("Instrument %d binded to channel %d!\n", cmusician->instrument_class, chcounter);
    }

    memcpy(header, "MThd", 4);
    /* header size */
    header[7] = 6;
    /* midi format */
    header[9] = 1;
    /* number of tracks */
    header[11] = instrumentsNumber;
    /* time division */
    header[13] = ATOM_TO_PPQN;
    header[4] = header[5] = header[6] = header[8] = header[10] = header[12] = 0;

    if(tracks[0]){
        t = 60000000 / bpm;
        s = byte_size(t);
        tempoEvent = (char *) calloc(3+s, sizeof(char));
        tempoEvent[0] = 0xFF;
        tempoEvent[1] = 0x51;
        tempoEvent[2] = s;
        for(i=0, s-=1; s >= 0; i++, s--)
            tempoEvent[i] = (t >> (s * 8)) & 0xFF;
        memcpy(tracks[0]+trackPointer[0], tempoEvent, MIDI_EVENT_SIZE);
        free(tempoEvent);
    }

    return FALSE;
}

int writeNote(unsigned int atom, unsigned char* event, bool fileOnly){
    unsigned int chnum, v_size, i;
    unsigned int *pointer;
    unsigned char *track;
    char *v_time;
    /* write to midi dev */
    if(!fileOnly)
        write(*dev, event, MIDI_EVENT_SIZE);

    /* write to midi buffer */
    if(midifilename != NULL){
        chnum = event[0] & 0xF;
        pointer = &(trackPointer[chnum]);
        track = tracks[chnum];

        i = atom - lastAtom[chnum];
        //FIXME add byte_size() to utils.cpp
        v_size = variabilize_val(i, &v_time);

        //check della morte da rifare
        if(((*pointer) % BUFFER_INIT_SIZE) + (MIDI_EVENT_SIZE + v_size) > BUFFER_INIT_SIZE){
            track = (unsigned char *) realloc (track, (BUFFER_INIT_SIZE - ((*pointer)%BUFFER_INIT_SIZE))+(*pointer)+BUFFER_INIT_SIZE);
        }

        memcpy(track+(*pointer), v_time, v_size);
        *pointer += v_size;

        memcpy(track+(*pointer), event, MIDI_EVENT_SIZE);
        *pointer += MIDI_EVENT_SIZE;

        lastAtom[chnum] = atom;

        free(v_time);
    }
}

int closeFile(){
    int i;



    for(i = 0; i < instrumentsNumber; i++){
        free(tracks[i]);
    }
    free(tracks);
    free(trackPointer);
    free(lastAtom);

}

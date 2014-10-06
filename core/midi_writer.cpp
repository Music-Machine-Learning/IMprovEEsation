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

#include <improveesation/midi_writer.h>
#include <improveesation/utils.h>
#include <improveesation/const.h>

#define BUFFER_INIT_SIZE    1024;
#define MIDI_EVENT_SIZE     3;
#define ATOM_TO_PPQN        12;

int *dev;
FILE *midifile;

using namespace std;

/* maps midi channel to tracks buffer number */
map <int, int> instMap;
unsigned char **tracks;
unsigned int *trackPointer;
uint32_t *lastAtom;
char header[14];

int variabilize_val(unsigned int v, char *str){
    int s, i;

    s = byte_size(v);
    if(s <= 1){
        str = (char *)malloc(sizeof(char));
        *str = v & 0xFF;
        return 1;
    }
    s += 1 + (s/8);
    str = (char *)calloc(s, sizeof(char));

    for(i = 0; i < s; i++){
        str[i] = ((v >> (i*7)) & 0x7F) | (i==0 ? 0 : 0x80);
    }
    return s;
}

int initFile(char *fname, map<int, int> *instruments, uint8_t bpm, int *midiDev){
    map<int, int>::iterator it;
    char *tempoEvent;
    unsigned int i, s, t;

    midifile = fopen(fname, "w+b");
    if(midifile == NULL){
        return TRUE;
    }

    memcpy(header, "MThd", 4);
    /* header size */
    header[7] = 6;
    /* midi format */
    header[9] = 1;
    /* number of tracks */
    header[11] = instruments.size();
    /* time division */
    header[13] = ATOM_TO_PPQN;
    header[4] = header[5] = header[6] = header[8] = header[10] = header[12] = 0;

    tracks = (char **)calloc(instruments.size(), sizeof(char *));
    trackPointer = (unsigned int *)calloc(instruments.size(), sizeof(unsigned int));
    lastAtom = (uint32_t *)calloc(instruments.size(), sizeof(uint32_t));
    for(i = 0, it = instruments.begin(); i < instruments.size(); i++, it++){
        tracks[i] = (char *)calloc(BUFFER_INIT_SIZE, sizeof(char));
        memccpy(tracks[i], "MTrk", 4);
        /* move buffer pointer to the beginning of the midi event sequence */
        trackPointer[i] = 8;
        instMap[it->first] = i;
        lastAtom[i] = 0;
    }

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

int writeNote(unsigned int atom, unsigned char* event){
    unsigned int chnum, v_size, i;
    int *pointer;
    char *track, *v_time;
    /* write to midi dev */
    write(dev, event, MIDI_EVENT_SIZE);

    /* write to midi buffer */
    if(midifile != NULL){
        chnum = event[0] & 0xF;
        pointer = trackPointer + instMap[chnum];
        track = tracks + instMap[chnum];

        i = atom - lastAtom[instMap[chnum]];
        //FIXME add byte_size() to utils.cpp
        v_size = variabilize_val(i, v_time);

        //check della morte da rifare
        if(((*pointer) % BUFFER_INIT_SIZE) + (MIDI_EVENT_SIZE + v_size) > BUFFER_INIT_SIZE){
            track = (char *) realloc (track, (BUFFER_INIT_SIZE - (*pointer)%BUFFER_INIT_SIZE)+(*pointer));
        }

        memcpy(track+(*pointer), event, MIDI_EVENT_SIZE);

        *pointer += MIDI_EVENT_SIZE;
        lastAtom[instMap[chnum]] = atom;

        free(v_time);
    }
}

int closeFile(){
    int i;



    for(i = 0; i < instMap.size(); i++){
        free(tracks[i]);
    }
    free(tracks);
    free(trackPointer);
    free(lastAtom);

    if(midifile == NULL || fclose(midifile) != 0)
        return TRUE;
    return FALSE;
}

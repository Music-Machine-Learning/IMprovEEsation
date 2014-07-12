/*****************************************************************************/
/* Director's core facilities                                                */
/* This header is a part of the IMprovEEsation suite.                        */
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

#include <improveesation/director_core.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static char* genre; /* genre is fixed from the beginning... */

static int measures_per_section;
static char** dynamics;
static char** moods;
static int prev_mood;
static int moods_num;
static uint16_t* steps;
static uint16_t* modes;
static tempo_s tempo;
static tonal_zone_s tonality;
static uint8_t bpm;

static int impro_end;

void load_genre_info(){
	/*
	 * initialize:
	 * 	measures_per_section
	 *  dynamics,
	 *  steps,
	 *  modes,
	 *  moods,
	 * 	moods_num
	 * */
    //test stubs
    int i;
    measures_per_section = 12;
    dynamics = (char**) malloc(measures_per_section*sizeof(char*));
    for(i = 0; i < measures_per_section-1; i++){
        dynamics[i] = (char *) malloc(6*sizeof(char));
        strcpy(dynamics[i], "groove");
    }
    dynamics[measures_per_section-1] = (char*) malloc(4*sizeof(char));
    strcpy(dynamics[measures_per_section-1], "fill");

    steps = (uint16_t*) malloc(measures_per_section*sizeof(uint16_t));
    steps[0] = steps[2] = steps[3] = steps[6] = steps[7] = steps[10] = 0;
    steps[1] = steps[4] = steps[5] = steps[9] = 5;
    steps[8] = steps[11] = 7;

    modes = (uint16_t*) malloc(measures_per_section*sizeof(uint16_t));
    for(i = 0; i < measures_per_section; i++)
        modes[i] = CHORD_MAJOR|CHORD_SEVENTH;

    moods_num = 2;
    moods = (char**) malloc(moods_num*sizeof(char*));
    moods[0] = (char*) malloc(6*sizeof(char));
    strcpy(moods[0], "pushed");
    moods[1] = (char*) malloc(4*sizeof(char));
    strcpy(moods[1], "slow");

    prev_mood = 0;
}

void init_score_defs(){
	/*
	 * initialize:
     *  tonality
     *  tempo
     * 	bpm
	 * */
    //test stubs
    tonality.note = 0;
    tonality.scale = 0b010011101001; //blues scale
    tempo.upper = 4;
    tempo.lower = 4;
    bpm = 60;
}

chord_s *getTritone(int current_measure_id){
    int i;
    chord_s *chords = (chord_s*) calloc(tempo.upper, sizeof(chord_s));

    for(i = 0; i < tempo.upper; i++) {
        chords[i].note = (steps[current_measure_id]+6)%12;
        chords[i].mode = CHORD_MAJOR | CHORD_SEVENTH;
    }
    return chords;
}

chord_s *getCadenza(int current_measure_id){
    int i;
    chord_s *chords = (chord_s*) calloc(tempo.upper,sizeof(chord_s));
	/*
	 * [II m7, V 7]
	 * relatives to original chord
	 * */
    for(i = 0; i < (tempo.upper >> 1); i++){
        chords[i].note = (steps[current_measure_id]+2)%12;
        chords[i].mode = CHORD_MINOR | CHORD_SEVENTH;
    }
    for(; i < tempo.upper; i++){
        chords[i].note = (steps[current_measure_id]+7)%12;
        chords[i].mode = CHORD_MAJOR | CHORD_SEVENTH;
    }
    return chords;
}

chord_s *getTonalZoneChord(int current_measure_id){
    int i;
    chord_s *chords = (chord_s*) calloc(tempo.upper,sizeof(chord_s));
	if(rand() % 1){
        for(i = 0; i < tempo.upper; i++){
            chords[i].note = (steps[current_measure_id]+9)%12;
            chords[i].mode = CHORD_MINOR | CHORD_SEVENTH;
        }
	} else {
        for(i = 0; i < tempo.upper; i++){
            chords[i].note = (steps[current_measure_id]+4)%12;
            chords[i].mode = CHORD_MINOR; //should be diminished
        }
	}
    return chords;
}

chord_s *getRandomChord(){
    int i;
    uint16_t note, mode;
    chord_s *chords;
    note = rand() % 12;
	switch(rand() % 6){
		case 0:
            mode = CHORD_MAJOR; break;
		case 1:
            mode = CHORD_MAJOR | CHORD_SEVENTH; break;
		case 2:
            mode = CHORD_MAJOR | CHORD_DELTA; break;
		case 3:
            mode = CHORD_MINOR; break;
		case 4:
            mode = CHORD_MINOR | CHORD_SEVENTH; break;
		case 5:
            mode = CHORD_MINOR | CHORD_DELTA; break;
	}
    chords = (chord_s*) calloc(tempo.upper, sizeof(chord_s));
    for(i = 0; i < tempo.upper; i++){
        chords[i].note = note;
        chords[i].mode = mode;
    }
    return chords;
}

void setupTags(measure_s *measure, int current_measure_id){
    char *mood, *dyn;
    int genLen, moodLen, dynLen, i;
    if(rand() % 100 < (current_measure_id == 0 ? MOOD_CHANGE_ON_ONE_THRESHOLD : MOOD_CHANGE_THRESHOLD))
        prev_mood = rand() % moods_num;
    mood = moods[prev_mood];
    dyn = dynamics[current_measure_id];

    genLen = strlen(genre);
    moodLen = strlen(mood);
    dynLen = strlen(dyn);

    measure->tags.size = genLen + 1 + moodLen + 1 + dynLen + 1;
    measure->tags.payload = (char *) calloc(measure->tags.size, sizeof(char));
    //FIXME: violence!
    for(i = 0; i < genLen; i++){
        measure->tags.payload[i] = genre[i];
    }
    measure->tags.payload[i] = ';';
    for(i ++; i < dynLen+genLen+1; i++){
        measure->tags.payload[i] = dyn[i-genLen-1];
    }
    measure->tags.payload[i] = ';';
    for(i ++; i < moodLen+dynLen+genLen+2; i++){
        measure->tags.payload[i] = mood[i-genLen-dynLen-2];
    }
    measure->tags.payload[i] = '\0';
    //strcpy(measure->tags.payload, "blues;ciao;test");
}

void init_director_core(char* gen, char *sub){
    genre = gen;
    impro_end = 0;

    srand(time(NULL));

    load_genre_info();

    init_score_defs();
}

int decide_next_measure(measure_s *measure, int current_measure_id){
    int trand, i;
	
	if(rand() % 100 < (current_measure_id == 0 ? GENRE_CHANGE_ON_ONE_THRESHOLD : GENRE_CHANGE_THRESHOLD)){
		if(rand() % 100 < CHANGE_TO_SUBGENRE){
			/*
			 * pick a valid subgenre
			 */
			 // genre = subgen;
		} else {
			/*
			 * pick a random genre
			 */
			 // genre = gen; 
		}
		//if(new_genre != genre)
			//load_genre_info();
	}
	
    //FIXME: more than one chord per measure
	if(rand() % 100 < (current_measure_id == 0 ? CHORD_CHANGE_ON_ONE_THRESHOLD : CHORD_CHANGE_THRESHOLD))
	{	// go to an unexpected chord 
		trand = rand() % 100;
		if(trand < CHORD_CHANGE_TRITONE){
            measure->chords = getTritone(current_measure_id);
		} else if (trand < CHORD_CHANGE_CADENZA){
            measure->chords = getCadenza(current_measure_id);
		} else if (trand < CHORD_CHANGE_TONAL_ZONE){
            measure->chords = getTonalZoneChord(current_measure_id);
		} else {
            measure->chords = getRandomChord();
		}
	} else {
        measure->chords = (chord_s*) calloc(tempo.upper, sizeof(chord_s));
        for(i = 0; i < tempo.upper; i++){
            measure->chords[i].note = steps[current_measure_id];
            measure->chords[i].mode = modes[current_measure_id];
        }
	}
	
    measure->bpm = bpm;

    //if we want to have variable tempo do it here
    measure->tempo.upper = tempo.upper;
    measure->tempo.lower = tempo.lower;

    //if we want to have variable tonal zones do it here
    measure->tonal_zones = (tonal_zone_s*) calloc(tempo.upper, sizeof(tonal_zone_s));
    for(i = 0; i < tempo.upper; i++){
        measure->tonal_zones[i].note = tonality.note;
        measure->tonal_zones[i].scale = tonality.scale;
    }

    setupTags(measure, current_measure_id);
	
    if(0 && current_measure_id == (measures_per_section-1))	// if it is last measure and it's already time to stop, put an end to the improvisation
        impro_end = 1;

    return measures_per_section;
}

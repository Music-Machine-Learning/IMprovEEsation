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
#include <improveesation/db.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

struct variant_couple {
    char *subgenre;
    pattern_s *pattern;
    struct variant_couple *next;
};

static char* genre;

/*static int measures_per_section;
static char** dynamics;
static char** moods;
static uint16_t* steps;
static uint16_t* modes;
*/

static tempo_s tempo;
static uint16_t tonality;
static uint8_t bpm;
static uint32_t *soloers;
static uint32_t soloers_num;

static int prev_mood;
static int moods_num;

static PGconn *database;

static pattern_s *current_pattern;
static variant_couple *improvariants;

static int impro_end;

void load_genre_info(char* gen, char* sub){
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
    int i, j;
    char *variant;
    variant_couple *tmp;
    /*measures_per_section = 12;
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
    strcpy(moods[1], "slow");*/

    get_pattern(database, gen, sub, &current_pattern);

    prev_mood = 0;
    moods_num = 0;
    while(current_pattern->moods[moods_num] != NULL)
        moods_num++;

    improvariants = NULL;

    if(current_pattern->variants_size > 0){
        tmp = improvariants;

        for(i = 0; i < current_pattern->variants_size; i++){
            j = 0;
            while((variant = current_pattern->variants[i].variants[j++])[0] != '\0'){
                improvariants = (variant_couple*) malloc(sizeof(variant_couple));
                strcpy(tmp->subgenre, variant);
                get_pattern(database, gen, variant, &(tmp->pattern));
                improvariants->next = tmp;
                tmp = improvariants;
            }
        }
    }
}

void init_score_defs(){
	/*
	 * initialize:
     *  tonality
     *  tempo
     * 	bpm
	 * */
    //test stubs
    tonality = 0;
    //tonality.scale = 0b010011101001; //blues scale
    tempo.upper = 4;
    tempo.lower = 4;
    bpm = 60;
}

//NOTE: if new modes are to be added, update this function!
uint16_t modeToChord(char* mode){
    int i = 0;
    uint16_t ret = 0;
    if(mode[i] == 'm'){
        ret |= CHORD_MINOR;
        i++;
    } else
        ret |= CHORD_MAJOR;
    if(mode[i] == '7'){
        if(mode[i+1] == '+')
            ret |= CHORD_DELTA;
        else
            ret |= CHORD_SEVENTH;
    }
    return ret;
}

//returns the reference chord if cadenza detected, -1 if not
int checkCadenza(measure_pattern_s *measure){
    if(measure->stepnumber != 2)
        return -1;
    if(!strcmp(measure->modes[0], "m7"))
        return -1;
    if(measure->steps[1] != ((measure->steps[0]+5) % 12))
        return -1;
    if(!strcmp(measure->modes[1], "7"))
        return -1;
    return measure->steps[1];
}

chord_s *getTritone(int step){
    int i;
    chord_s *chords = (chord_s*) calloc(tempo.upper, sizeof(chord_s));
    printf("tritone:\n");

    for(i = 0; i < tempo.upper; i++) {
        chords[i].note = tonality + (step+6)%12;
        chords[i].mode = CHORD_MAJOR | CHORD_SEVENTH;
    }
    return chords;
}

chord_s *getCadenza(int step){
    int i;
    chord_s *chords = (chord_s*) calloc(tempo.upper,sizeof(chord_s));
    printf("cadenza:\n");

	/*
	 * [II m7, V 7]
	 * relatives to original chord
	 * */
    for(i = 0; i < (tempo.upper >> 1); i++){
        chords[i].note = tonality + (step+2)%12;
        chords[i].mode = CHORD_MINOR | CHORD_SEVENTH;
    }
    for(; i < tempo.upper; i++){
        chords[i].note = tonality + (step+7)%12;
        chords[i].mode = CHORD_MAJOR | CHORD_SEVENTH;
    }
    return chords;
}

chord_s *getTonalZoneChord(int step){
    int i;
    chord_s *chords = (chord_s*) calloc(tempo.upper,sizeof(chord_s));
    printf("tonal zone changes:\n");

	if(rand() % 1){
        for(i = 0; i < tempo.upper; i++){
            chords[i].note = tonality + (step+9)%12;
            chords[i].mode = CHORD_MINOR | CHORD_SEVENTH;
        }
	} else {
        for(i = 0; i < tempo.upper; i++){
            chords[i].note = tonality + (step+4)%12;
            chords[i].mode = CHORD_MINOR; //NOTE: should be diminished
        }
	}
    return chords;
}

chord_s *getRandomChord(){
    int i;
    uint16_t note, mode;
    chord_s *chords;
    i = 0;
    chords = (chord_s*) calloc(tempo.upper, sizeof(chord_s));
    printf("random pick:\n");

RANDOM_CHORD_IS_RANDOM:
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

    for(; i < tempo.upper; i++){
        chords[i].note = note;
        chords[i].mode = mode;
        if(rand() % 100 < RANDOM_MULTI_CHORD_THRESHOLD){
            i++;
            goto RANDOM_CHORD_IS_RANDOM;
        }
    }
    return chords;
}

void decideChord(measure_s *measure, int current_measure_id){
    int trand, range, i, step = -1;

    //we can change chord when the actual measure chord is clear (eg: single chord / cadenza)
    if(current_pattern->measures[current_measure_id].stepnumber == 1 || (step = checkCadenza(&(current_pattern->measures[current_measure_id]))) >= 0){
        if(rand() % 100 < (current_measure_id == 0 ? CHORD_CHANGE_ON_ONE_THRESHOLD : CHORD_CHANGE_THRESHOLD))
        {	// go to an unexpected chord
            printf("\tgo to unexpected chord, use ");
            if(step < 0)
                step = current_pattern->measures[current_measure_id].steps[0];
            trand = rand() % 100;
            if(trand < CHORD_CHANGE_TRITONE){
                measure->chords = getTritone(step);
            } else if (trand < CHORD_CHANGE_CADENZA){
                measure->chords = getCadenza(step);
            } else if (trand < CHORD_CHANGE_TONAL_ZONE){
                measure->chords = getTonalZoneChord(step);
            } else {
                measure->chords = getRandomChord();
            }
            return;
        }
    }
    printf("\tmeasure chords: (standard)\n");
    measure->chords = (chord_s*) calloc(tempo.upper, sizeof(chord_s));
    range = tempo.upper / current_pattern->measures[current_measure_id].stepnumber +
            (tempo.upper % current_pattern->measures[current_measure_id].stepnumber > 0 ? 1 : 0);
    for(i = 0; i < tempo.upper; i++){
        measure->chords[i].note = current_pattern->measures[current_measure_id].steps[i/range];
        measure->chords[i].mode = modeToChord(current_pattern->measures[current_measure_id].modes[i/range]);
    }
}

void setupTags(measure_s *measure, int current_measure_id){
    char *mood, *dyn;
    int genLen, moodLen, dynLen, i;
    if(rand() % 100 < (current_measure_id == 0 ? MOOD_CHANGE_ON_ONE_THRESHOLD : MOOD_CHANGE_THRESHOLD)){
        printf("\tchange mood\n");
        prev_mood = rand() % moods_num;
    }
    mood = current_pattern->moods[prev_mood];
    dyn = current_pattern->measures[current_measure_id].dynamics;

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
}

void decideImproScale(measure_s *measure, int current_measure_id){
    uint16_t note, scale;
    int i, j, range, step, tmp, tzones_total = 0;
    measure_pattern_s *m_patt;
    uint *count = (uint*) calloc (12, sizeof(uint));
    uint *tzones = (uint*) calloc (12, sizeof(uint));

    //TODO: consider impro variants!

    for(range = current_pattern->measures_count; range > 0; range--){
        memset(count, 0, 12 * sizeof(uint));
        for(i = 0; i < range; i++){         //count each note occurrence in range
            tmp = current_measure_id - (range >> 1) + i;
            m_patt = &(current_pattern->measures[(tmp >= 0 ? tmp : current_pattern->measures_count + tmp)]);
            if((step = checkCadenza(m_patt)) >= 0)
                count[step] ++;
            else{
                for(j = 0; j < m_patt->stepnumber; j++){
                    count[m_patt->steps[j]] ++;
                }
            }
        }
        for(i = 0, tmp = 0; i < 12; i++){   //get the maximum for this range
            if(count[i] > tmp)
                tmp = count[i];
        }
        for(i = 0; i < 12; i++){            //update plausible tonal zones array
            if(count[i] == tmp){
                tzones[i]++;
                tzones_total++;
            }
        }
    }

    tmp = rand() % tzones_total;

    for(i = 0; i < 12; i++){
        if(tzones[i] > 0){
            if(tmp < tzones[i]){
                note = i;
                break;
            }
            else
                tmp -= tzones[i];
        }
    }

    //TODO: get all possible scales and select a matching one
    //(if this measure has 2 chords and it is a cadenza, the second one is the refernce chord)
    scale = 0b010011101001;

    measure->tonal_zones = (tonal_zone_s*) calloc(tempo.upper, sizeof(tonal_zone_s));
    for(i = 0; i < tempo.upper; i++){
        measure->tonal_zones[i].note = note;
        measure->tonal_zones[i].scale = scale;
        printf("\t\tN: %d\tS: %d\n", note, scale);
    }
}

void init_director_core(char* gen, char *sub, uint32_t solocount, uint32_t *sololist){
    genre = gen;
    impro_end = 0;

    soloers = sololist;
    soloers_num = solocount;

    srand(time(NULL));

    database = db_connect("griffin.dberardi.eu",
                          "improveesation_experimental_testes",
                          "read_only",
                          "testiamo123");

    load_genre_info(genre, sub);

    init_score_defs();
}

void free_director_core(){
    int i;
    variant_couple *tmp;

    tmp = improvariants;
    while(improvariants != NULL){
        free_pattern(improvariants->pattern);
        tmp = improvariants->next;
        free(improvariants);
        improvariants = tmp;
    }

    free_pattern(current_pattern);

    db_close(database);
}

int decide_next_measure(measure_s *measure, int current_measure_id){
    int i;
	
	if(rand() % 100 < (current_measure_id == 0 ? GENRE_CHANGE_ON_ONE_THRESHOLD : GENRE_CHANGE_THRESHOLD)){
        printf("\tchange");
		if(rand() % 100 < CHANGE_TO_SUBGENRE){
			/*
			 * pick a valid subgenre
			 */
			 // genre = subgen;
            printf("subgenre\n");
		} else {
			/*
			 * pick a random genre
			 */
			 // genre = gen; 
            printf("genre\n");
		}
		//if(new_genre != genre)
			//load_genre_info();
	}

    decideChord(measure, current_measure_id);

    for(i = 0; i < tempo.upper; i++){   //NOTE: useless debug
        printf("\t\tN: %d\tM: %d\n", measure->chords[i].note, measure->chords[i].mode);
    }
	
    measure->bpm = bpm;
    printf("\ttime: %dbpm\n", bpm);

    //NOTE: if we want to have variable tempo do it here
    measure->tempo.upper = tempo.upper;
    measure->tempo.lower = tempo.lower;
    printf("\ttempo: %d/%d\n", tempo.upper, tempo.lower);

    printf("\ttonal zones:\n");
    decideImproScale(measure, current_measure_id);

    setupTags(measure, current_measure_id);
    printf("\ttags: %s\n", measure->tags.payload);
	
    //FIXME: this should follow some policy
    measure->soloist_id = soloers[rand() % soloers_num];
    printf("\tsoloer: %d\n", measure->soloist_id);

    //if it is last measure and it's already time to stop, put an end to the improvisation
    if(0 && current_measure_id == (current_pattern->measures_count-1))
        impro_end = 1;

    return current_pattern->measures_count;
}

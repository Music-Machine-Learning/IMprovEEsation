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
#include <errno.h>
#include <string.h>
#include <time.h>
#include <improveesation/configuration.h>
#include <stdio.h>

struct variant_couple_s {
    char *subgenre;
    pattern_s *pattern;
    int first, last;
    struct variant_couple_s *next;
};

struct scale_list_s {
    int size;
    uint16_t *list;
};

struct soloer_s {
    uint32_t id;
    uint32_t measures_to_go;
};

static char* genre;
static char* subgenre;
static char** genresList;
static char** subgenresList;

static tempo_s tempo;
static uint16_t tonality;
static uint8_t bpm;
static soloer_s *soloers;
static int soloer;
static uint32_t soloers_num;

static int prev_mood;
static int moods_num;

static PGconn *database;

static pattern_s *current_pattern;
static variant_couple_s *improvariants;

static scale_list_s available_scales;

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
    variant_couple_s *tmp, *last;

    get_pattern(database, gen, sub, &current_pattern);

    if(current_pattern==NULL){
        printf("ERROR: missing genre+subgenre: %s, %s!!!\n", gen, sub);
        return;
    }

    prev_mood = 0;
    moods_num = 0;
    while(current_pattern->moods[moods_num] != NULL)
        moods_num++;

    if(available_scales.size > 0){
        free_db_results(available_scales.list);
    }

    available_scales.size = get_scales(database, gen, &(available_scales.list));

    improvariants = NULL;

    if(current_pattern->variants_size > 0){
        last = NULL;

        for(i = 0; i < current_pattern->variants_size; i++){
            j = 0;
            while((variant = current_pattern->variants[i].variants[j++]) != NULL){
                tmp = (variant_couple_s*) malloc(sizeof(variant_couple_s));
                tmp->subgenre = (char *) malloc(sizeof(char)*strlen(variant));
                strcpy(tmp->subgenre, variant);
                get_pattern(database, gen, variant, &(tmp->pattern));
                tmp->first = current_pattern->variants[i].first;
                tmp->last = current_pattern->variants[i].last;
                tmp->next = last;
                last = tmp;
            }
        }
        improvariants = tmp;
    }
}

void init_score_defs(uint8_t key=-1, uint8_t tUpper=4, uint8_t tLower=4, uint8_t beats=80){
    /*
     * initialize:
     *  tonality
     *  tempo
     * 	bpm
     * */
    tonality = (key==-1 ? rand() % 12 : key);
    tempo.upper = tUpper;
    tempo.lower = tLower;
    bpm = beats;
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

// performs a rotation of the lowest 12 bits of mask by rot places (positive numbers rotate left, negatives rotate right)
uint16_t rotateMask(uint16_t mask, int rotation){
    int rot = rotation % 12;
    if(rot > 0){    //left rotation
        return (mask << rot | mask >> (12-rot)) & 0b111111111111;
    } else if(rot < 0){ //right rotation
        return (mask >> rot | mask << (12-rot)) & 0b111111111111;
    } else  //rot == 0, no rotations at all
        return mask;
}

//returns a list of matching scales referred to chord and based on reference note
int getMatchingScales(uint16_t **list, uint16_t chord_step, uint16_t chord_mode, uint16_t reference_note){
    int i, s;
    uint16_t scale, chord, ref;
    *list = (uint16_t *) calloc(available_scales.size, sizeof(uint16_t));
    for(i = 0, s = 0; i < available_scales.size; i++){
        if(available_scales.list[i] == BLUES_SCALE){
            //if it's a standard blues chord we have special rules (be ready for a deadly conditional branch)
            if(chord_step == tonality || chord_step == tonality + 5 || chord_step == tonality + 7){
                if((chord_step == tonality + 5 && reference_note == tonality + 5) ||
                   (chord_step == tonality + 7 && (reference_note == tonality + 7 || reference_note == tonality + 4)) ||
                   (reference_note == tonality || reference_note == tonality + 2 || reference_note == tonality + 9)){
                    (*list)[s] = BLUES_SCALE;
                    s++;
                    continue;
                }
            }
        }
        scale = rotateMask(available_scales.list[i], reference_note);
        chord = rotateMask(chord_mode, chord_step);
        ref = scale & chord;
        if(ref == chord){
            (*list)[s] = available_scales.list[i];
            s++;
        }
    }
    if(!s){
        for(i = 0, ref = 0; i < available_scales.size; i++){
            scale = rotateMask(available_scales.list[i], reference_note);
            chord = rotateMask(chord_mode, chord_step);
            if(ref < chord & scale){
                ref = chord & scale;
                (*list)[0] = scale;
            }
        }
        s = 1;
    }
    return s;
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
        measure->chords[i].note = (current_pattern->measures[current_measure_id].steps[i/range] + tonality) % 12;
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

    sprintf(measure->tags.payload, "%s;%s;%s", genre, dyn, mood);
}

void decideImproScale(measure_s *measure, int current_measure_id){
    uint16_t note, scale, *list;
    int i, j, range, step, tmp, tzones_total = 0;
    measure_pattern_s *m_patt;
    uint *count = (uint*) calloc (12, sizeof(uint));
    uint *tzones = (uint*) calloc (12, sizeof(uint));

    //TODO: consider impro variants!

    for(range = current_pattern->measures_count; range > 0; range--){
        memset(count, 0, 12 * sizeof(uint));
        for(i = 0; i < range; i++){         //count each note occurrence in range
            tmp = current_measure_id - (range >> 1) + i;
            m_patt = &(current_pattern->measures[(tmp >= 0 ? tmp % 12 : current_pattern->measures_count + tmp)]);
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
                note = (i + tonality) % 12;
                break;
            }
            else
                tmp -= tzones[i];
        }
    }

    measure->tonal_zones = (tonal_zone_s*) calloc(tempo.upper, sizeof(tonal_zone_s));

    step = -1;
    //if there is one valid reference chord for the entire measure decide based on that chord
    if(current_pattern->measures[current_measure_id].stepnumber == 1 || (step = checkCadenza(&(current_pattern->measures[current_measure_id]))) >= 0){
        if(step < 0){
            step = current_pattern->measures[current_measure_id].steps[0];
            scale = measure->chords[0].mode;
        } else
            scale = measure->chords[tempo.upper-1].mode;
        step = (step + tonality) % 12;

        i = getMatchingScales(&list, step, scale, note);

        //NOTE: this is a bit rough...
        scale = list[rand() % i];

        for(i = 0; i < tempo.upper; i++){
            measure->tonal_zones[i].note = note;
            measure->tonal_zones[i].scale = scale;
            printf("\t\tN: %d\tS: %d\ttempo: %d\n", note, scale, tempo.upper);
        }
    } else {    //otherways select one scale for each chord
        for(i = 0; i < tempo.upper; i++){
            measure->tonal_zones[i].note = note;
            j = getMatchingScales(&list, measure->chords[i].note, measure->chords[i].mode, note);
            //NOTE: this is a bit rough as well...
            measure->tonal_zones[i].scale = list[rand() % j];
            printf("\t\tN: %d\tS: %d\ttempo: %d\n", note, scale, tempo.upper);
        }
    }

}

void decidePriorities(measure_s *measure, int current_measure_id){
	int i;

	/* TODO: Now this is just for testing but this function should 
	   fill the prioargs array with some policy */
	i = 0;
	measure->prioargs[i++] = QUARTER_ARG_DYNA;
	measure->prioargs[i++] = QUARTER_ARG_MOOD;
	measure->prioargs[i++] = QUARTER_ARG_INSTR;
	measure->prioargs[i++] = QUARTER_ARG_CNOTE;
	measure->prioargs[i++] = QUARTER_ARG_CMODE;
	measure->prioargs[i++] = QUARTER_ARG_POS;
	measure->prioargs[i++] = QUARTER_ARG_GENRE;
	measure->prioargs[i++] = QUARTER_ARG_SCALE;
	measure->prioargs[i++] = QUARTER_ARG_SOLO;

	if (i != QUARTER_QUERY_ARGS)
		printf("too many or not enough arguments checked\n");
}

void pickSubgenre(){
    int i;
    char** pick;

    if(subgenresList){
        for(i = 0; subgenresList[i] != NULL; i++);

        pick = &(subgenresList[rand()%i]);
        while(*pick=="" || !strcmp(*pick, subgenre)){
            pick = &(subgenresList[rand()%i]);
        }

        free(subgenre);
        subgenre = (char*)calloc(strlen(*pick)+1, sizeof(char));
        strcpy(subgenre, *pick);
    }
}

void pickGenre(){
    int i;
    char** pick;

    for(i = 0; genresList[i] != NULL; i++);

    pick = &(genresList[rand()%i]);
    while(*pick=="" || !strcmp(*pick, genre)){
        pick = &(genresList[rand()%i]);
    }

    free(genre);
    genre = (char*)calloc(strlen(*pick)+1, sizeof(char));
    strcpy(genre, *pick);

    free_db_results(subgenresList);
    get_subgenres(database, genre, &subgenresList);
}

void setupSoloers(uint32_t solocount, uint32_t *sololist, int measures_count){
    uint32_t leader, tmp;
    int i, solo_slot, not_solo_riffs;

    if(solocount){
        leader = sololist[rand() % solocount];
        printf("band leader: %d\n", leader);

        solo_slot = measures_count / (solocount+2); // leader instrument will improvise twice the time as the other instuments and one soloer slot is taken for non-improvised measures;

        if(current_pattern){
            if((current_pattern->measures_count * 3) > measures_count){ //only band leader will improvise
                soloers_num = 3;
                soloers = (soloer_s *) calloc(soloers_num, sizeof(soloer_s));
                soloers[0].id = leader;
                soloers[0].measures_to_go = solo_slot - (solo_slot % current_pattern->measures_count);
                soloers[1].id = SOLOER_NONE;
                soloers[1].measures_to_go = solo_slot - (solo_slot % current_pattern->measures_count);
                soloers[2].id = leader;
                soloers[2].measures_to_go = measures_count - (soloers[0].measures_to_go + soloers[1].measures_to_go);
            } else {
                not_solo_riffs = (measures_count / current_pattern->measures_count) / NOT_SOLO_MAX_SECTIONS;
                not_solo_riffs = (not_solo_riffs ? rand() % not_solo_riffs : 0);
                soloers_num = (measures_count - ((not_solo_riffs + 2) * current_pattern->measures_count)) / current_pattern->measures_count + not_solo_riffs + 2;
                soloers = (soloer_s *) calloc(soloers_num, sizeof(soloer_s));
                soloers[0].id = leader;
                soloers[0].measures_to_go = current_pattern->measures_count;
                for(i = 1; i < soloers_num - 1; i++){
                    tmp = rand() % solocount;
                    if(tmp == leader && not_solo_riffs > 0){
                        soloers[i].id = SOLOER_NONE;
                    } else {
                        soloers[i].id = sololist[tmp];
                    }
                    soloers[i].measures_to_go = current_pattern->measures_count;
                }
                soloers[soloers_num-1].id = leader;
                soloers[soloers_num-1].measures_to_go = measures_count;
            }
        } else {
            soloers_num = 2;
            soloers = (soloer_s *) calloc(soloers_num, sizeof(soloer_s));
            soloers[0].id = SOLOER_NONE;
            soloers[0].measures_to_go = measures_count / 2;
            soloers[1].id = leader;
            soloers[1].measures_to_go = measures_count / 2 + (measures_count % 2);
        }

    } else {
        soloers_num = 1;
        soloers = (soloer_s *) calloc(soloers_num, sizeof(soloer_s));
        soloers[0].id = SOLOER_NONE;
        soloers[0].measures_to_go = measures_count;
    }

    soloer = 0;
}

int init_director_core(char* gen, char *sub, uint32_t solocount, uint32_t *sololist, int measures_count){
    struct rc_conf_s conf;
    genre = (char*) calloc(strlen(gen)+1, sizeof(char));
    strcpy(genre, gen);
    subgenre = (char*) calloc(strlen(sub)+1, sizeof(char));
    strcpy(subgenre, sub);
    impro_end = 0;

    available_scales.size = -1;

    srand(time(NULL));

    if(load_conf(DEFAULT_RC_PATH, &conf) < 4){
        fprintf(stderr, "error while loading configuration (%s)\n", strerror(errno));
        return FALSE;
    }

    database = db_connect(conf.db_host,
                          conf.db_name,
                          conf.db_user,
                          conf.db_passwd);
    if(database == NULL){
        fprintf(stderr, "failed to connect to database (%s):\n"
                "\tdb_host: %s\n"
                "\tdb_name: %s\n"
                "\tdb_user: %s\n"
                "\tdb_password: %s\n", strerror(errno), conf.db_host, conf.db_name, conf.db_user, conf.db_passwd);
        return FALSE;
    }

    if(get_genres(database, &genresList) <= -1){
        fprintf(stderr, "error while retrieving genres from db (%s)\n", strerror(errno));
        return FALSE;
    }

    if(get_subgenres(database, genre, &subgenresList) <= -1){
        fprintf(stderr, "error while retrieving subgenres from db (%s)\n", strerror(errno));
        return FALSE;
    }

    load_genre_info(genre, sub);
    if(!current_pattern){
        fprintf(stderr, "error while loading genre infos\n");
        return FALSE;
    }

    setupSoloers(solocount, sololist, measures_count);

    init_score_defs();

    return TRUE;
}

void free_director_core(){
    int i;
    variant_couple_s *tmp;

    tmp = improvariants;
    while(improvariants != NULL){
        free_db_results(improvariants->pattern);
        tmp = improvariants->next;
        free(improvariants);
        improvariants = tmp;
    }

    if(available_scales.size > 0){
        free(available_scales.list);
    }

    free_db_results(current_pattern);

    db_close(database);
}

int decide_next_measure(measure_s *measure, int current_measure_id){
    int i;

    if(rand() % 100 < (current_measure_id == 0 ? GENRE_CHANGE_ON_ONE_THRESHOLD : GENRE_CHANGE_THRESHOLD)){
        printf("\tchanged ");
        if(rand() % 100 < CHANGE_TO_SUBGENRE){
            pickSubgenre();
            printf("subgenre: %s\n", subgenre);
        } else {
            pickGenre();
            printf("genre; %s\n", genre);
        }
        load_genre_info(genre, subgenre);
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

    decidePriorities(measure, current_measure_id);
    /* Debug print */
    printf("\tprioargs: {");
    for (i = QUARTER_ARG_FIRST; i < QUARTER_QUERY_ARGS; i++)
	    printf("%d ", measure->prioargs[i]);
    printf("\t}\n");

    if(soloers[soloer].measures_to_go <= 0)
        soloer ++;
    measure->soloist_id = soloers[soloer].id;
    soloers[soloer].measures_to_go --;
    printf("\tsoloer: %d\n", measure->soloist_id);

    //if it is last measure and it's already time to stop, put an end to the improvisation
    if(0 && current_measure_id == (current_pattern->measures_count-1))
        impro_end = 1;

    return current_pattern->measures_count;
}

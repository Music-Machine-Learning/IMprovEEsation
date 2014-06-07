#include "notePicker.h"
#include <stdlib.h>

typedef struct noterange_t {
	uint8_t id;
	uint8_t range;
	struct noterange_t *next;
} noterange_t;

void insertRange(noterange_t **list, uint8_t note, uint8_t range){
	noterange_t *newRange;
	noterange_t *iter;
	
	newRange = malloc(sizeof(noterange_t));
	newRange->id = note;
	newRange->range = range;
	
	if((*list) == NULL || (*list)->range > range){
		newRange->next = *list;
		*list = newRange;
		return;
	}
	
	iter = *list;
	while(iter->next != NULL && iter->next->range <= range)
		iter = iter->next;
	
	newRange->next = iter->next;
	iter->next = newRange;	
}

uint8_t pickSingleNote(uint8_t *probabilities, uint8_t randomV){
	noterange_t *range = NULL;
	noterange_t *gauss[12];
	int rangeSize = 0, i, dir = DIR_BOTTOM;
	
	
	// parse input probabilities array
	for(i = 0; i < 12; i++){
		if(probabilities[i] > 0){
			insertRange(&range, i, probabilities[i]);
			rangeSize++;
		}
	}
	
	// prepare gaussian distributed array
	for(i = 0; i < rangeSize; i++, dir = 1-dir, range = range->next){
		switch(dir){
			case DIR_TOP:
				gauss[(rangeSize-1)-((i-1)/2)] = range;
				break;
			case DIR_BOTTOM:
				gauss[i/2] = range;
				break;
		}
	}
	
	// return note
	for(i = 0; i < rangeSize; randomV -= gauss[i]->range, i++){
		if(randomV <= gauss[i]->range){
			return gauss[i]->id;
		}
	}
}

#include <stdint.h>

#ifndef NOTEPICKER_H
#define NOTEPICKER_H

#define NULL 0
#define DIR_TOP 0
#define DIR_BOTTOM 1

/* this method returns a note extracted from probabilities vector based on randomV value */
uint8_t pickSingleNote(uint8_t *probabilities, uint8_t randomV);

#endif //NOTEPICKER_H

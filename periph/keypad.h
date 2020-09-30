/*
 * keypad.h
 *
 *  Created on: Oct 3, 2018
 *      Author: Robbie
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>
#include <stdbool.h>

void initKeypad( const uint8_t table[4][4] );

bool getKey( uint8_t* key );

uint8_t doLookup (uint8_t row, uint8_t col);

#endif /* KEYPAD_H_ */

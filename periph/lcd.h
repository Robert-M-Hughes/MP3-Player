/*
 * lcd.h
 *
 *  Created on: Aug 12, 2018
 *      Author: khughes
 */

#ifndef _LCD_H
#define _LCD_H

#include <stdint.h>

void initLCD( void );
void clearLCD ( void );
void positionLCD ( uint8_t y, uint8_t x );
void stringLCD( uint8_t *str );

#endif // _LCD_H

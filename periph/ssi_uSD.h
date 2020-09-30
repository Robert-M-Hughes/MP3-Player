/*
 * ssi_uSD.h
 *
 *  Created on: Aug 8, 2018
 *      Author: khughes
 */

#ifndef _SSI_USD_H
#define _SSI_USD_H

#include <stdint.h>
#include <stdbool.h>

// Initializes the QSSI0 module on PORTA for the MicroSD card.
void initSSI0( void );

// Sets the "baud rate" divisor to generate either a slow speed (~400KHz) 
// or high speed (~12.5MHz). If fast is false then low speed is configured; 
// this is used during card initialization.
void setSSI0Speed( bool fast );

// Transmit the byte data to the slave over QSSI0.
void txByteSSI0( uint8_t data );

// Receive a byte from the slave over QSSI0.
uint8_t rxByteSSI0( void );

// Set the chip select output level on PA3. If on is true the pin is set 
// low. The pin must not be changed when QSSI0 is busy.
void assertCS( bool on );

#endif // _SSI_USD_H

/*
 * ssi_uSD.c
 *
 *  Created on: Aug 8, 2018
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>

// SYSCTL, GPIO, and QSSI definitions.
#include "sysctl.h"
#include "gpio.h"
#include "qssi.h"

// Main oscillator clock frequency (in KHz)
#define  MAIN_OSC_FREQ  120000

// Slow bus speed (in KHz)
#define  SLOW_SPEED     400   

// Fast bus speed (in KHz)
#define  FAST_SPEED     12500   

// Prescalar
#define  PRESCALAR       2

// Constants for setting slow and fast speeds
#define SLOW_BAUD (MAIN_OSC_FREQ/SLOW_SPEED/PRESCALAR)
#define FAST_BAUD (MAIN_OSC_FREQ/FAST_SPEED/PRESCALAR)

// Initializes the QSSI0 module on PORTA for the MicroSD card.
void initSSI0( void ) {
  // Enable PORTA and QSSI0 gate clocks.
  SYSCTL[SYSCTL_RCGCGPIO] |= SYSCTL_RCGCGPIO_PORTA;
  SYSCTL[SYSCTL_RCGCSSI_SSI0] |= SYSCTL_RCGCSSI;
  SYSCTL[SYSCTL_RCGCSSI_SSI0] |= SYSCTL_RCGCSSI;

  //AFSEL  this is for the alternate on port a
  GPIO_PORTA[GPIO_AFSEL] |= GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5;


  GPIO_PORTA[GPIO_PCTL] &= ~( (0xf<<(4*2))|(0xf<<(4*4))|(0xf<<(4*5)) );
  GPIO_PORTA[GPIO_PCTL] |=  ( (0xf<<(4*2))|(0xf<<(4*4))|(0xf<<(4*5)) );

  //need the digital i/o for PA3 PORTA
  GPIO_PORTA[GPIO_DIR] |= GPIO_PIN_3;

  //now enable the pins
  GPIO_PORTA[GPIO_DEN] |= GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_5;

  //set SPO and SPH for mode 0
  QSSI0[QSSI_SSICR0] &= ~QSSI_CR0_SPO;      // SPO = 0
  QSSI0[QSSI_SSICR0] &= ~QSSI_CR0_SPH;      // SPH = 0

  // Disable SSI0
  QSSI0[QSSI_SSICR1] &= ~QSSI_CR1_SSE;

 // Set master mode - MS bit set to 0 for master
  QSSI0[QSSI_SSICR1] &= ~QSSI_CR1_MS;

  // Set prescaler
  QSSI0[QSSI_SSICPSR] = PRESCALAR;

  // Set CR0 for Freescale, 8-bit data, 400KHz
  QSSI0[QSSI_SSICR0] &= ~QSSI_CR0_FRF_MOTO;        // QSSI Frame Format Select bit field 5:4 ; 0x0 Freescale SPI Frame Format

  // 8-bit data
  QSSI0[QSSI_SSICR0] &= ~QSSI_CR0_DSS_M;        // clear SSI Data Size Select
  QSSI0[QSSI_SSICR0] |= QSSI_CR0_DSS_8;         //  8-bit data

  // 400Khz
  QSSI0[QSSI_SSICR0] &= ~QSSI_CR0_SCR_M;
  QSSI0[QSSI_SSICR0] |= (SLOW_BAUD << QSSI_CR0_SCR_S);

  // Enable SSI0
  QSSI0[QSSI_SSICR1] |= QSSI_CR1_SSE;
}

// Sets the "baud rate" divisor to generate either a slow speed (~400KHz) 
// or high speed (~12.5MHz). If fast is false then low speed is configured; 
// this is used during card initialization.
void setSSI0Speed( bool fast ) {
  // Clear the current baud rate divisor value
  QSSI0[QSSI_SSICR0] &= ~QSSI_CR0_SCR_M;

  switch( fast ) {
  case false:
    QSSI0[QSSI_SSICR0] |= (SLOW_BAUD << QSSI_CR0_SCR_S); // 400KHz
    break;
  default:
    QSSI0[QSSI_SSICR0] |= (FAST_BAUD << QSSI_CR0_SCR_S); // 12.5MHz
    break;
  }
}



// Transmit the byte data to the slave over QSSI0.
void txByteSSI0( uint8_t data ) {
  // wait until TX FIFO is not full
  while(!(QSSI0[QSSI_SR] & QSSI_SR_TNF));

  // write data to TX FIFO
  QSSI0[QSSI_DR] = data;

  // wait until RX FIFO is not empty
  while(!(QSSI0[QSSI_SR] & QSSI_SR_RNE));

  // read value from RX FIFO and discard
  volatile uint8_t dummy = QSSI0[QSSI_DR];
}

// Receive a byte from the slave over QSSI0.
uint8_t rxByteSSI0( void ) {
    while(!(QSSI0[QSSI_SR] & QSSI_SR_TNF));


    QSSI0[QSSI_DR] = 0xff;

    while(!(QSSI0[QSSI_SR] & QSSI_SR_RNE));

    return (uint8_t)QSSI0[QSSI_DR];

}

// Set the chip select output level on PA3. If on is true the pin is set 
// low. The pin must not be changed when QSSI0 is busy.
void assertCS( bool level ) {
   // check if QSSI0 is busy, if busy wait until it is not busy
    while(QSSI0[QSSI_SR] & QSSI_SR_BSY);

    if(level){
        //(*GPIO_PORTA) &= ~(GPIO_PIN_3 << 2);
        GPIO_PORTA[GPIO_PIN_3] &= ~GPIO_PIN_3;
    }
    else
        GPIO_PORTA[GPIO_PIN_3] |= GPIO_PIN_3;
}

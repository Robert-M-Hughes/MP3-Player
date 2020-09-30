/*
 * timer3A.c
 *
 *  Created on: Nov 28, 2018
 *      Author: rhughes
 */



#include <stdint.h>
#include <stdbool.h>

// SYSCTL, GPIO, PPB, and Timer3a definitions.
#include "sysctl.h"
#include "gptm.h"
#include "ppb.h"
#include "timer3A.h"

// Main oscillator clock frequency
#define  MAIN_OSC_FREQ  120000000

// Configure Timer3A as a 32-bit periodic timer which will generate an
// interrupt on time-out. Note: this procedure DOES NOT enable the timer
// or set the reload value; that is done by the procedures enableTimer3A()
// and setTimer3ARate().
void initTimer3A( void ) {
  // Enable Run Clock Gate Control
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3;

  // 32-bit timer
  GPTM_TIMER3[GPTM_CFG] &= ~GPTM_CFG_M;
  GPTM_TIMER3[GPTM_CFG] |= GPTM_CFG_32BITTIMER;

  // periodic timer
  GPTM_TIMER3[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
  GPTM_TIMER3[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

  // Set the time-out interrupt mask.
  // INSERT CODE HERE
  GPTM_TIMER3[GPTM_IMR] |= GPTM_IMR_TATOIM;

  // Enable timer interrupt in NVIC.
  // INSERT CODE HERE
  PPB[PPB_EN1] |= PPB_EN1_TIMER3A;
}

// Enables/disables Timer3A. If enabled is true, the timer is enabled.
void enableTimer3A( bool enabled ) {
  clearTimer3A();

  // Enable or disable the timer.
  // INSERT CODE HERE
  if(enabled){
      GPTM_TIMER2[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
  }
  else{
      GPTM_TIMER2[GPTM_CTL] &= ~GPTM_CTL_TAEN;
  }

}

// Sets Timer3A to reload at the frequency rate (in Hz).
void setTimer3ARate( uint16_t rate ) {
  GPTM_TIMER2[GPTM_TAILR] = (MAIN_OSC_FREQ/rate)-1;
}

// Clears the time-out flag for Timer3A (which is set on a time-out).
void clearTimer3A( void ) {
  GPTM_TIMER2[GPTM_ICR] |= GPTM_ICR_TATOCINT;
}




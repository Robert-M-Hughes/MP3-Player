// boilerplate for timer control

#include <stdint.h>
#include <stdbool.h>
// The two # lines below control whether to compile the SYSTICK code or your
// Timer3A code.  The following line defines a symbol which is used to enable
// only the SYSTICK related code.
#define USING_SYSCTL            
#define USING_16BIT
// Uncommenting this line disables the SYSTICK code and enables your Timer3A
// code.
#undef USING_SYSCTL

#ifdef USING_SYSCTL

// Copy these definitions into a "ppb.h" header file.  Create it using the
// same format as sysctl.h and gpio.h.
#define PPB                     ((volatile uint32_t *) 0xe000e000)


#else

#include "sysctl.h"
#include "gptm.h"
#endif
// osc.h defines MAINOSCFREQ
#include "osc.h"
#include "ppb.h"

#ifdef USING_SYSCTL
#define DIVISOR 1
#else
#ifdef USING_16BIT
#define DIVISOR 100
#else
#define DIVISOR 1
#endif
#endif

// Frequency of switching between left and right LED display
#define   SWITCHFREQ    200

// Add the calculation for your reload value here.
#define RELOAD_VALUE    (MAINOSCFREQ/SWITCHFREQ)/DIVISOR

// Configure Timer3A as a periodic timer with a period of 1/200th second.
// The names of each register you will need are defined in gptm.h; you do not 
// need to use any registers not named there.  You will need to define the 
// offsets for each bit or field in the registers.

// ALSO: remember to enable the Run Mode Clock Gating for the timer before
// attempting to access any of its registers!
void initTimer( void ) {
#ifdef USING_SYSCTL

  PPB[PPB_STCTRL] = 0;
  PPB[PPB_STRELOAD] = RELOAD_VALUE - 1;
  PPB[PPB_STCTRL] = PPB_STCTRL_CLK_SRC | PPB_STCTRL_ENABLE;

#else
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER3;

  GPTM_TIMER3[GPTM_CTL] &= ~GPTM_CTL_TAEN;

  GPTM_TIMER3[GPTM_CFG] &= ~GPTM_CFG_M;

#ifdef USING_16BIT
  GPTM_TIMER3[GPTM_CFG] |= GPTM_CFG_16BITTIMER;
#else
  GPTM_TIMER3[GPTM_CFG] |= GPTM_CFG_32BITTIMER;
#endif

  GPTM_TIMER3[GPTM_TAMR] = ( GPTM_TIMER3[GPTM_TAMR] & ~GPTM_TAMR_TAMR_M) | GPTM_TAMR_TAMR_PERIODIC;

  GPTM_TIMER3[GPTM_TAILR] = RELOAD_VALUE - 1;
#ifdef USING_16BIT
  GPTM_TIMER3[GPTM_TAPR] = DIVISOR - 1;
#endif
  GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;

  GPTM_TIMER3[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
  GPTM_TIMER3[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;

#endif

}

// Wait for Timer3A to reload, then return.  Perform any necessary actions to
// clear the reload condition.  DO NOT turn off the timer!
void waitOnTimer( void ) {
#ifdef USING_SYSCTL

  while( ( PPB[PPB_STCTRL] & PPB_STCTRL_COUNT ) == 0 );


#else

  while( ( GPTM_TIMER3 [GPTM_RIS] & GPTM_RIS_TATORIS) == 0);

  GPTM_TIMER3[GPTM_ICR] = GPTM_ICR_TATOCINT;

#endif
}

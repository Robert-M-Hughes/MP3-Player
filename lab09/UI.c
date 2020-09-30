/*
 * UI.c
 *
 *  Created on: Nov 1, 2018
 *      Author: khughes
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "gptm.h"
#include "gpio.h"
#include "ppb.h"
#include "sysctl.h"
#include "keypad.h"
#include "sound.h"
#include "control.h"
#include "lcd.h"
#include "ID3.h"
#include "timer3A.h"



// Return key value upon initial key press.  Since keys are
// ASCII, they cannot have a value greater than 0xff, so when
// no new key is pressed the procedure returns UINT16_MAX.
static uint16_t UIKey( void ) { 
  static enum {NOT_PRESSED, PRESSED} state = NOT_PRESSED;
  uint8_t key;

  switch( state ) {
  case NOT_PRESSED:
    if( getKey( &key ) ) {
      state = PRESSED;
      return key;
    }
    break;
  case PRESSED:
    if( !getKey( &key ) )
      state = NOT_PRESSED;
    break;
  }
  return UINT16_MAX;
}
//ID3 tags
struct id3tags tags;


enum keycmds_t {
  SCROLL_TITLE  = '1',
  SCROLL_ARTIST = '4',
  SCROLL_ALBUM  = '7',
  PLAY_PAUSE    = 'A',
  SHUFFLE       = 'B',
  VOLUME_UP     = 'C',
  VOLUME_DOWN   = 'D',
  SKIP_BACKWARD = 'E',
  SKIP_FORWARD  = 'F',
};
static   bool scrollTitle= true;
static   bool scrollArtist= true;
static   bool scrollAlbum= true;

// Interrupt handler for user interface.  When called it scans the
// keypad and if a new key is detected, performs necessary actions.
void UIHandler( void ) {
  uint16_t key = UIKey( ); 

  if( key != UINT16_MAX ) {
    switch( (enum keycmds_t)key ) {
    case SCROLL_TITLE:    // '1'
        if (scrollTitle){
            scrollTitle = false;
            displaySong(&tags,20,0,0);
        }
        else{
            scrollTitle = true;
            displaySong(&tags,0,0,0);
        }
      break;
    case SCROLL_ARTIST:    // '4'
        //scroll = true;
        if (scrollArtist){
            scrollArtist = false;
            displaySong(&tags,0,20,0);
        }
        else{
            scrollArtist = true;
            displaySong(&tags,0,0,0);
        }
      break;
    case SCROLL_ALBUM:    // '7'
      //scroll = true;
        if (scrollAlbum){
            scrollAlbum = false;
            displaySong(&tags,0,0,20);
        }
        else{
            scrollAlbum = true;
            displaySong(&tags,0,0,0);
        }
      break;
    case PLAY_PAUSE:    // 'A'
      setPaused( !isPaused() );
      break;
    case SHUFFLE:       // 'B'
        onOffShuffle();
      break;
    case VOLUME_UP:     // 'C'
        upVolume();
      break;
    case VOLUME_DOWN:   // 'D'
        downVolume();
      break;
    case SKIP_BACKWARD: // '*'
        skipBack();
      break;
    case SKIP_FORWARD:  // 'F'
      setDone();
      break;
    default:            // other unused keys
      break;
    }
  }

  // Clear the time-out.
  GPTM_TIMER5[GPTM_ICR] |= GPTM_ICR_TATOCINT;
}

#define KEYPAD_PHONE
 //#define KEYPAD_ABT

static const uint8_t lookup[4][4] = {
#ifdef KEYPAD_PHONE
  {SCROLL_TITLE,           '2', '3',          PLAY_PAUSE},
  {SCROLL_ARTIST,           '5', '6',          SHUFFLE},
  {SCROLL_ALBUM,           '8', '9',          VOLUME_UP},
  {SKIP_BACKWARD, '0', SKIP_FORWARD, VOLUME_DOWN},
#endif

#ifdef KEYPAD_ABT
  {'0',           SCROLL_TITLE,          '2',         '3'},
  {SCROLL_ARTIST,           '5',          '6',         SCROLL_ALBUM},
  {'8',           '9',          VOLUME_UP,   SHUFFLE},
  {SKIP_BACKWARD, SKIP_FORWARD, VOLUME_DOWN, PLAY_PAUSE},
#endif
};

void initUI( void ) {
  // Prep the keypad.
  initKeypad( lookup );

  // Enable Run Clock Gate Control
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER5;
  SYSCTL[SYSCTL_RCGCTIMER] |= SYSCTL_RCGCTIMER_TIMER5;

  // 32-bit periodic timer.
  GPTM_TIMER5[GPTM_CFG] &= ~GPTM_CFG_M;
  GPTM_TIMER5[GPTM_CFG] |= GPTM_CFG_32BITTIMER;
  GPTM_TIMER5[GPTM_TAMR] &= ~GPTM_TAMR_TAMR_M;
  GPTM_TIMER5[GPTM_TAMR] |= GPTM_TAMR_TAMR_PERIODIC;

  // Set reload value for 20Hz
  GPTM_TIMER5[GPTM_TAILR] = (120000000/20)-1;

  // Enable the interrupt.
  GPTM_TIMER5[GPTM_IMR] |= GPTM_IMR_TATOIM;

  // Enable interrupt for timer.
  PPB[PPB_EN2] |= PPB_EN2_TIMER5A;

  // Set priority level to 1 (lower priority than Timer2A).
  PPB[PPB_PRI16] = ( PPB[PPB_PRI16] & PPB_PRI_INTB_M ) | (1 << PPB_PRI_INTB_S);

  // Clear the time-out.
  GPTM_TIMER5[GPTM_ICR] |= GPTM_ICR_TATOCINT;

  // Enable the timer.
  GPTM_TIMER5[GPTM_CTL] |= GPTM_CTL_TAEN | GPTM_CTL_TASTALL;
}

void displaySong( struct id3tags * tags, uint8_t x, uint8_t y, uint8_t z ){

        clearLCD();
        positionLCD(0,-x);
        stringLCD(tags->title);
        positionLCD(1,-y);
        stringLCD(tags->artist);
        positionLCD(2,-z);
        stringLCD(tags->album);

}
/*
void timer3_control( void ){
    enableTimer3A(true);
    rate = 2; //Hertz (movement/second)
    setTimer3ARate( rate );

}
*/


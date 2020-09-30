/*
 * lab08.c
 *
 *  Created on: Aug 13, 2018
 *      Author: khughes
 *
 *      KEYS WITH FUNCTIONS:
 *      Specification 3: Shuffle ---- KEY B
 *
 *      Specification 4: Screen Roll to see rest of Words: KEY 1 -- Scroll Title
 *                                                         KEY 4 --- Scroll Artist
 *                                                         KEY 7 --- Scroll Album
 *
*       Specification 5: Please Go Easy on us C is not my first Language
 *
 *
 */

#include <stdint.h>
#include <stdbool.h>

// Peripherals
#include "osc.h"
#include "ssi_uSD.h"
#include "ssi_DAC.h"
#include "timer2A.h"
#include "UI.h"

// FatFs
#include "ff.h"

// Utilities
#include "MP3utils.h"
#include "ID3.h"

// Other project files
#include "mp3.h"
#include "sound.h"
#include "control.h"

// ID3 tags
struct id3tags tags;

// Decode and play an MP3 file.
void playSong( uint8_t song  ) {
  FIL fp;
  // Get the file from the MicroSD card.
  findMP3( song, &fp );

  // Process ID3 header (if any).
  getID3Tags( &fp , &tags );

  //display the song
  displaySong(&tags,0,0,0);


  // Prepare for sound output.
  initSound();

   // Decode and play the song.
  MP3decode( &fp );

   // Wait for the last data to be sent to the DACs.
  while( isSoundFinished() == false ) __asm( " wfi" );

  // Stop the DAC timer.
  enableTimer2A( false );
}

main() {
  // Initialize clock, SSIs, and Timer
  initOsc();
  initSSI0();
  initSSI2();
  initTimer2A();
  initUI();
  initLCD();
  uint8_t keyPressed;
  // Initialize structure.
  FATFS fs; f_mount( &fs, "", 0 );

  // Find out how many MP3 files are on the card.
  uint8_t numSongs = countMP3();
  setNumSongs( numSongs );

  // Obligatory endless loop.
  while( true ) {
    // Get the next file from the MicroSD card.
    uint8_t song = getSong();
    //display(song);


    // Send the file to the MP3 decoder
    playSong( song );
  }
}

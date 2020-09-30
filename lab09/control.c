/*
 * control.c
 *
 *  Created on: Aug 13, 2018
 *      Author: khughes
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "control.h"

// The total number of songs on the MicroSD card.
static uint8_t numSongs;
// The next song to play.
static uint8_t song = 0;
// Indicates termination of current song by user.
static bool done = false;
// Indicates whether decoding/playing is playing or paused.
static bool paused = false;
// Indicates if the MP3 player is in song shuffle mode.
static bool shuffle = false;

//static uint8_t songarray[];
//song array order
static uint8_t *songs = 0;

// Private procedure for selecting the next song in shuffle mode.
static uint8_t getShuffle( uint8_t song ) {
  return song;
}

// Return the number of the song to play.  Initially, just
// return the next sequential song, and wrap back to 0 when all
// songs have been returned.
uint8_t getSong( void ) {
  // Save the song number for the end.
  uint8_t tmp = song;

  // Pick the next song to play.

    song = ( song + 1 ) % numSongs;

    setSong( song );


  // Return song number.
  return songs[tmp];
}

// Store the total number of songs (needed by getSong()).
void setNumSongs( uint8_t num ) {
    numSongs = num;
    // Free the array if it already set
    if(songs != 0) {
        free(songs);
    }
    songs = malloc(num * sizeof(uint8_t));
    resetArray();

}
//reset to the original array
void resetArray( void ){
    for (uint8_t i = 0; i < numSongs; i++) {
        songs[i] = i;
    }
}

//shuffle the array

void shuffleSongs(){
    if (numSongs > 1) {
        uint8_t i;
        for (i = 0; i < numSongs - 1; i++) // runs through all songs on the playlist
        {
          uint8_t j = i + rand() / (RAND_MAX / (numSongs - i) + 1); // utilizes rand() to shuffle the song spot
          uint8_t t = songs[j];
          songs[j] = songs[i]; // sets the random spot to the original array
          songs[i] = t;
        }
    }
}


// Store the total number of songs (needed by getSong()).
uint8_t getNumSongs( void ) {
  return numSongs;
}

// Set next song explicitly.
void setSong( uint8_t v ) {
  song = v;
}

// Indicates whether the current MP3 song should be terminated early.
bool isDone( void ) {
  bool tmp = done;
  done = false;
  return tmp;
}

// Set song to terminate.
void setDone( void ) {
  done = true;
}

// Indicates whether the MP3 player is paused.
bool isPaused( void ) {
  return paused;
}

// Set state of pause.
void setPaused( bool v ) {
  paused = v;
}

// Indicates state of shuffle mode.
bool isShuffle( void ) {
  return shuffle;
}

// Set state of shuffle mode.

void setShuffle( bool v ) {
    shuffle = v;
    if(shuffle) { // if button is set to shuffle
        shuffleSongs(); // shuffles the array of songs into random order
        song = 0;
        setDone();
    } else {
        resetArray(); // resets the array if songs were previously shuffled
        song = 0;
        setDone();
    }
}

void onOffShuffle( void ){
    setShuffle( !shuffle );
}

void skipBack(){
    if (song == 0){ // on last song of the playlist
        song = numSongs -2; // song is equal to the next song so need to go back 2
    }else if(song == 1){ // first song
       song = numSongs-1; // go back to the last song
    }else{ // any other song in the playlist
        song = song - 2;
    }
    done = true;
}




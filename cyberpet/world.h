#include "Arduino.h"
#include "squirrel_hash.h"
#include "noisemap.h"

#ifndef MAP_W
#define MAP_W 8
#endif

#ifndef MAP_H
#define MAP_H 14
#endif

#ifndef EXIT_N
#define EXIT_N 1
#define EXIT_E 2
#define EXIT_S 4
#define EXIT_W 8
#endif

// Herringbone tile pattern for world room exits
// https://nothings.org/gamedev/herringbone/index.html
const byte world_tile_exit_patterns [] PROGMEM = {
	// Row 1
  EXIT_N | EXIT_E | EXIT_W,
  EXIT_N | EXIT_S | EXIT_W,
  EXIT_N | EXIT_E | EXIT_S,
  EXIT_E | EXIT_S | EXIT_W,

  // Row 2
  EXIT_E | EXIT_S | EXIT_W,
  EXIT_N | EXIT_E | EXIT_W,
  EXIT_N | EXIT_S | EXIT_W,
  EXIT_N | EXIT_E | EXIT_S,

  // Row 3
  EXIT_N | EXIT_E | EXIT_S,
  EXIT_E | EXIT_S | EXIT_W,
  EXIT_N | EXIT_E | EXIT_W,
  EXIT_N | EXIT_S | EXIT_W,

  // Row 4
  EXIT_N | EXIT_S | EXIT_W,
  EXIT_N | EXIT_E | EXIT_S,
  EXIT_E | EXIT_S | EXIT_W,
  EXIT_N | EXIT_E | EXIT_W,
};

#ifndef WORLD_H
#define WORLD_H

byte get_room_exits(int world_x, int world_y);
unsigned int get_room_level(int world_x, int world_y);
unsigned int get_room_level(int world_x, int world_y, int world_z);

#endif
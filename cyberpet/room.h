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

#define ROOM_FLOOR 0
#define ROOM_WALL 1
#define ROOM_EXIT_N 2
#define ROOM_EXIT_E 3
#define ROOM_EXIT_S 4
#define ROOM_EXIT_W 5
#define ROOM_WALL_DECO 6
#define ROOM_PILLAR1 11
#define ROOM_PILLAR2 12
#define ROOM_PILLAR3 13
#define ROOM_PILLAR4 14
#define ROOM_PILLAR5 15
#define ROOM_PILLAR6 16
#define ROOM_PILLAR7 17
#define ROOM_PILLAR8 18
#define ROOM_PILLAR9 19
#define ROOM_PILLAR10 20

#ifndef ROOM_H
#define ROOM_H

int setup_room(byte wall_map[MAP_W][MAP_H], int tile_map[MAP_W][MAP_H], int world_x, int world_y, int world_tile_data[15], int area_x, int area_y, byte exits, unsigned int seed);
int build_djikstra_map(byte djikstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H], unsigned int seed);

#endif
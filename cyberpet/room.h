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

#ifndef DIR_N
#define DIR_N 1
#define DIR_E 2
#define DIR_S 4
#define DIR_W 8
#endif

#ifndef ROOM_H
#define ROOM_H

#include "Arduino.h"
#include "squirrel_hash.h"
#include "noisemap.h"

#define ROOM_FLOOR 0
#define ROOM_WALL 1
#define ROOM_WALL_DECO 2
#define ROOM_EXIT_N 3
#define ROOM_EXIT_E 4
#define ROOM_EXIT_S 5
#define ROOM_EXIT_W 6
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

#define DJIKSTRA_MAX 255

#define ROOM_TILESET_SIZE 24
#define ROOM_TILESET_FLOOR 0
#define ROOM_TILESET_WALL 1
#define ROOM_TILESET_WALL_DECO 2
#define ROOM_TILESET_EXIT 3
#define ROOM_TILESET_H_DECO 4
#define ROOM_TILESET_V_DECO 5

#define ROOM_TILES_DUNGEON 0
#define ROOM_TILES_SEA_TOWN 1
#define ROOM_TILES_TOWN 2
#define ROOM_TILES_SEA_CITY 3
#define ROOM_TILES_CITY 4
#define ROOM_TILES_DEEP_SEA 5
#define ROOM_TILES_SEA 6
#define ROOM_TILES_MOUNTAIN 7
#define ROOM_TILES_PEAK 8
#define ROOM_TILES_ICE_FIELD 9
#define ROOM_TILES_TUNDRA 10
#define ROOM_TILES_TAIGA 11
#define ROOM_TILES_PLAINS 12
#define ROOM_TILES_FOREST 13
#define ROOM_TILES_DEEP_FOREST 14
#define ROOM_TILES_DESERT 15
#define ROOM_TILES_SAVANNAH 16
#define ROOM_TILES_JUNGLE 17

// Room tileset array contains sets of 4 possible tiles for each tile type:
// {
//   -1,-1,-1,-1, // ROOM_TILESET_FLOOR
//    1, 1, 1, 1, // ROOM_TILESET_WALL
//    2, 2, 2, 2, // ROOM_TILESET_WALL_DECO
//   -1,-1,-1,-1, // ROOM_TILESET_EXIT, this is used for all doors
//    2, 2, 2, 2, // ROOM_TILESET_H_DECO, 2x1 decorative wall replacements, 2x left/right
//    2, 2, 2, 2, // ROOM_TILESET_V_DECO, 1x2 decorative wall replacements, 2x top/bottom
// }
const int room_tiles_dungeon1 [ROOM_TILESET_SIZE] = {-1,432,608, 839,838,837,840, 1127,860,1128,787, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_sea_town1 [ROOM_TILESET_SIZE] = {-1,1980,1978,1979, 441,440,1960,1956, 1519,618,1510,860, 725,725,725,725, 2,2,2,2, 2,2,2,2,};

const int room_tiles_town1 [ROOM_TILESET_SIZE] = {-1,432,609,620, 441,440,1960,1956, 1438,787,326,860, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_sea_city1 [ROOM_TILESET_SIZE] = {-1,1980,1978,1979, 441,442,438,1519, 1978,1523,1521,860, 1537,1537,1537,1537, 2,2,2,2, 2,2,2,2,};

const int room_tiles_city1 [ROOM_TILESET_SIZE] = {-1,432,608,620, 1519,1523,1522,839, 326,1511,787,860, -1,651,651,651, 2,2,2,2, 2,2,2,2,};

const int room_tiles_deep_sea1 [ROOM_TILESET_SIZE] = {-1,1980,1978,1979, 441,442,438,1517, 2,2,2,2, 1537,1537,1537,1537, 2,2,2,2, 2,2,2,2,};

const int room_tiles_sea1 [ROOM_TILESET_SIZE] = {-1,1980,1978,1979, 441,440,1960,1956, 870,618,1510,2, 1537,1537,1537,1537, 2,2,2,2, 2,2,2,2,};

const int room_tiles_mountain1 [ROOM_TILESET_SIZE] = {-1,870,871,432, 518,517,1515,1516, 1273,1277,1216,1217, 439,439,439,439, 2,2,2,2, 2,2,2,2,};

const int room_tiles_peak1 [ROOM_TILESET_SIZE] = {-1,870,871,432, 1516,1517,1516,1517, 1273,1277,1216,1217, 715,715,715,715, 2,2,2,2, 2,2,2,2,};

const int room_tiles_ice_field1 [ROOM_TILESET_SIZE] = {-1,432,436,441, 435,435,435,435, 2,2,2,2, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_tundra1 [ROOM_TILESET_SIZE] = {-1,-1,-1,-1, 434,434,434,434, 2,2,2,2, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_taiga1 [ROOM_TILESET_SIZE] = {-1,-1,-1,-1, 1509,1509,1509,1509, 2,2,2,2, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_plains1 [ROOM_TILESET_SIZE] = {-1,609,620,621, 616,617,618,1510, 2,2,2,2, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_forest1 [ROOM_TILESET_SIZE] = {-1,609,610,871, 1511,1510,1512,950, 616,612,520,1273, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_deep_forest1 [ROOM_TILESET_SIZE] = {608,609,432,610, 1511,1510,1512,950, 616,612,520,1273, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_desert1 [ROOM_TILESET_SIZE] = {-1,432,608,1946, 1933,1932,1934,1273, 1923,1921,1941,1017, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_savannah1 [ROOM_TILESET_SIZE] = {-1,608,609,610, 1373,618,612,1, 2,2,2,2, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

const int room_tiles_jungle1 [ROOM_TILESET_SIZE] = {608,609,1971,1970, 1512,1512,1510,1973, 618,617,1978,1979, -1,-1,-1,-1, 2,2,2,2, 2,2,2,2,};

extern const int *room_tilesets[72];

int setup_room(byte wall_map[MAP_W][MAP_H], int tile_map[MAP_W][MAP_H], int world_x, int world_y, int world_tile_data[15], int area_x, int area_y, byte exits, unsigned int seed);
int get_room_tileset_index(int world_tile_data[15], int world_x, int world_y, int area_x, int area_y, unsigned int seed);
bool room_tile_walkable(byte wall_map[MAP_W][MAP_H], int tile_x, int tile_y);

int clear_djikstra_map(int djikstra_map[MAP_W][MAP_H]);
int get_djikstra_lowest(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y);
int build_djikstra_map(int djikstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H], int goal_x, int goal_y);
int merge_djikstra_maps(int output_map[MAP_W][MAP_H], int input1_map[MAP_W][MAP_H]);
int get_djikstra_direction(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y, unsigned int seed);

#endif
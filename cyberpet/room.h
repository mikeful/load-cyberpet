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
const int room_tiles_dungeon1 [ROOM_TILESET_SIZE] =     {-1,-1,432,608, 839,838,837,839,     1127,860,1128,787,   -1,706,688,705,      766,768,1006,1007,   2,2,2,2,};

const int room_tiles_sea_town1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 441,440,437,1956,    1519,1520,950,860,   725,725,725,725,     985,986,771,755,     2,2,2,2,};

const int room_tiles_town1 [ROOM_TILESET_SIZE] =        {-1,-1,432,608, 1519,1520,1471,1510, 1438,787,326,860,    -1,-1,-1,-1,         852,853,1027,1028,   2,2,2,2,};

const int room_tiles_sea_city1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 441,440,437,1956,    1978,1523,1521,860,  1537,1537,1537,1537, 985,986,852,853,     2,2,2,2,};

const int room_tiles_city1 [ROOM_TILESET_SIZE] =        {-1,-1,432,608, 1519,1523,1522,839,  326,1511,787,860,    -1,651,687,651,      756,771,961,964,     2,2,2,2,};

const int room_tiles_deep_sea1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 442,438,1604,441,    1980,1273,1964,950,  1537,1537,1537,1537, 1275,1276,1711,1712, 2,2,2,2,};

const int room_tiles_sea1 [ROOM_TILESET_SIZE] =         {-1,-1,432,608, 441,440,437,1956,    1273,1964,950,1980,  1537,1537,1537,1537, 550,551,1275,1276,   2,2,2,2,};

const int room_tiles_mountain1 [ROOM_TILESET_SIZE] =    {-1,-1,870,871, 518,517,1515,1516,   1273,1277,1216,1217, 439,439,439,439,     954,955,1275,1276,   2,2,2,2,};

const int room_tiles_peak1 [ROOM_TILESET_SIZE] =        {-1,-1,870,871, 1516,1517,1516,1517, 1273,1277,1216,1217, 715,715,715,715,     1262,1263,956,957,   2,2,2,2,};

const int room_tiles_ice_field1 [ROOM_TILESET_SIZE] =   {-1,-1,432,608, 435,435,435,435,     1261,1217,1218,1216, -1,-1,-1,-1,         1262,1263,1266,1267, 2,2,2,2,};

const int room_tiles_tundra1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 604,1509,1273,434,   517,1216,613,1017,   -1,-1,-1,-1,         956,957,1935,1936,   2,2,2,2,};

const int room_tiles_taiga1 [ROOM_TILESET_SIZE] =       {-1,-1,432,608, 1509,951,1438,945,   615,1438,520,1273,   -1,-1,-1,-1,         552,553,954,955,     2,2,2,2,};

const int room_tiles_plains1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 615,613,1377,1510,   611,616,614,605,     -1,-1,-1,-1,         1439,1440,1965,1966, 2,2,2,2,};

const int room_tiles_forest1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 1511,1510,1509,950,  615,1438,520,1273,   -1,-1,-1,-1,         550,551,599,601,     2,2,2,2,};

const int room_tiles_deep_forest1 [ROOM_TILESET_SIZE] = {-1,-1,432,608, 1512,1511,1510,1238, 615,605,520,607,     -1,-1,-1,-1,         550,551,599,601,     2,2,2,2,};

const int room_tiles_desert1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 1933,1932,1934,1273, 1923,1921,1941,1017, -1,-1,-1,-1,         1935,1936,1275,1276, 2,2,2,2,};

const int room_tiles_savannah1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 1373,615,611,614,    611,613,614,605,     -1,-1,-1,-1,         1939,1940,599,601,   2,2,2,2,};

const int room_tiles_jungle1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 1512,1512,1510,1973, 618,617,441,440,     -1,-1,-1,-1,         599,601,1951,1952,   2,2,2,2,};

extern const int *room_tilesets[72];

int setup_room(byte wall_map[MAP_W][MAP_H], int tile_map[MAP_W][MAP_H], int world_x, int world_y, int world_tile_data[15], int area_x, int area_y, byte exits, unsigned int seed);
int get_room_tileset_index(int world_tile_data[15], int world_x, int world_y, int area_x, int area_y, unsigned int seed);
int get_room_tile_index(int tile_x, int tile_y, byte tile_type, unsigned int seed);
bool room_tile_walkable(byte wall_map[MAP_W][MAP_H], int tile_x, int tile_y);

int clear_djikstra_map(int djikstra_map[MAP_W][MAP_H]);
int get_djikstra_lowest(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y);
int build_djikstra_map(int djikstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H], int goal_x, int goal_y);
int merge_djikstra_maps(int output_map[MAP_W][MAP_H], int input1_map[MAP_W][MAP_H]);
int get_djikstra_direction(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y, unsigned int seed);

#endif
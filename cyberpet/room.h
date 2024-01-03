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
const int room_tiles_dungeon1 [ROOM_TILESET_SIZE] =     {-1,-1,432,608, 839,836,840,837,     860,83,1000,1017,    706,689,705,689,     766,768,1006,1007,   1132,1128,978,976,}; // generic
const int room_tiles_dungeon2 [ROOM_TILESET_SIZE] =     {-1,-1,432,608, 836,839,840,837,     887,892,891,893,     706,689,705,689,     983,984,1006,1007,   1002,1003,895,894,}; // graves
// TODO cave
// TODO cultist base

const int room_tiles_sea_town1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 441,440,437,1956,    1519,1526,950,975,   1491,1496,1492,1499, 985,986,771,755,     1031,688,860,1121,}; // waves
const int room_tiles_sea_town2 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 441,1519,440,1526,   609,326,328,975,     1492,1499,1491,1496, 989,990,771,755,     1031,688,860,1121,}; // buildings
const int room_tiles_sea_town3 [ROOM_TILESET_SIZE] =    {-1,-1,-1,432,  1458,1461,1496,1499, 326,785,773,684,     687,687,687,687,     766,768,756,771,     764,763,760,769,}; // house
const int room_tiles_sea_town4 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 1458,1461,1496,1499, 777,787,773,775,     687,687,687,687,     1010,1011,771,755,   771,1339,783,975,};  // storage

const int room_tiles_town1 [ROOM_TILESET_SIZE] =        {-1,-1,432,608, 1519,1523,1526,1413, 1382,326,328,975,    -1,-1,-1,-1,         852,853,1027,1028,   1031,688,860,1121,}; // buildings
const int room_tiles_town2 [ROOM_TILESET_SIZE] =        {-1,-1,-1,432,  1458,1461,1496,1499, 326,785,773,684,     687,687,687,687,     766,768,756,771,     764,763,760,769,}; // house
const int room_tiles_town3 [ROOM_TILESET_SIZE] =        {-1,-1,432,608, 839,836,840,837,     777,787,773,775,     687,687,687,687,     1010,1011,771,755,   771,1339,783,975,}; // storage
// TODO crafting/farming

const int room_tiles_sea_city1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 441,440,1956,1522,   326,1523,1526,975,   1537,1537,1537,1537, 985,986,852,853,     860,1121,1540,688,}; // waves
const int room_tiles_sea_city2 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 441,440,1522,1523,   326,328,1526,1964,   1537,1537,1537,1537, 989,990,961,964,     1031,688,1540,688,}; // buildings
const int room_tiles_sea_city3 [ROOM_TILESET_SIZE] =    {-1,-1,-1,432,  839,836,840,837,     326,328,773,781,     687,687,687,687,     766,768,756,771,     764,763,760,769,}; // house
const int room_tiles_sea_city4 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 839,836,840,837,     781,773,775,785,     687,687,687,687,     1010,1011,771,755,   771,1339,783,975,};  // storage
// TODO boat?

const int room_tiles_city1 [ROOM_TILESET_SIZE] =        {-1,-1,432,608, 839,836,1523,1526,   326,328,1522,975,    689,689,689,689,     756,771,961,964,     320,896,1540,688,}; // buildings
const int room_tiles_city2 [ROOM_TILESET_SIZE] =        {-1,-1,-1,432,  839,836,840,837,     326,328,773,781,     687,687,687,687,     766,768,756,771,     764,763,760,769,}; // house
const int room_tiles_city3 [ROOM_TILESET_SIZE] =        {-1,-1,432,608, 839,836,840,837,     781,773,775,785,     687,687,687,687,     1010,1011,771,755,   771,1339,783,975,}; // storage
const int room_tiles_city4 [ROOM_TILESET_SIZE] =        {-1,-1,-1,432,  839,836,840,837,     326,328,327,336,     687,687,687,687,     766,768,337,338,     764,763,337,330,}; // party

const int room_tiles_deep_sea1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608, 442,438,441,441,     1980,1273,1964,950,  1537,1537,1537,1537, 1275,1276,1711,1712, 1018,1019,1222,1221,}; // big waves

const int room_tiles_sea1 [ROOM_TILESET_SIZE] =         {-1,-1,432,608, 441,440,437,1956,    1273,1964,950,1980,  1537,1537,1537,1537, 550,551,1275,1276,   1222,1221,955,944,}; // waves/rocks

const int room_tiles_mountain1 [ROOM_TILESET_SIZE] =    {-1,-1,608,870, 1516,1517,1273,517,  1215,1219,512,951,   714,498,716,497,     954,955,1275,1276,   1509,1273,1222,1221,}; // high level
// TODO cave

const int room_tiles_peak1 [ROOM_TILESET_SIZE] =        {-1,-1,870,871, 1517,1516,1266,1216, 1217,512,1262,1263, 714,497,716,498,     1262,1263,956,957,   1264,1265,1222,1221,}; // high level
// TODO cave

const int room_tiles_ice_field1 [ROOM_TILESET_SIZE] =   {432,436,608,433, 512,513,1266,1267, 1216,1217,1262,1263, 433,433,433,433,     1275,1276,1439,1440, 1019,1017,1222,1221,}; // snow/ice
// TODO ice cave

const int room_tiles_tundra1 [ROOM_TILESET_SIZE] =      {432,608,609,-1, 513,512,1438,1273,  1509,602,604,614,    433,433,433,433,     956,957,1275,1276,   1438,944,1222,1221,}; // snow/trees

const int room_tiles_taiga1 [ROOM_TILESET_SIZE] =       {432,-1,608,609, 1438,1509,512,1273, 1510,615,520,1438,   608,608,608,608,     552,553,954,955,     1438,944,1222,1221,}; // trees/snow

const int room_tiles_plains1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 610,615,1382,1381,   615,616,602,606,     -1,-1,-1,-1,         1439,1440,1965,1966, 1510,603,602,1510,}; // bushes
const int room_tiles_plains2 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 610,615,1382,1381,   441,440,437,615,     -1,-1,-1,-1,         1439,1440,441,1956,  1956,440,441,1956,}; // water

const int room_tiles_forest1 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 1511,1510,1509,950,  615,1273,1438,520,   -1,-1,-1,-1,         550,551,956,957,     1438,944,602,944,}; // generic
const int room_tiles_forest2 [ROOM_TILESET_SIZE] =      {-1,-1,432,608, 1511,1510,1509,950,  1273,872,604,1262,   -1,-1,-1,-1,         954,955,1275,1276,   1509,1273,1222,1221,}; // rocks
// TODO camp?

const int room_tiles_deep_forest1 [ROOM_TILESET_SIZE] = {-1,-1,432,608, 1512,1511,1510,1509, 615,605,604,610,     -1,-1,-1,-1,         954,955,956,957,     603,944,602,944,}; // generic
const int room_tiles_deep_forest2 [ROOM_TILESET_SIZE] = {-1,432,608,609, 1512,1512,1512,1511, 1510,1509,605,1273, -1,-1,-1,-1,         954,955,956,957,     603,944,602,944,}; // thick walls
const int room_tiles_deep_forest3 [ROOM_TILESET_SIZE] = {-1,-1,432,608, 1512,1511,1510,1509, 441,440,437,615,     -1,-1,-1,-1,         441,1956,956,957,    603,944,1956,440,}; // water

const int room_tiles_desert1 [ROOM_TILESET_SIZE] =      {-1,432,608,225, 1923,1921,1922,1920, 1917,1918,1273,1017,  -1,-1,-1,-1,       1935,1936,1275,1276, 1927,1924,1222,1221,}; // cactus
const int room_tiles_desert2 [ROOM_TILESET_SIZE] =      {-1,-1,225,432,  506,505,507,439,     1211,1213,1213,1920,  -1,-1,-1,-1,       1935,1936,1275,1276, 1927,1924,1222,1221,}; // sand
// TODO cave
// TODO oasis

const int room_tiles_savannah1 [ROOM_TILESET_SIZE] =    {-1,-1,432,608,  610,611,1970,621,    605,606,602,603,    -1,-1,-1,-1,         1939,1940,599,601,   1510,610,1927,1924,}; // bushes
const int room_tiles_savannah2 [ROOM_TILESET_SIZE] =    {-1,-1,432,608,  610,611,1970,621,    441,440,437,615,    -1,-1,-1,-1,         599,601,441,1956,    1510,610,1927,1924,}; // water

const int room_tiles_jungle1 [ROOM_TILESET_SIZE] =      {-1,432,609,608, 1512,1512,1510,1973, 605,607,441,440,    -1,-1,-1,-1,         599,601,1951,1952,   1688,1688,603,944,};

extern const int *room_tilesets[72];

int setup_room(byte wall_map[MAP_W][MAP_H], int tile_map[MAP_W][MAP_H], int world_x, int world_y, int world_tile_data[15], int area_x, int area_y, byte exits, unsigned int seed);
int get_room_tileset_index(int world_tile_data[15], int world_x, int world_y, int area_x, int area_y, unsigned int seed);
int get_room_tile_index(int tile_x, int tile_y, byte tile_type, unsigned int seed);
bool room_tile_walkable(byte wall_map[MAP_W][MAP_H], int tile_x, int tile_y);

int clear_djikstra_map(int djikstra_map[MAP_W][MAP_H]);
int get_djikstra_lowest(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y);
int build_djikstra_map(int djikstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H], int goal_x, int goal_y);
int build_djikstra_map(int djikstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H]);
int merge_djikstra_maps(int output_map[MAP_W][MAP_H], int input1_map[MAP_W][MAP_H]);
int get_djikstra_direction(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y, unsigned int seed);
int get_djikstra_direction(int djikstra_map[MAP_W][MAP_H], int tile_x, int tile_y, int target_distance, unsigned int seed);

#endif
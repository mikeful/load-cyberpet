#ifndef MAP_W
#define MAP_W 8
#endif

#ifndef MAP_H
#define MAP_H 14
#endif

#ifndef NOISEMAP_H
#define NOISEMAP_H

#include "Arduino.h"
#include "squirrel_hash.h"
#include "toolbox.h"

#define HEIGHT_DEEP_SEA 0
#define HEIGHT_SEA 1
#define HEIGHT_SHALLOW 2
#define HEIGHT_REGULAR 3
#define HEIGHT_HILL 4
#define HEIGHT_MOUNTAIN 5
#define HEIGHT_PEAK 6

#define BIOME_NONE 0
#define BIOME_ICE_FIELD 1
#define BIOME_TUNDRA 2
#define BIOME_TAIGA 3
#define BIOME_PLAINS 4
#define BIOME_FOREST 5
#define BIOME_DEEP_FOREST 6
#define BIOME_DESERT 7
#define BIOME_SAVANNAH 8
#define BIOME_JUNGLE 9

const byte biome_matrix[9] = {
    BIOME_ICE_FIELD, BIOME_TUNDRA,   BIOME_TAIGA,       // cold
    BIOME_PLAINS,    BIOME_FOREST,   BIOME_DEEP_FOREST, // moderate
    BIOME_DESERT,    BIOME_SAVANNAH, BIOME_JUNGLE       // hot
    // dry,          mid,            wet
};

#define BULDINGS_NONE 0
#define BULDINGS_CAMP 1
#define BULDINGS_HUT 2
#define BULDINGS_TOWN 3
#define BULDINGS_CITY 4

#define TREASURE_NONE 0
#define TREASURE_CAMP 1
#define TREASURE_DUNGEON 2

#define FACTION_NONE 0
#define FACTION_CONFLICT 1
#define FACTION_HUMAN 2
#define FACTION_MONSTER 3
#define FACTION_NATURE 4

#define TILE_LEVEL 0
#define TILE_HEIGHT_VALUE 1
#define TILE_TEMPERATURE 2
#define TILE_LUSHNESS 3
#define TILE_HEIGHT 4
#define TILE_BIOME 5
#define TILE_DENSITY 6
#define TILE_HUMANITY 7
#define TILE_BUILDINGS 8
#define TILE_TREASURE 9
#define TILE_DUNGEON 10
#define TILE_FACTION 11
#define TILE_FACTION_HUMAN 12
#define TILE_FACTION_MONSTER 13
#define TILE_FACTION_NATURE 14

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

// Internally fully connected 3x3 room area, exits to next world tile is added
// at room generation
const byte area_room_exits [] PROGMEM = {
  // Row 1
  EXIT_E | EXIT_S,
  EXIT_E | EXIT_S | EXIT_W,
  EXIT_S | EXIT_W,

  // Row 2
  EXIT_N | EXIT_E | EXIT_S,
  EXIT_N | EXIT_E | EXIT_S | EXIT_W,
  EXIT_N | EXIT_S | EXIT_W,

  // Row 3
  EXIT_N | EXIT_E,
  EXIT_N | EXIT_E | EXIT_W,
  EXIT_N | EXIT_W,
};

float sine_gauss_filter_2d(int width, int x, int y);
float sine_gauss_filter_3d(int width, int x, int y, int z);
int get_cosine_noisepoint(int pos_x, int pos_y, unsigned int seed, float scale);
int get_cosine_noisepoint3d(int pos_x, int pos_y, int pos_z, unsigned int seed, float scale);
int get_simple_noisepoint(int pos_x, int pos_y, unsigned int seed, float scale);
int get_simple_noisepoint3d(int pos_x, int pos_y, int pos_z, unsigned int seed, float scale);
int get_simple_noisemap(byte noisemap2d[MAP_W][MAP_H], int pos_x, int pos_y, int width, int height, unsigned int seed, float scale);
int get_simple_noisemap3d(byte noisemap2d[MAP_W][MAP_H], int pos_x, int pos_y, int pos_z, int width, int height, unsigned int seed, float scale);

bool is_triangle_grid_node(int check_x, int check_y, int grid_size);
bool is_poi_big(int world_x, int world_y, int tile_value, int poi_prob, int poi_grid_size, unsigned int seed);
bool is_poi_small(int world_x, int world_y, int tile_value, int poi_prob, int poi_grid_size);
int get_world_tile(int tile_data[15], int world_x, int world_y, int time_step, unsigned int seed);

unsigned int get_world_draw_tile(int world_tile_data[15]);
byte get_area_exits(int world_x, int world_y);
byte get_room_exits(int area_x, int area_y);

#endif
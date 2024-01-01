#ifndef STAT_STR
#define STAT_STR 0
#define STAT_DEX 1
#define STAT_INT 2
#define STAT_VIT 3
#endif

#ifndef ENTITY_SIZE
#define ENTITY_SIZE 8
#define ENTITY_ATTRS 15

#define ENTITY_ID_PLAYER 0

#define ENTITY_STR 0
#define ENTITY_DEX 1
#define ENTITY_INT 2
#define ENTITY_VIT 3
#define ENTITY_LEVEL 4
#define ENTITY_ALIVE 5
#define ENTITY_HP 6
#define ENTITY_SP 7
#define ENTITY_BLESSED_TICKS 8
#define ENTITY_CURSED_TICKS 9
#define ENTITY_STUNNED_TICKS 10
#define ENTITY_ROOM_X 11
#define ENTITY_ROOM_Y 12
#define ENTITY_ICON 13
#define ENTITY_AI_PROFILE 14
#define ENTITY_AI_STATE 15
#endif

#ifndef AI_PROFILE_NONE
#define AI_PROFILE_NONE 0 // Stands still and no reactions, loot containers/obstacles

#define AI_STATE_START 0
#define AI_STATE_IDLE 1
#define AI_STATE_MELEE 2
#define AI_STATE_RANGED 3
#define AI_STATE_FLEE 4
#endif

#ifndef ENTITIES_H
#define ENTITIES_H

#include "Arduino.h"
#include "squirrel_hash.h"
#include "noisemap.h"
#include "room.h"
#include "items.h"

const float statpoint_growth_player = 1.11;
const float statpoint_growth_enemy = 1.13;
const unsigned int hp_base = 98; // +1 vit + modified main stat = min 100

int setup_player_entity(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS]);
int setup_room_entities(
  unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS],
  byte room_wallmap[MAP_W][MAP_H],
  int room_exit_navmap[MAP_W][MAP_H],
  int room_entity_navmap[MAP_W][MAP_H],
  int world_x, int world_y,
  int world_tile_data[15],
  int area_x, int area_y,
  unsigned int seed
);
int setup_entity(
  unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int entity_x,
  int entity_y,
  int world_tile_data[15],
  unsigned int seed
);
int update_entity_stats(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);
unsigned int get_entity_stat(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, byte stat);
int get_main_stat(unsigned int stat_str, unsigned int stat_dex, unsigned int stat_int);
int get_main_stat(unsigned int stats[4]);
unsigned get_entity_max_hp(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);
unsigned get_entity_max_sp(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);
unsigned int get_max_hp(unsigned int stat_str, unsigned int stat_dex, unsigned int stat_int, unsigned int stat_vit);
unsigned int get_max_sp(unsigned int stat_str, unsigned int stat_dex, unsigned int stat_int, unsigned int stat_vit);

#endif
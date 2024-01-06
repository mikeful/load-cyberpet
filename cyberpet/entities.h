#ifndef STAT_STR
#define STAT_STR 0
#define STAT_DEX 1
#define STAT_INT 2
#define STAT_VIT 3
#endif

#ifndef ENTITY_SIZE
#define ENTITY_SIZE 8
#define ENTITY_ATTRS 26

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
#define ENTITY_AI_DATA1 16 // Generic AI profile/state data fields
#define ENTITY_AI_DATA2 17
#define ENTITY_AI_DATA3 18
#define ENTITY_AI_DATA4 19
#define ENTITY_ITEM1 20
#define ENTITY_ITEM1_TIER 21
#define ENTITY_ITEM1_COOLDOWN 22
#define ENTITY_ITEM2 23
#define ENTITY_ITEM2_TIER 24
#define ENTITY_ITEM2_COOLDOWN 25
#endif

#ifndef AI_PROFILE_NONE
#define AI_PROFILE_NONE 0 // Stands still and no reactions, loot containers/obstacles
#define AI_PROFILE_MELEE 1 // Seek and stay next to player after detection
#define AI_PROFILE_RANGED 2 // Seek and stay close to player after detection, flee if next to player

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
const uint64_t hp_base = 200;

int setup_player_entity(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], unsigned int player_level);
int setup_room_entities(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  byte room_wallmap[MAP_W][MAP_H],
  int room_exit_navmap[MAP_W][MAP_H],
  int room_entity_navmap[MAP_W][MAP_H],
  int room_entity_idmap[MAP_W][MAP_H],
  int world_x, int world_y,
  int world_tile_data[15],
  int area_x, int area_y,
  byte world_tile_dead_bits,
  unsigned int seed
);
int setup_entity(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int entity_x,
  int entity_y,
  int world_tile_data[15],
  bool is_alive,
  unsigned int seed
);

uint64_t get_entity_stat(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, byte stat);
int update_entity_stats(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);
uint64_t modify_entity_hp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, long long change_amount);
uint64_t modify_entity_sp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, long long change_amount);
int process_regen_tick(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);

byte get_main_stat(uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int);
byte get_main_stat(uint64_t stats[4]);
byte get_entity_main_stat(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);
uint64_t get_entity_max_hp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);
uint64_t get_entity_max_sp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id);

uint64_t get_max_hp(uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int, uint64_t stat_vit);
uint64_t get_max_hp(byte main_stat, uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int, uint64_t stat_vit);
int get_hp_gain_regen_tick(byte main_stat);

uint64_t get_max_sp(uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int, uint64_t stat_vit);
uint64_t get_max_sp(byte main_stat, uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int, uint64_t stat_vit);
int get_sp_gain_damage_in(byte main_stat);
int get_sp_gain_damage_out(byte main_stat);
int get_sp_gain_regen_tick(byte main_stat);

int update_ai_state(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int room_entity_idmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
);
int run_ai_state_movement(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int room_entity_idmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
);
int run_ai_state_action(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int room_entity_idmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
);

int resolve_combat(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int attacker_id,
  int defender_id,
  bool can_crit,
  int effect_tilemap[MAP_W][MAP_H],
  unsigned int seed
);

#endif
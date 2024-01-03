#include "entities.h"

int setup_player_entity(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], unsigned int player_level) {
  int entity_id = ENTITY_ID_PLAYER;
  int equipment_id = 20; // 7/20 hp testing
  int equipment_tier = 0;

  entities[entity_id][ENTITY_STR] = get_equip_stat(equipment_id, STAT_STR, equipment_tier);
  entities[entity_id][ENTITY_DEX] = get_equip_stat(equipment_id, STAT_DEX, equipment_tier);
  entities[entity_id][ENTITY_INT] = get_equip_stat(equipment_id, STAT_INT, equipment_tier);
  entities[entity_id][ENTITY_VIT] = get_equip_stat(equipment_id, STAT_VIT, equipment_tier);
  entities[entity_id][ENTITY_LEVEL] = player_level;

  entities[entity_id][ENTITY_ALIVE] = 1;
  entities[entity_id][ENTITY_BLESSED_TICKS] = 0;
  entities[entity_id][ENTITY_CURSED_TICKS] = 0;
  entities[entity_id][ENTITY_STUNNED_TICKS] = 0;

  entities[entity_id][ENTITY_ROOM_X] = 4;
  entities[entity_id][ENTITY_ROOM_Y] = 7;
  entities[entity_id][ENTITY_ICON] = 326;

  entities[entity_id][ENTITY_AI_PROFILE] = AI_PROFILE_NONE;
  entities[entity_id][ENTITY_AI_STATE] = AI_STATE_START;
  entities[entity_id][ENTITY_AI_DATA1] = 0;
  entities[entity_id][ENTITY_AI_DATA2] = 0;
  entities[entity_id][ENTITY_AI_DATA3] = 0;
  entities[entity_id][ENTITY_AI_DATA4] = 0;

  update_entity_stats(entities, 0);

  return 1;
}

int setup_room_entities(
  unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS],
  byte room_wallmap[MAP_W][MAP_H],
  int room_exit_navmap[MAP_W][MAP_H],
  int room_entity_navmap[MAP_W][MAP_H],
  int world_x, int world_y,
  int world_tile_data[15],
  int area_x, int area_y,
  unsigned int seed
) {
  // Setup variables
  int room_seed = squirrel_4d(world_x, world_y, area_x, area_y, seed); // TODO unsigned int?
  int position_tries = 0;
  int entity_x = 0;
  int entity_y = 0;

  for (int entity_id = 1; entity_id < 8; entity_id++) {
    // TODO Check area entity alive bitmask
    
    // Select position for entity
    entity_x = 0;
    entity_y = 0;
    
    while (entity_x == 0 && entity_y == 0) {
      entity_x = 1 + squirrel_2d(entity_id, position_tries, room_seed) % (MAP_W - 2);
      entity_y = 1 + squirrel_2d(entity_id, position_tries + 1, room_seed) % (MAP_H - 2);

      // Check that position is not occupied by other entity and is player accessible
      if (
        room_entity_navmap[entity_x][entity_y] == 0
        || room_exit_navmap[entity_x][entity_y] <= 3
        || room_exit_navmap[entity_x][entity_y] == DJIKSTRA_MAX
      ) {
        entity_x = 0;
        entity_y = 0;
      }

      position_tries++;
    }

    // Mark position occupied and for entity navigation map building
    room_entity_navmap[entity_x][entity_y] = 0;

    setup_entity(entities, entity_id, entity_x, entity_y, world_tile_data, room_seed);
  }
  
  return 1;
}

int setup_entity(
  unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int entity_x,
  int entity_y,
  int world_tile_data[15],
  unsigned int seed
) {
  if (entity_id == ENTITY_ID_PLAYER) { return 0; }

  int equipment_id = 1 + (squirrel(entity_id, seed) % 23); // TODO define max item count

  bool is_elite = false; // +3 levels, gold+loot
  bool is_boss = false; // +5 levels, gold+loot

  entities[entity_id][ENTITY_LEVEL] = (unsigned int)world_tile_data[TILE_LEVEL];
  entities[entity_id][ENTITY_STR] = get_equip_stat(equipment_id, STAT_STR);
  entities[entity_id][ENTITY_DEX] = get_equip_stat(equipment_id, STAT_DEX);
  entities[entity_id][ENTITY_INT] = get_equip_stat(equipment_id, STAT_INT);
  entities[entity_id][ENTITY_VIT] = get_equip_stat(equipment_id, STAT_VIT);

  entities[entity_id][ENTITY_ALIVE] = 1;
  entities[entity_id][ENTITY_BLESSED_TICKS] = 0;
  entities[entity_id][ENTITY_CURSED_TICKS] = 0;
  entities[entity_id][ENTITY_STUNNED_TICKS] = 0;

  entities[entity_id][ENTITY_ROOM_X] = entity_x;
  entities[entity_id][ENTITY_ROOM_Y] = entity_y;
  entities[entity_id][ENTITY_ICON] = 174; //343;

  entities[entity_id][ENTITY_AI_PROFILE] = AI_PROFILE_NONE;
  entities[entity_id][ENTITY_AI_STATE] = AI_STATE_START;
  entities[entity_id][ENTITY_AI_DATA1] = 0;
  entities[entity_id][ENTITY_AI_DATA2] = 0;
  entities[entity_id][ENTITY_AI_DATA3] = 0;
  entities[entity_id][ENTITY_AI_DATA4] = 0;

  update_entity_stats(entities, entity_id);

  return 1;
}

unsigned int get_entity_stat(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, byte stat) {
  float stat_growth = statpoint_growth_enemy;
  if (entity_id == ENTITY_ID_PLAYER) {
    stat_growth = statpoint_growth_player;
  }

  float level = (float)entities[entity_id][ENTITY_LEVEL];
  float stat_value = (float)entities[entity_id][stat];
  return (unsigned int)(stat_value * pow(stat_growth, level));
}

int update_entity_stats(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  unsigned int stat_str = get_entity_stat(entities, entity_id, STAT_STR);
  unsigned int stat_dex = get_entity_stat(entities, entity_id, STAT_DEX);
  unsigned int stat_int = get_entity_stat(entities, entity_id, STAT_INT);
  unsigned int stat_vit = get_entity_stat(entities, entity_id, STAT_VIT);

  entities[entity_id][ENTITY_HP] = get_max_hp(stat_str, stat_dex, stat_int, stat_vit);
  entities[entity_id][ENTITY_SP] = get_max_sp(stat_str, stat_dex, stat_int, stat_vit);

  return 1;
}

int get_main_stat(unsigned int stat_str, unsigned int stat_dex, unsigned int stat_int) {
  unsigned int stats[] = {stat_str, stat_dex, stat_int};
  return get_main_stat(stats);
}

int get_main_stat(unsigned int stats[4]) {
  unsigned int highest_stat_value = -1;
  int highest_stat = -1;

  for (int stat = 0; stat < 3; stat++) {
    if (stats[stat] > highest_stat_value) {
      highest_stat_value = stats[stat];
      highest_stat = stat;
    }
  }

  return highest_stat;
}

unsigned get_entity_max_hp(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  unsigned int stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  unsigned int stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  unsigned int stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  unsigned int stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);

  return get_max_hp(stat_str, stat_dex, stat_int, stat_vit);
}

unsigned get_entity_max_sp(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  unsigned int stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  unsigned int stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  unsigned int stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  unsigned int stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);

  return get_max_sp(stat_str, stat_dex, stat_int, stat_vit);
}

unsigned int get_max_hp(unsigned int stat_str, unsigned int stat_dex, unsigned int stat_int, unsigned int stat_vit) {
  int main_stat = get_main_stat(stat_str, stat_dex, stat_int);

  switch(main_stat) {
    case STAT_STR:
      return hp_base + stat_vit + (stat_str * stat_vit);
    break;
    case STAT_DEX:
      return hp_base + stat_vit + (stat_dex * stat_vit);
    break;
    case STAT_INT:
      return hp_base + stat_vit + (stat_int * stat_vit);
    break;
  }

  return hp_base + (stat_vit * 2);
}

unsigned int get_max_sp(unsigned int stat_str, unsigned int stat_dex, unsigned int stat_int, unsigned int stat_vit) {
  int main_stat = get_main_stat(stat_str, stat_dex, stat_int);

  switch(main_stat) {
    case STAT_STR:
      return (stat_str + stat_vit) * 1;
    break;
    case STAT_DEX:
      return (stat_dex + stat_vit) * 1.67;
    break;
    case STAT_INT:
      return (stat_int + stat_vit) * 5;
    break;
  }

  return stat_vit;
}

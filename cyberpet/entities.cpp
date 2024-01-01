#include "entities.h"

int setup_player_entity(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS]) {
  int entity_id = 0;
  int equipment_id = 20;

  entities[entity_id][ENTITY_STR] = get_equip_stat(equipment_id, STAT_STR);
  entities[entity_id][ENTITY_DEX] = get_equip_stat(equipment_id, STAT_DEX);
  entities[entity_id][ENTITY_INT] = get_equip_stat(equipment_id, STAT_INT);
  entities[entity_id][ENTITY_VIT] = get_equip_stat(equipment_id, STAT_VIT);
  entities[entity_id][ENTITY_LEVEL] = 1;

  entities[entity_id][ENTITY_ALIVE] = 1;
  entities[entity_id][ENTITY_BLESSED_TICKS] = 0;
  entities[entity_id][ENTITY_CURSED_TICKS] = 0;
  entities[entity_id][ENTITY_STUNNED_TICKS] = 0;

  entities[entity_id][ENTITY_ROOM_X] = 4;
  entities[entity_id][ENTITY_ROOM_Y] = 7;
  entities[entity_id][ENTITY_AI_PROFILE] = AI_PROFILE_NONE;
  entities[entity_id][ENTITY_AI_STATE] = AI_STATE_START;

  update_entity_stats(entities, 0);

  return 1;
}

int setup_entity(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, int world_tile_data[15], int world_x, int world_y, int area_x, int area_y, unsigned int seed) {
  int room_seed = squirrel_4d(world_x, world_y, area_x, area_y, seed); // TODO unsigned int?
  int equipment_id = 1 + (squirrel(entity_id, room_seed) % 23); // TODO define max items count

  entities[entity_id][ENTITY_STR] = get_equip_stat(equipment_id, STAT_STR);
  entities[entity_id][ENTITY_DEX] = get_equip_stat(equipment_id, STAT_DEX);
  entities[entity_id][ENTITY_INT] = get_equip_stat(equipment_id, STAT_INT);
  entities[entity_id][ENTITY_VIT] = get_equip_stat(equipment_id, STAT_VIT);
  entities[entity_id][ENTITY_LEVEL] = world_tile_data[TILE_LEVEL];

  entities[entity_id][ENTITY_ALIVE] = 1; // TODO Check area alive bitmask
  entities[entity_id][ENTITY_BLESSED_TICKS] = 0;
  entities[entity_id][ENTITY_CURSED_TICKS] = 0;
  entities[entity_id][ENTITY_STUNNED_TICKS] = 0;

  entities[entity_id][ENTITY_ROOM_X] = 4;
  entities[entity_id][ENTITY_ROOM_Y] = 7;
  entities[entity_id][ENTITY_AI_PROFILE] = AI_PROFILE_NONE;
  entities[entity_id][ENTITY_AI_STATE] = AI_STATE_START;

  update_entity_stats(entities, entity_id);

  return 1;
}

unsigned int get_entity_stat(unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, byte stat) {
  float stat_growth = statpoint_growth_enemy;
  if (entity_id == 0) {
    stat_growth = statpoint_growth_player;
  }

  float level = (float)entities[entity_id][ENTITY_LEVEL];
  return (unsigned int)pow(stat_growth, (float)entities[entity_id][stat] * level);
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

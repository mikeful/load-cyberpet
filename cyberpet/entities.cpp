#include "entities.h"

int setup_player_entity(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], unsigned int player_level) {
  int entity_id = ENTITY_ID_PLAYER;
  int equipment_id = 20; // 7/20 hp testing
  int equipment_tier = 0;

  entities[entity_id][ENTITY_STR] = (uint64_t)get_equip_stat(equipment_id, STAT_STR, equipment_tier);
  entities[entity_id][ENTITY_DEX] = (uint64_t)get_equip_stat(equipment_id, STAT_DEX, equipment_tier);
  entities[entity_id][ENTITY_INT] = (uint64_t)get_equip_stat(equipment_id, STAT_INT, equipment_tier);
  entities[entity_id][ENTITY_VIT] = (uint64_t)get_equip_stat(equipment_id, STAT_VIT, equipment_tier);
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
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
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

  for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
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
        || room_exit_navmap[entity_x][entity_y] == DIJKSTRA_MAX
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
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
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

  entities[entity_id][ENTITY_LEVEL] = (uint64_t)world_tile_data[TILE_LEVEL];
  entities[entity_id][ENTITY_STR] = (uint64_t)get_equip_stat(equipment_id, STAT_STR);
  entities[entity_id][ENTITY_DEX] = (uint64_t)get_equip_stat(equipment_id, STAT_DEX);
  entities[entity_id][ENTITY_INT] = (uint64_t)get_equip_stat(equipment_id, STAT_INT);
  entities[entity_id][ENTITY_VIT] = (uint64_t)get_equip_stat(equipment_id, STAT_VIT);

  entities[entity_id][ENTITY_ALIVE] = 1;
  entities[entity_id][ENTITY_BLESSED_TICKS] = 0;
  entities[entity_id][ENTITY_CURSED_TICKS] = 0;
  entities[entity_id][ENTITY_STUNNED_TICKS] = 0;

  entities[entity_id][ENTITY_ROOM_X] = entity_x;
  entities[entity_id][ENTITY_ROOM_Y] = entity_y;
  entities[entity_id][ENTITY_ICON] = 347; //174

  // TODO Entity faction
  // TODO Entity combat type
  // TODO Entity sprite based on faction, type and biome
  byte main_stat = get_entity_main_stat(entities, entity_id);
  int ai_profile = AI_PROFILE_NONE;
  switch(main_stat) {
    case STAT_STR:
      if (squirrel(entity_id, seed + 409) % 3 == 0) {
        ai_profile = AI_PROFILE_RANGED;
      } else {
        ai_profile = AI_PROFILE_MELEE;
      }
    break;
    case STAT_DEX:
      if (squirrel(entity_id, seed + 409) % 3 == 0) {
        ai_profile = AI_PROFILE_MELEE;
      } else {
        ai_profile = AI_PROFILE_RANGED;
      }
    break;
    case STAT_INT:
      if (squirrel(entity_id, seed + 409) % 4 == 0) {
        ai_profile = AI_PROFILE_MELEE;
      } else {
        ai_profile = AI_PROFILE_RANGED;
      }
    break;
  }

  entities[entity_id][ENTITY_AI_PROFILE] = ai_profile;
  entities[entity_id][ENTITY_AI_STATE] = AI_STATE_START;
  entities[entity_id][ENTITY_AI_DATA1] = 0;
  entities[entity_id][ENTITY_AI_DATA2] = 0;
  entities[entity_id][ENTITY_AI_DATA3] = 0;
  entities[entity_id][ENTITY_AI_DATA4] = 0;

  update_entity_stats(entities, entity_id);

  return 1;
}

uint64_t get_entity_stat(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, byte stat) {
  double stat_growth = statpoint_growth_enemy;
  if (entity_id == ENTITY_ID_PLAYER) {
    stat_growth = statpoint_growth_player;
  }

  double level = (double)entities[entity_id][ENTITY_LEVEL];
  uint64_t stat_value = entities[entity_id][stat];
  return stat_value * (uint64_t)pow(stat_growth, level);
}

int update_entity_stats(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, STAT_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, STAT_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, STAT_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, STAT_VIT);

  entities[entity_id][ENTITY_HP] = get_max_hp(stat_str, stat_dex, stat_int, stat_vit);
  entities[entity_id][ENTITY_SP] = get_max_sp(stat_str, stat_dex, stat_int, stat_vit);

  return 1;
}

byte get_main_stat(uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int) {
  uint64_t stats[] = {stat_str, stat_dex, stat_int};
  return get_main_stat(stats);
}

byte get_main_stat(uint64_t stats[3]) {
  uint64_t highest_stat_value = 0;
  byte highest_stat = 255;

  for (byte stat = 0; stat < 3; stat++) {
    if (stats[stat] > highest_stat_value) {
      highest_stat_value = stats[stat];
      highest_stat = stat;
    }
  }

  return highest_stat;
}

byte get_entity_main_stat(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);

  return get_main_stat(stat_str, stat_dex, stat_int);
}

uint64_t get_entity_max_hp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);

  return get_max_hp(stat_str, stat_dex, stat_int, stat_vit);
}

uint64_t get_entity_max_sp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);

  return get_max_sp(stat_str, stat_dex, stat_int, stat_vit);
}

uint64_t get_max_hp(uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int, uint64_t stat_vit) {
  byte main_stat = get_main_stat(stat_str, stat_dex, stat_int);

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

uint64_t get_max_sp(uint64_t stat_str, uint64_t stat_dex, uint64_t stat_int, uint64_t stat_vit) {
  byte main_stat = get_main_stat(stat_str, stat_dex, stat_int);

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

int update_ai_state(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
) {
  int current_state = (int)entities[entity_id][ENTITY_AI_STATE];
  int next_state = current_state;
  int profile = (int)entities[entity_id][ENTITY_AI_PROFILE];
  byte main_stat = get_entity_main_stat(entities, entity_id);
  int entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
  int entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];

  switch(current_state) {
    case AI_STATE_START:
    // Setup AI variabled based on current profile and entity type
    switch(profile) {
      case AI_PROFILE_MELEE:
        // Preferred range is next to player
        entities[entity_id][ENTITY_AI_DATA1] = 1;
      break;
      case AI_PROFILE_RANGED:
        // Preferred range is 2-3 tiles to player
        entities[entity_id][ENTITY_AI_DATA1] = 2 + (squirrel(entity_id, seed + 409) % 2);
      break;
    }

    next_state = AI_STATE_IDLE;
    break;

    case AI_STATE_IDLE:
      // Stand around or patrol, try to detect player in range
      if (room_player_navmap[entity_x][entity_y] <= 4) {
        if (profile == AI_PROFILE_MELEE) { next_state = AI_STATE_MELEE; }
        else if (profile == AI_PROFILE_RANGED) { next_state = AI_STATE_RANGED; }
      }

      // TODO Check if friendly combat entity in range
    
    break;
    
    case AI_STATE_MELEE:
      // Try to move next to player for attacks, flee on low health/skill points
      if (entities[entity_id][ENTITY_HP] < (get_entity_max_hp(entities, entity_id) / 5)) {
        if (squirrel(entity_id, seed + 419) % 2) {
          next_state = AI_STATE_FLEE;
        }
      } else if (entities[entity_id][ENTITY_SP] < (get_entity_max_sp(entities, entity_id) / 10)) {
        if (squirrel(entity_id, seed + 419) % 2) {
          next_state = AI_STATE_FLEE;
        }
      }

    break;
    
    case AI_STATE_RANGED:
      // Try to move next to player for attacks, flee on low health/skill points
      if (entities[entity_id][ENTITY_HP] < (get_entity_max_sp(entities, entity_id) / 3)) {
        if (squirrel(entity_id, seed + 429) % 2) {
          next_state = AI_STATE_FLEE;
        }
      } else if (entities[entity_id][ENTITY_SP] < (get_entity_max_sp(entities, entity_id) / 8)) {
        if (squirrel(entity_id, seed + 429) % 2) {
          next_state = AI_STATE_FLEE;
        }
      }

    break;
    
    case AI_STATE_FLEE:
      // Move away from player, go back to combat after recovery
      if (
        profile == AI_PROFILE_MELEE
        && squirrel(entity_id, seed + 439) % 2
        && entities[entity_id][ENTITY_HP] > (get_entity_max_sp(entities, entity_id) / 3)
        && entities[entity_id][ENTITY_SP] > (get_entity_max_sp(entities, entity_id) / 4)
      ) {
        next_state = AI_STATE_MELEE;
      } else if (
        profile == AI_PROFILE_RANGED
        && squirrel(entity_id, seed + 439) % 2
        && entities[entity_id][ENTITY_HP] > (get_entity_max_sp(entities, entity_id) / 2)
        && entities[entity_id][ENTITY_SP] > (get_entity_max_sp(entities, entity_id) / 3)
      ) {
        next_state = AI_STATE_RANGED;
      }

    break;
  }

  if (next_state != current_state) {
    entities[entity_id][ENTITY_AI_STATE] = next_state;
    return 1;
  }

  return 0;
}

int run_ai_state_movement(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
) {
  if (entities[entity_id][ENTITY_ALIVE] == 0) { return 1; }

  int current_state = (int)entities[entity_id][ENTITY_AI_STATE];
  int profile = (int)entities[entity_id][ENTITY_AI_PROFILE];
  byte main_stat = get_entity_main_stat(entities, entity_id);
  int entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
  int entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];
  int prev_entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
  int prev_entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];

  int tile_value = 0;
  int target_distance = 1;
  int ai_room_dir = 0;

  switch(current_state) {
    case AI_STATE_START:
    // Skip AI in setup state
    break;

    case AI_STATE_IDLE:
      // Stand around or patrol, try to detect player in range
      if (squirrel(entity_id, seed + 831) % 5 == 0) {
        ai_room_dir = 1 + (squirrel(entity_id, seed + 386) % 4);
        if (ai_room_dir == 1) { entity_y--; }
        else if (ai_room_dir == 2) { entity_y++; }
        else if (ai_room_dir == 3) { entity_x--; }
        else if (ai_room_dir == 4) { entity_x++; }

        // Check that target tile is not occupied
        tile_value = get_dijkstra_value(room_entity_navmap, entity_x, entity_y);
        if (tile_value != DIJKSTRA_MAX && tile_value != 0) {
          // Partially update entity nav map to allow other entities check for occupancy
          room_entity_navmap[prev_entity_x][prev_entity_y] = 1;
          room_entity_navmap[entity_x][entity_y] = 0;
          
          // Set updated entity coordinates
          entities[entity_id][ENTITY_ROOM_X] = entity_x;
          entities[entity_id][ENTITY_ROOM_Y] = entity_y;
        }
      }

    break;
    
    case AI_STATE_MELEE:
    case AI_STATE_RANGED:
      // Try to move next to player for attacks
      if (squirrel(entity_id, seed + 831) % 4 != 0) {
        target_distance = (int)entities[entity_id][ENTITY_AI_DATA1];
        ai_room_dir = get_dijkstra_direction(room_player_navmap, entity_x, entity_y, target_distance, seed);

        if (ai_room_dir == DIR_N) { entity_y--; }
        else if (ai_room_dir == DIR_S) { entity_y++; }
        else if (ai_room_dir == DIR_W) { entity_x--; }
        else if (ai_room_dir == DIR_E) { entity_x++; }
      }

      // Check that target tile is not occupied
      if (ai_room_dir > 0 && room_entity_navmap[entity_x][entity_y] != 0) {
        // Partially update entity nav map to allow other entities check for occupancy
        room_entity_navmap[prev_entity_x][prev_entity_y] = 1;
        room_entity_navmap[entity_x][entity_y] = 0;
        
        // Set updated entity coordinates
        entities[entity_id][ENTITY_ROOM_X] = entity_x;
        entities[entity_id][ENTITY_ROOM_Y] = entity_y;
      }

    break;
    
    case AI_STATE_FLEE:
      // Move away from player
      ai_room_dir = get_dijkstra_direction(room_player_navmap, entity_x, entity_y, 7, seed);

      if (ai_room_dir == DIR_N) { entity_y--; }
      else if (ai_room_dir == DIR_S) { entity_y++; }
      else if (ai_room_dir == DIR_W) { entity_x--; }
      else if (ai_room_dir == DIR_E) { entity_x++; }

      // Check that target tile is not occupied
      if (ai_room_dir > 0 && room_entity_navmap[entity_x][entity_y] != 0) {
        // Partially update entity nav map to allow other entities check for occupancy
        room_entity_navmap[prev_entity_x][prev_entity_y] = 1;
        room_entity_navmap[entity_x][entity_y] = 0;
        
        // Set updated entity coordinates
        entities[entity_id][ENTITY_ROOM_X] = entity_x;
        entities[entity_id][ENTITY_ROOM_Y] = entity_y;
      }

    break;
  }

  return 1;
}

int run_ai_state_action(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
) {
  if (entities[entity_id][ENTITY_ALIVE] == 0) { return 1; }

  int current_state = (int)entities[entity_id][ENTITY_AI_STATE];
  int profile = (int)entities[entity_id][ENTITY_AI_PROFILE];
  byte main_stat = get_entity_main_stat(entities, entity_id);

  switch(current_state) {
    case AI_STATE_START:
    // Skip AI in setup state
    break;

    case AI_STATE_IDLE:
    // Stand around or patrol, try to detect player in range

    break;
    
    case AI_STATE_MELEE:
    // Try to move next to player for attacks
    break;
    
    case AI_STATE_RANGED:
    // Try to move next to player for attacks
    break;
    
    case AI_STATE_FLEE:
    break;
  }

  return 1;
}

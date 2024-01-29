#include "entities.h"

int setup_player_entity(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], unsigned int player_level) {
  int entity_id = ENTITY_ID_PLAYER;
  int equipment_id = 7; // 7/21 hp testing
  int equipment_tier = 0;

  entities[entity_id][ENTITY_ITEM1] = (uint64_t)equipment_id;
  entities[entity_id][ENTITY_ITEM1_TIER] = (uint64_t)equipment_tier;
  entities[entity_id][ENTITY_ITEM1_COOLDOWN] = 0;
  entities[entity_id][ENTITY_STR] = (uint64_t)get_equip_stat(equipment_id, STAT_STR, equipment_tier);
  entities[entity_id][ENTITY_DEX] = (uint64_t)get_equip_stat(equipment_id, STAT_DEX, equipment_tier);
  entities[entity_id][ENTITY_INT] = (uint64_t)get_equip_stat(equipment_id, STAT_INT, equipment_tier);
  entities[entity_id][ENTITY_VIT] = (uint64_t)get_equip_stat(equipment_id, STAT_VIT, equipment_tier);

  equipment_id = 2;
  equipment_tier = 0;

  entities[entity_id][ENTITY_ITEM2] = (uint64_t)equipment_id;
  entities[entity_id][ENTITY_ITEM2_TIER] = (uint64_t)equipment_tier;
  entities[entity_id][ENTITY_ITEM2_COOLDOWN] = 0;
  entities[entity_id][ENTITY_STR] += (uint64_t)get_equip_stat(equipment_id, STAT_STR, equipment_tier);
  entities[entity_id][ENTITY_DEX] += (uint64_t)get_equip_stat(equipment_id, STAT_DEX, equipment_tier);
  entities[entity_id][ENTITY_INT] += (uint64_t)get_equip_stat(equipment_id, STAT_INT, equipment_tier);
  entities[entity_id][ENTITY_VIT] += (uint64_t)get_equip_stat(equipment_id, STAT_VIT, equipment_tier);

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

  update_entity_stats(entities, entity_id);

  return 1;
}

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
) {
  // Setup variables
  int room_seed = squirrel_4d(world_x, world_y, area_x, area_y, seed); // TODO unsigned int?
  int position_tries = 0;
  int entity_x = 0;
  int entity_y = 0;
  int entity_index = 0;
  bool is_alive = false;
  int setup_result = 0;
  int target_distance = 4;

  for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
    // Select position for entity
    position_tries = 0;
    target_distance = 4;
    entity_x = 0;
    entity_y = 0;
    
    while (entity_x == 0 && entity_y == 0) {
      entity_x = 1 + (squirrel_2d(entity_id, position_tries, room_seed) % (MAP_W - 2));
      entity_y = 1 + (squirrel_2d(entity_id, position_tries + 1, room_seed) % (MAP_H - 2));

      // Check that position is not occupied by other entity and is player accessible
      if (
        room_entity_navmap[entity_x][entity_y] == 0
        || room_exit_navmap[entity_x][entity_y] <= target_distance
        || room_exit_navmap[entity_x][entity_y] == DIJKSTRA_MAX
      ) {
        entity_x = 0;
        entity_y = 0;
      }

      position_tries += 2;
      if (position_tries % 20 == 0) {
        target_distance--;
      }
    }

    // Check area entity dead bitmask
    entity_index = (byte)1 << (byte)entity_id;
    is_alive = !(bool)(world_tile_dead_bits & entity_index);

    setup_result = setup_entity(entities, entity_id, entity_x, entity_y, world_tile_data, is_alive, room_seed);

    if (setup_result) {
      // Mark position occupied and for entity navigation map building
      room_entity_navmap[entity_x][entity_y] = 0;
      room_entity_idmap[entity_x][entity_y] = entity_id;
    }

  }
  
  return 1;
}

int setup_entity(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int entity_x,
  int entity_y,
  int world_tile_data[15],
  bool is_alive,
  unsigned int seed
) {
  if (entity_id == ENTITY_ID_PLAYER) { return 0; }

  int equipment_id = 1 + (squirrel(entity_id, seed) % 23); // TODO define max item count
  int equipment_tier = 0;

  bool is_elite = false; // TODO +3 levels/tier, gold+loot
  bool is_boss = false; // TODO +5 levels/tier, gold+loot

  entities[entity_id][ENTITY_ITEM1] = (uint64_t)equipment_id;
  entities[entity_id][ENTITY_ITEM1_TIER] = (uint64_t)equipment_tier;
  entities[entity_id][ENTITY_ITEM1_COOLDOWN] = 0;

  entities[entity_id][ENTITY_STR] = (uint64_t)get_equip_stat(equipment_id, STAT_STR);
  entities[entity_id][ENTITY_DEX] = (uint64_t)get_equip_stat(equipment_id, STAT_DEX);
  entities[entity_id][ENTITY_INT] = (uint64_t)get_equip_stat(equipment_id, STAT_INT);
  entities[entity_id][ENTITY_VIT] = (uint64_t)get_equip_stat(equipment_id, STAT_VIT);

  entities[entity_id][ENTITY_ITEM2] = 0;
  entities[entity_id][ENTITY_ITEM2_TIER] = 0;
  entities[entity_id][ENTITY_ITEM2_COOLDOWN] = 0;

  entities[entity_id][ENTITY_LEVEL] = (uint64_t)world_tile_data[TILE_LEVEL];

  entities[entity_id][ENTITY_ALIVE] = (uint64_t)is_alive;
  entities[entity_id][ENTITY_BLESSED_TICKS] = 0;
  entities[entity_id][ENTITY_CURSED_TICKS] = 0;
  entities[entity_id][ENTITY_STUNNED_TICKS] = 0;

  entities[entity_id][ENTITY_ROOM_X] = entity_x;
  entities[entity_id][ENTITY_ROOM_Y] = entity_y;

  // Entity sprite based on faction
  // TODO Also based on type and biome
  int icon_variation = squirrel(entity_id, seed + 338) % 4;
  int icon_faction = world_tile_data[TILE_FACTION] - 2;
  if (world_tile_data[TILE_FACTION] <= FACTION_CONFLICT) {
    // World tile in conflict, select random faction
    icon_faction = squirrel(entity_id, seed + 392) % 3;
  }
  entities[entity_id][ENTITY_ICON] = entity_faction_icon[icon_faction][icon_variation];

  // Entity combat type
  byte weapon_type = equipments[equipment_id][EQUIP_TYPE];
  int ai_profile = AI_PROFILE_NONE;
  switch(weapon_type) {
    case EQUIP_TYPE_UNARMED:
    case EQUIP_TYPE_WEAPON:
      ai_profile = AI_PROFILE_MELEE;
    break;
    case EQUIP_TYPE_RANGED:
      ai_profile = AI_PROFILE_RANGED;
    break;
    case EQUIP_TYPE_MAGIC:
      if (squirrel(entity_id, seed + 409) % 3 == 0) {
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

  entities[entity_id][ENTITY_HP] = get_entity_max_hp(entities, entity_id);
  entities[entity_id][ENTITY_SP] = get_entity_max_sp(entities, entity_id);

  return 1;
}

// TODO Support long long and uint64_t
uint64_t modify_entity_hp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, long long change_amount) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, STAT_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, STAT_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, STAT_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, STAT_VIT);

  uint64_t max_hp = get_entity_max_hp(entities, entity_id);
  uint64_t current_hp = entities[entity_id][ENTITY_HP];

  if (change_amount > 0) {
    if (current_hp + change_amount > max_hp) {
      entities[entity_id][ENTITY_HP] = max_hp;
    } else {
      entities[entity_id][ENTITY_HP] = current_hp + change_amount;
    }
  } else if (change_amount < 0) {
    if (current_hp < abs(change_amount)) {
      entities[entity_id][ENTITY_HP] = 0;
      entities[entity_id][ENTITY_ALIVE] = 0;
    } else {
      entities[entity_id][ENTITY_HP] = current_hp + change_amount;
    }
  }

  return entities[entity_id][ENTITY_HP];
}

// TODO Support long long and uint64_t
uint64_t modify_entity_sp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id, long long change_amount) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, STAT_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, STAT_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, STAT_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, STAT_VIT);

  uint64_t max_sp = get_entity_max_sp(entities, entity_id);
  uint64_t current_sp = entities[entity_id][ENTITY_SP];

  if (change_amount > 0) {
    if (current_sp + change_amount > max_sp) {
      entities[entity_id][ENTITY_SP] = max_sp;
    } else {
      entities[entity_id][ENTITY_SP] = current_sp + change_amount;
    }
  } else if (change_amount < 0) {
    if (current_sp < abs(change_amount)) {
      entities[entity_id][ENTITY_SP] = 0;
    } else {
      entities[entity_id][ENTITY_SP] = current_sp + change_amount;
    }
  }

  return entities[entity_id][ENTITY_SP];
}

int process_regen_tick(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  if (entities[entity_id][ENTITY_ALIVE] != 1) { return 0; }

  uint64_t stat_str = get_entity_stat(entities, entity_id, STAT_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, STAT_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, STAT_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, STAT_VIT);
  byte main_stat = get_main_stat(stat_str, stat_dex, stat_int);

  uint64_t max_hp = get_entity_max_hp(entities, entity_id);
  if (entities[entity_id][ENTITY_HP] < max_hp) {
    long long hp_gain_regen = (long long)get_hp_gain_regen_tick(main_stat);
    long long hp_gain = max((long long)1, (long long)(max_hp / 50) * hp_gain_regen) ;
    modify_entity_hp(entities, entity_id, hp_gain);
  }

  uint64_t max_sp = get_entity_max_sp(entities, entity_id);
  if (entities[entity_id][ENTITY_SP] < max_sp) {
    long long sp_gain_regen = (long long)get_sp_gain_regen_tick(main_stat);
    long long sp_gain = max((long long)1, (long long)(max_sp / 50) * sp_gain_regen);
    modify_entity_sp(entities, entity_id, sp_gain);
  }

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

uint64_t get_entity_max_sp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);
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

uint64_t get_entity_max_hp(uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS], int entity_id) {
  uint64_t stat_str = get_entity_stat(entities, entity_id, ENTITY_STR);
  uint64_t stat_dex = get_entity_stat(entities, entity_id, ENTITY_DEX);
  uint64_t stat_int = get_entity_stat(entities, entity_id, ENTITY_INT);
  uint64_t stat_vit = get_entity_stat(entities, entity_id, ENTITY_VIT);
  byte main_stat = get_main_stat(stat_str, stat_dex, stat_int);

  unsigned int entity_level = (unsigned int)entities[entity_id][ENTITY_LEVEL];

  uint64_t mod_hp = hp_base + (entity_id == ENTITY_ID_PLAYER ? hp_player_bonus : 0);

  switch(main_stat) {
    case STAT_STR:
      return mod_hp + entity_level + (stat_str * stat_vit);
    break;
    case STAT_DEX:
      return mod_hp + entity_level + (stat_dex * stat_vit);
    break;
    case STAT_INT:
      return mod_hp + entity_level + (stat_int * stat_vit);
    break;
  }

  return mod_hp + entity_level + (stat_vit * 2);
}

int get_hp_gain_regen_tick(byte main_stat) {
  switch(main_stat) {
    case STAT_STR:
      return 1;
    break;
    case STAT_DEX:
      return 3;
    break;
    case STAT_INT:
      return 2;
    break;
  }

  return 2;
}

int get_sp_gain_damage_in(byte main_stat) {
  if (main_stat == STAT_STR) { return 3; }
  return 0;
}

int get_sp_gain_damage_out(byte main_stat) {
  if (main_stat == STAT_STR) { return 3; }
  else if (main_stat == STAT_DEX) { return 1; }
  return 0;
}

int get_sp_gain_regen_tick(byte main_stat) {
  switch(main_stat) {
    case STAT_STR:
      return -1;
    break;
    case STAT_DEX:
      return 2;
    break;
    case STAT_INT:
      return 1;
    break;
  }

  return 2;
}

uint64_t get_armor_rating(byte main_stat) {
  switch(main_stat) {
    case STAT_STR:
      return 3;
    break;
    case STAT_DEX:
      return 2;
    break;
    case STAT_INT:
      return 1;
    break;
  }

  return 2;
}

int update_ai_state(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int room_entity_idmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
) {
  int current_state = (int)entities[entity_id][ENTITY_AI_STATE];
  int next_state = current_state;
  int profile = (int)entities[entity_id][ENTITY_AI_PROFILE];

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
      if (room_player_navmap[entity_x][entity_y] <= 3) {
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
  int room_entity_idmap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
) {
  if (entities[entity_id][ENTITY_ALIVE] == 0) { return 1; }

  int current_state = (int)entities[entity_id][ENTITY_AI_STATE];
  int profile = (int)entities[entity_id][ENTITY_AI_PROFILE];

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

    case AI_STATE_RANGED:
      // Try to move next to player for attacks
      if (squirrel(entity_id, seed + 831) % 5 != 0) {
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
      if (squirrel(entity_id, seed + 831) % 4 != 0) {
        ai_room_dir = get_dijkstra_direction(room_player_navmap, entity_x, entity_y, 4, seed);

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
  }

  return 1;
}

int run_ai_state_action(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int entity_id,
  int room_entity_navmap[MAP_W][MAP_H],
  int room_player_navmap[MAP_W][MAP_H],
  int room_entity_idmap[MAP_W][MAP_H],
  int room_effect_tilemap[MAP_W][MAP_H],
  int world_tile_data[15],
  unsigned int seed
) {
  if (entities[entity_id][ENTITY_ALIVE] == 0) { return 1; }

  int current_state = (int)entities[entity_id][ENTITY_AI_STATE];
  int profile = (int)entities[entity_id][ENTITY_AI_PROFILE];

  int player_x = entities[ENTITY_ID_PLAYER][ENTITY_ROOM_X];
  int player_y = entities[ENTITY_ID_PLAYER][ENTITY_ROOM_Y];
  int check_entity_x = entities[entity_id][ENTITY_ROOM_X];
  int check_entity_y = entities[entity_id][ENTITY_ROOM_Y];
  int player_distance = room_player_navmap[check_entity_x][check_entity_y];

  switch(current_state) {
    case AI_STATE_START:
    // Skip AI in setup state
    break;

    case AI_STATE_IDLE:
    // Stand around or patrol, try to detect player in range
    break;
    
    case AI_STATE_MELEE:
      // Try to move next to player for attacks
      if (player_distance == 1) {
        resolve_combat(entities, entity_id, ENTITY_ID_PLAYER, false, room_effect_tilemap, seed);
      }
    break;
    
    case AI_STATE_RANGED:
      // Try to move next to player for attacks
      if (player_distance == 2) {
        resolve_combat(entities, entity_id, ENTITY_ID_PLAYER, false, room_effect_tilemap, seed);
      }
    break;
    
    case AI_STATE_FLEE:
    // Move away from player
    // TODO Use health items/spells?
    break;
  }

  return 1;
}

int resolve_combat(
  uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS],
  int attacker_id,
  int defender_id,
  bool can_crit,
  int effect_tilemap[MAP_W][MAP_H],
  unsigned int seed
) {
  uint64_t damage = 0;
  uint64_t defense = 0;
  uint64_t damage_die = 0;
  bool was_crit = false;
  long long sp_change = 0;
  int effect_tile = -1;

  // Attacker combat data
  int attacker_x = entities[attacker_id][ENTITY_ROOM_X];
  int attacker_y = entities[attacker_id][ENTITY_ROOM_Y];
  unsigned int attacker_level = (unsigned int)entities[attacker_id][ENTITY_LEVEL];
  uint64_t attacker_str = get_entity_stat(entities, attacker_id, ENTITY_STR);
  uint64_t attacker_dex = get_entity_stat(entities, attacker_id, ENTITY_DEX);
  uint64_t attacker_int = get_entity_stat(entities, attacker_id, ENTITY_INT);
  uint64_t attacker_vit = get_entity_stat(entities, attacker_id, ENTITY_VIT);
  byte attacker_main_stat = get_main_stat(attacker_str, attacker_dex, attacker_int);
  int attacker_weapon = (int)entities[attacker_id][ENTITY_ITEM1];
  byte attacker_weapon_type = equipments[attacker_weapon][EQUIP_TYPE];
  
  int attacker_crit_chance = get_attack_count(attacker_weapon_type);
  uint64_t attacker_base_damage = (uint64_t)get_attack_base_damage(attacker_weapon_type);
  uint64_t attacker_stat_damage = get_attack_damage_stat(attacker_weapon_type, attacker_str, attacker_dex, attacker_int);
  double attacker_crit_multiplier = 12.0 / (double)attacker_base_damage;
  long long attacker_special_cost = get_attack_cost(attacker_weapon_type);

  long long attacker_sp_gain_damage_out = (long long)get_sp_gain_damage_out(attacker_main_stat);
  uint64_t attacker_max_sp = get_entity_max_sp(entities, attacker_id);
  long long attacker_max_sp_fraction = max((long long)1, (long long)(attacker_max_sp / 10));
  long long attacker_crit_sp_cost = attacker_max_sp_fraction * attacker_special_cost;

  // Defender combat data
  int defender_x = entities[defender_id][ENTITY_ROOM_X];
  int defender_y = entities[defender_id][ENTITY_ROOM_Y];
  unsigned int defender_level = (unsigned int)entities[defender_id][ENTITY_LEVEL];
  uint64_t defender_str = get_entity_stat(entities, defender_id, ENTITY_STR);
  uint64_t defender_dex = get_entity_stat(entities, defender_id, ENTITY_DEX);
  uint64_t defender_int = get_entity_stat(entities, defender_id, ENTITY_INT);
  uint64_t defender_vit = get_entity_stat(entities, defender_id, ENTITY_VIT);
  byte defender_main_stat = get_main_stat(defender_str, defender_dex, defender_int);

  uint64_t defender_armor_rating = (uint64_t)get_armor_rating(defender_main_stat);
  long long defender_sp_gain_damage_in = (long long)get_sp_gain_damage_in(defender_main_stat);
  uint64_t defender_max_sp = get_entity_max_sp(entities, defender_id);
  long long defender_max_sp_fraction = max((long long)1, (long long)(defender_max_sp / 10));

  // Attacker attacks
  damage_die = attacker_base_damage; // Normal damage
  if (
    can_crit
    && entities[attacker_id][ENTITY_SP] > attacker_crit_sp_cost
    && squirrel_2d(defender_id, attacker_id, seed) % 10 < attacker_crit_chance
  ) {
    // Critical damage, chance 10-30%
    was_crit = true;
    damage_die = 12;
    sp_change = sp_change - attacker_crit_sp_cost;
  }
  sp_change = sp_change + (attacker_sp_gain_damage_out * attacker_max_sp_fraction);
  modify_entity_sp(entities, attacker_id, sp_change);

  // Defender damage mitigation
  damage = (1 + (squirrel_2d(defender_id, attacker_id, seed + 53) % damage_die)) * (uint64_t)(attacker_stat_damage * attacker_crit_multiplier);
  defense = defender_vit * defender_armor_rating;
  if (damage > defense) {
    damage = damage - defense;
    effect_tile = get_attack_effect(attacker_weapon_type, was_crit ? EQUIP_EFFECT_CRIT : EQUIP_EFFECT_HIT, seed + 643);
  } else {
    damage = 0;
    effect_tile = get_attack_effect(attacker_weapon_type, EQUIP_EFFECT_MISS, seed + 291);
  }

  effect_tilemap[defender_x][defender_y] = effect_tile;

  //Serial.println(String(attacker_id) + "->" + String(defender_id) + " Attacker hits " + String(damage) + "/" + String(entities[defender_id][ENTITY_HP]) + ", SP " + String(sp_change));

  if (modify_entity_hp(entities, defender_id, -(long long)damage) == 0) {
    // Defender died from attack
    //Serial.println(String(attacker_id) + "->" + String(defender_id) + " Combat end, defender died");
    return 1;
  } else {
    sp_change = defender_sp_gain_damage_in * defender_max_sp_fraction;
    modify_entity_sp(entities, defender_id, sp_change);
  }

  return 0;
}

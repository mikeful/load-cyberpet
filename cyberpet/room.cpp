#include "room.h"

const int *room_tilesets[72] = {
  room_tiles_dungeon1, // ROOM_TILES_DUNGEON
  room_tiles_dungeon2,
  room_tiles_dungeon2,
  room_tiles_dungeon1,
  room_tiles_sea_town1, // ROOM_TILES_SEA_TOWN
  room_tiles_sea_town2,
  room_tiles_sea_town3,
  room_tiles_sea_town4,
  room_tiles_town1, // ROOM_TILES_TOWN
  room_tiles_town1,
  room_tiles_town2,
  room_tiles_town3,
  room_tiles_sea_city1, // ROOM_TILES_SEA_CITY
  room_tiles_sea_city2,
  room_tiles_sea_city3,
  room_tiles_sea_city4,
  room_tiles_city1, // ROOM_TILES_CITY
  room_tiles_city2,
  room_tiles_city3,
  room_tiles_city4,
  room_tiles_deep_sea1, // ROOM_TILES_DEEP_SEA
  room_tiles_deep_sea1,
  room_tiles_deep_sea1,
  room_tiles_deep_sea1,
  room_tiles_sea1, // ROOM_TILES_SEA
  room_tiles_sea1,
  room_tiles_sea1,
  room_tiles_sea1,
  room_tiles_mountain1, // ROOM_TILES_MOUNTAIN
  room_tiles_mountain1,
  room_tiles_mountain1,
  room_tiles_mountain1,
  room_tiles_peak1, // ROOM_TILES_PEAK
  room_tiles_peak1,
  room_tiles_peak1,
  room_tiles_peak1,
  room_tiles_ice_field1, // ROOM_TILES_ICE_FIELD
  room_tiles_ice_field1,
  room_tiles_ice_field1,
  room_tiles_ice_field1,
  room_tiles_tundra1, // ROOM_TILES_TUNDRA
  room_tiles_tundra1,
  room_tiles_tundra1,
  room_tiles_tundra1,
  room_tiles_taiga1, // ROOM_TILES_TAIGA
  room_tiles_taiga1,
  room_tiles_taiga1,
  room_tiles_taiga1,
  room_tiles_plains1, // ROOM_TILES_PLAINS
  room_tiles_plains2,
  room_tiles_plains2,
  room_tiles_plains1,
  room_tiles_forest1, // ROOM_TILES_FOREST
  room_tiles_forest2,
  room_tiles_forest2,
  room_tiles_forest1,
  room_tiles_deep_forest1, // ROOM_TILES_DEEP_FOREST
  room_tiles_deep_forest2,
  room_tiles_deep_forest3,
  room_tiles_deep_forest1,
  room_tiles_desert1, // ROOM_TILES_DESERT
  room_tiles_desert2,
  room_tiles_desert2,
  room_tiles_desert1,
  room_tiles_savannah1, // ROOM_TILES_SAVANNAH
  room_tiles_savannah2,
  room_tiles_savannah2,
  room_tiles_savannah1,
  room_tiles_jungle1, // ROOM_TILES_JUNGLE
  room_tiles_jungle1,
  room_tiles_jungle1,
  room_tiles_jungle1,
};

int setup_room(byte wall_map[MAP_W][MAP_H], int tile_map[MAP_W][MAP_H], int world_x, int world_y, int world_tile_data[15], int area_x, int area_y, byte exits, unsigned int seed) {
  // Setup variables
  int room_seed = squirrel_4d(world_x, world_y, area_x, area_y, seed);
  bool is_constructed = false;
  byte d6 = 0;
  bool deco_inner_walls = (5 == room_seed % 6);
  int tileset_index = 0;
  int tile_index = 0;
  
  // Flip constructed/handmade status on
  if (world_tile_data[TILE_BUILDINGS] == BULDINGS_TOWN) {
    is_constructed = true;
  } else if (world_tile_data[TILE_BUILDINGS] == BULDINGS_CITY) {
    is_constructed = true;
  } else if (world_tile_data[TILE_DUNGEON] == TREASURE_DUNGEON) {
    is_constructed = true;
  }

  // Flip it off if all inner walls will be replaced with decorative walls
  if (deco_inner_walls) {
    is_constructed = false;
  }

  // Build edge walls, clear tilemap
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      if (i == 0 || j == 0 || i == MAP_W - 1 || j == MAP_H - 1) {
        wall_map[i][j] = ROOM_WALL;
      } else {
        wall_map[i][j] = ROOM_FLOOR;
      }

      tile_map[i][j] = -1;
    }
  }

  // Build room exit doors to edge walls
  if (exits & EXIT_N) { wall_map[4][0] = ROOM_EXIT_N; }
  if (exits & EXIT_S) { wall_map[4][MAP_H - 1] = ROOM_EXIT_S; }
  if (exits & EXIT_W) { wall_map[0][7] = ROOM_EXIT_W; }
  if (exits & EXIT_E) { wall_map[MAP_W - 1][7] = ROOM_EXIT_E; }

  // Set pillars from template selected with tile density value
  d6 = 1 + (squirrel_4d(world_x, world_y, area_x, area_y, room_seed + 129) % 6);
  if (world_tile_data[TILE_DENSITY] == 0) {
    if (d6 <= 2) {
      // One wall
      wall_map[3][6] = ROOM_PILLAR1;
    } else if (d6 <= 5) {
      // Two walls
      wall_map[3][5] = ROOM_PILLAR1;
      wall_map[3][9] = ROOM_PILLAR2;
    }
    // Otherwise no walls
  } else if (world_tile_data[TILE_DENSITY] == 1) {
      if (d6 <= 3) {
      // Two walls
      wall_map[3][5] = ROOM_PILLAR1;
      wall_map[3][9] = ROOM_PILLAR2;
    } else {
      // Three walls
      wall_map[3][3] = ROOM_PILLAR1;
      wall_map[3][6] = ROOM_PILLAR2;
      wall_map[3][9] = ROOM_PILLAR3;
    }
  } else if (world_tile_data[TILE_DENSITY] == 3) {
    if (d6 <= 2) {
      // Six walls
      wall_map[2][3] = ROOM_PILLAR1;
      wall_map[2][6] = ROOM_PILLAR2;
      wall_map[2][10] = ROOM_PILLAR3;

      wall_map[5][3] = ROOM_PILLAR4;
      wall_map[5][6] = ROOM_PILLAR5;
      wall_map[5][10] = ROOM_PILLAR6;
    } else if (d6 <= 5) {
      // Eight walls
      wall_map[2][2] = ROOM_PILLAR1;
      wall_map[2][5] = ROOM_PILLAR2;
      wall_map[2][8] = ROOM_PILLAR3;
      wall_map[2][11] = ROOM_PILLAR4;

      wall_map[5][2] = ROOM_PILLAR5;
      wall_map[5][5] = ROOM_PILLAR6;
      wall_map[5][8] = ROOM_PILLAR7;
      wall_map[5][11] = ROOM_PILLAR8;
    } else {
      // Ten walls
      wall_map[2][2] = ROOM_PILLAR1;
      wall_map[2][4] = ROOM_PILLAR2;
      wall_map[2][6] = ROOM_PILLAR3;
      wall_map[2][9] = ROOM_PILLAR4;
      wall_map[2][11] = ROOM_PILLAR5;

      wall_map[5][2] = ROOM_PILLAR6;
      wall_map[5][4] = ROOM_PILLAR7;
      wall_map[5][6] = ROOM_PILLAR8;
      wall_map[5][9] = ROOM_PILLAR9;
      wall_map[5][11] = ROOM_PILLAR10;
    }

  } else if (world_tile_data[TILE_DENSITY] == 4) {
    // Ten walls
    wall_map[2][2] = ROOM_PILLAR1;
    wall_map[2][4] = ROOM_PILLAR2;
    wall_map[2][6] = ROOM_PILLAR3;
    wall_map[2][9] = ROOM_PILLAR4;
    wall_map[2][11] = ROOM_PILLAR5;

    wall_map[5][2] = ROOM_PILLAR6;
    wall_map[5][4] = ROOM_PILLAR7;
    wall_map[5][6] = ROOM_PILLAR8;
    wall_map[5][9] = ROOM_PILLAR9;
    wall_map[5][11] = ROOM_PILLAR10;
  } else {
    // Default: Four walls
    wall_map[2][4] = ROOM_PILLAR1;
    wall_map[2][9] = ROOM_PILLAR2;
    wall_map[5][4] = ROOM_PILLAR3;
    wall_map[5][9] = ROOM_PILLAR4;
  }

  // Pass 1: Extend walls out of pillars to random direction
  int pillars_found = 0;
  int pillar_dir = 0;
  for (int i = 2; i < MAP_W - 2; i++) {
    for (int j = 2; j < MAP_H - 2; j++) {
      if ((wall_map[i][j]) >= ROOM_PILLAR1) {
        // TODO Find way to randomize same pillar ids to same direction for symmetrical pillar templates and double walls
        //pillar_dir = squirrel(wall_map[i][j], room_seed + 307) % 5;
        pillars_found++;
        pillar_dir = squirrel_3d(i, j, pillars_found, room_seed + 307) % 5;

        switch (pillar_dir) {
          case 0:  // North
            for (int build_y = 1; wall_map[i][j - build_y] == ROOM_FLOOR; build_y++) {
              wall_map[i][j - build_y] = ROOM_WALL;
            }
            break;
          case 1:  // East
            for (int build_x = 1; wall_map[i + build_x][j] == ROOM_FLOOR; build_x++) {
              wall_map[i + build_x][j] = ROOM_WALL;
            }
            break;
          case 2:  // South
            for (int build_y = 1; wall_map[i][j + build_y] == ROOM_FLOOR; build_y++) {
              wall_map[i][j + build_y] = ROOM_WALL;
            }
            break;
          case 3:  // West
            for (int build_x = 1; wall_map[i - build_x][j] == ROOM_FLOOR; build_x++) {
              wall_map[i - build_x][j] = ROOM_WALL;
            }
            break;
          case 4:  // None
            break;
        }
      }
    }
  }

  // Pass 2: Replace tile patterns
  byte tile_type = 0;
  byte floor_neighbours = 0;
  byte pillar_neighbours = 0;
  byte wall_neighbours = 0;
  byte deco_neighbours = 0;
  byte exit_neighbours = 0;
  for (int i = 1; i < MAP_W - 1; i++) {
    for (int j = 1; j < MAP_H - 1; j++) {
      floor_neighbours = 0;
      pillar_neighbours = 0;
      wall_neighbours = 0;
      deco_neighbours = 0;
      exit_neighbours = 0;
      tile_type = wall_map[i][j];

      // Find neighbours
      for (int mod_x = -1; mod_x < 2; mod_x++) {
        for (int mod_y = -1; mod_y < 2; mod_y++) {
          // Don't count self
          if (mod_x != 0 || mod_y != 0) {
            // Check neighbour tile type
            switch (wall_map[i + mod_x][j + mod_y]) {
              case ROOM_FLOOR:
                floor_neighbours++;
                break;
              case ROOM_PILLAR1:
              case ROOM_PILLAR2:
              case ROOM_PILLAR3:
              case ROOM_PILLAR4:
              case ROOM_PILLAR5:
              case ROOM_PILLAR6:
              case ROOM_PILLAR7:
              case ROOM_PILLAR8:
              case ROOM_PILLAR9:
              case ROOM_PILLAR10:
                pillar_neighbours++;
                break;
              case ROOM_WALL:
                wall_neighbours++;
                break;
              case ROOM_EXIT_N:
              case ROOM_EXIT_E:
              case ROOM_EXIT_S:
              case ROOM_EXIT_W:
                exit_neighbours++;
                break;
              case ROOM_WALL_DECO:
                deco_neighbours++;
                break;
            }
          }
        }
      }

      // Replace tiles
      d6 = 1 + (squirrel_3d(i + floor_neighbours, j + wall_neighbours, tile_type + deco_neighbours, room_seed + 347) % 6);
      
      if (tile_type >= ROOM_PILLAR1 && tile_type <= ROOM_PILLAR10) {
        if (wall_neighbours == 0) {
          // All isolated pillars to decorative wall or floor
          wall_map[i][j] = d6 >= 5 ? ROOM_WALL_DECO : ROOM_FLOOR;
        } else if (wall_neighbours == 1) {
          // Some wall end pillars to regular or decorative wall
          wall_map[i][j] = d6 == 6 ? ROOM_WALL_DECO : ROOM_WALL;
        } else if (wall_neighbours == 2 && !is_constructed && d6 >= 4) {
          // Some connecting pillars to decorative wall or floor in natural areas
          wall_map[i][j] = (d6 % 2) == 0 ? ROOM_FLOOR : ROOM_WALL_DECO;
        } else {
          wall_map[i][j] = ROOM_WALL;
        }
      }

      d6 = 1 + (squirrel_3d(i + floor_neighbours, j + wall_neighbours, tile_type + deco_neighbours, room_seed + 371) % 6);
      if (wall_map[i][j] == ROOM_FLOOR) {
        if (floor_neighbours >= 7 && d6 == 6) {
          // Some isolated floors to decorative wall
          wall_map[i][j] = ROOM_WALL_DECO;
        }

        if (wall_neighbours == 7 && floor_neighbours == 1 && d6 >= 5) {
          // Some deadend floors to wall
          wall_map[i][j] = ROOM_WALL;
        } else if ((wall_neighbours + deco_neighbours) == 7 && floor_neighbours == 1 && d6 >= 5) {
          // Some deadend floors to decorative wall
          wall_map[i][j] = ROOM_WALL_DECO;
        }
      }

      if (wall_map[i][j] == ROOM_WALL && deco_inner_walls) {
        // All inner walls to decorative wall, early stop
        wall_map[i][j] = ROOM_WALL_DECO;
        continue;
      }
    }
  }

  // Check for room corner rounding on natural areas
  d6 = 1 + (squirrel_4d(world_x, world_y, area_x, area_y, room_seed + 151) % 6);
  if (!is_constructed && !deco_inner_walls) {
    if (
      wall_map[1][1] == ROOM_FLOOR &&
      wall_map[1][2] == ROOM_FLOOR &&
      wall_map[2][1] == ROOM_FLOOR &&
      wall_map[2][2] == ROOM_FLOOR &&
      d6 <= 4
    ) {
      wall_map[1][1] = ROOM_WALL;
      d6 = 1 + (squirrel_4d(world_x, world_y, area_x, area_y, room_seed + 152) % 6);
    }

    if (
      wall_map[MAP_W - 3][1] == ROOM_FLOOR &&
      wall_map[MAP_W - 3][2] == ROOM_FLOOR &&
      wall_map[MAP_W - 2][1] == ROOM_FLOOR &&
      wall_map[MAP_W - 2][2] == ROOM_FLOOR &&
      d6 <= 4
    ) {
      wall_map[MAP_W - 2][1] = ROOM_WALL;
      d6 = 1 + (squirrel_4d(world_x, world_y, area_x, area_y, room_seed + 153) % 6);
    }

    if (
      wall_map[1][MAP_H - 3] == ROOM_FLOOR &&
      wall_map[1][MAP_H - 2] == ROOM_FLOOR &&
      wall_map[2][MAP_H - 3] == ROOM_FLOOR &&
      wall_map[2][MAP_H - 2] == ROOM_FLOOR &&
      d6 <= 4
    ) {
      wall_map[1][MAP_H - 2] = ROOM_WALL;
      d6 = 1 + (squirrel_4d(world_x, world_y, area_x, area_y, room_seed + 154) % 6);
    }

    if (
      wall_map[MAP_W - 3][MAP_H - 3] == ROOM_FLOOR &&
      wall_map[MAP_W - 3][MAP_H - 2] == ROOM_FLOOR &&
      wall_map[MAP_W - 2][MAP_H - 3] == ROOM_FLOOR &&
      wall_map[MAP_W - 2][MAP_H - 2] == ROOM_FLOOR &&
      d6 <= 4
    ) {
      wall_map[MAP_W - 2][MAP_H - 2] = ROOM_WALL;
    }
  }

  // Clear map center tile
  wall_map[4][7] = ROOM_FLOOR;

  // Pass 3: Convert wallmap to tilemap
  tileset_index = get_room_tileset_index(world_tile_data, world_x, world_y, area_x, area_y, seed);
  unsigned int tile = 0;
  
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      tile = 0;
      tile_index = get_room_tile_index(i, j, wall_map[i][j], room_seed + 615);

      // Tile already filled
      if (tile_map[i][j] != -1) {
        continue;
      }

      switch (wall_map[i][j]) {
        case ROOM_FLOOR:
          tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_FLOOR * 4)];
          tile_map[i][j] = tile;
          break;
        case ROOM_EXIT_N:
          tile_index = 0;
          tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_EXIT * 4)];
          tile_map[i][j] = tile;
          break;
        case ROOM_EXIT_E:
          tile_index = 1;
          tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_EXIT * 4)];
          tile_map[i][j] = tile;
          break;
        case ROOM_EXIT_S:
          tile_index = 2;
          tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_EXIT * 4)];
          tile_map[i][j] = tile;
          break;
        case ROOM_EXIT_W:
          tile_index = 3;
          tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_EXIT * 4)];
          tile_map[i][j] = tile;
          break;
        case ROOM_WALL:
          tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_WALL * 4)];
          tile_map[i][j] = tile;
          break;
        case ROOM_WALL_DECO:
          if (tile_index >= 3 && j + 1 < (MAP_H - 1) && wall_map[i][j + 1] == ROOM_WALL_DECO) {
            // Check for 1x2 special replacements
            tile_index = 2 * (squirrel_2d(i, j, room_seed + 241) % 2);
            
            tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_V_DECO * 4)];
            tile_map[i][j] = tile;

            tile = room_tilesets[tileset_index][(tile_index + 1) + (ROOM_TILESET_V_DECO * 4)];
            tile_map[i][j + 1] = tile;
          } else if (tile_index >= 3 && i + 1 < (MAP_W - 1) && wall_map[i + 1][j] == ROOM_WALL_DECO) {
            // Check for 2x1 special replacements
            tile_index = 2 * (squirrel_2d(i, j, room_seed + 243) % 2);
            
            tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_H_DECO * 4)];
            tile_map[i][j] = tile;

            tile = room_tilesets[tileset_index][(tile_index + 1) + (ROOM_TILESET_H_DECO * 4)];
            tile_map[i + 1][j] = tile;
          } else {
            // No special replacements, fill normally
            tile = room_tilesets[tileset_index][tile_index + (ROOM_TILESET_WALL_DECO * 4)];
            tile_map[i][j] = tile;
          }
          
          break;
      }
    }
  }

  return 1;
}

int get_room_tileset_index(int world_tile_data[15], int world_x, int world_y, int area_x, int area_y, unsigned int seed) {
  int set_variation = squirrel_4d(world_x, world_y, area_x, area_y, seed) % 4;

  if (world_tile_data[TILE_DUNGEON] == TREASURE_DUNGEON) {
    return (ROOM_TILES_DUNGEON * 4) + set_variation;
  }

  if (world_tile_data[TILE_BUILDINGS] == BULDINGS_TOWN) {
    if (world_tile_data[TILE_HEIGHT] <= HEIGHT_SEA) {
      return (ROOM_TILES_SEA_TOWN * 4) + set_variation;
    } else {
      return (ROOM_TILES_TOWN * 4) + set_variation;
    }
  } else if (world_tile_data[TILE_BUILDINGS] == BULDINGS_CITY) {
    if (world_tile_data[TILE_HEIGHT] <= HEIGHT_SEA) {
      return (ROOM_TILES_SEA_CITY * 4) + set_variation;
    } else {
      return (ROOM_TILES_CITY * 4) + set_variation;
    }
  }

  switch (world_tile_data[TILE_HEIGHT]) {
    case HEIGHT_DEEP_SEA:
      return (ROOM_TILES_DEEP_SEA * 4) + set_variation;
    case HEIGHT_SEA:
      return (ROOM_TILES_SEA * 4) + set_variation;
    //case HEIGHT_SHALLOW:
    //case HEIGHT_REGULAR:
    //case HEIGHT_HILL:
    case HEIGHT_MOUNTAIN:
      return (ROOM_TILES_MOUNTAIN * 4) + set_variation;
    case HEIGHT_PEAK:
      return (ROOM_TILES_PEAK * 4) + set_variation;
    default:
      switch (world_tile_data[TILE_BIOME]) {
      case BIOME_ICE_FIELD:
        return (ROOM_TILES_ICE_FIELD * 4) + set_variation;
      case BIOME_TUNDRA:
        return (ROOM_TILES_TUNDRA * 4) + set_variation;
      case BIOME_TAIGA:
        return (ROOM_TILES_TAIGA * 4) + set_variation;
      case BIOME_PLAINS:
        return (ROOM_TILES_PLAINS * 4) + set_variation;
      case BIOME_FOREST:
        return (ROOM_TILES_FOREST * 4) + set_variation;
      case BIOME_DEEP_FOREST:
        return (ROOM_TILES_DEEP_FOREST * 4) + set_variation;
      case BIOME_DESERT:
        return (ROOM_TILES_DESERT * 4) + set_variation;
      case BIOME_SAVANNAH:
        return (ROOM_TILES_SAVANNAH * 4) + set_variation;
      case BIOME_JUNGLE:
        return (ROOM_TILES_JUNGLE * 4) + set_variation;
    }
  }

  return 0;
}

int get_room_tile_index(int tile_x, int tile_y, byte tile_type, unsigned int seed) {
  int tile_variation = squirrel_3d(tile_x, tile_y, tile_type, seed) % 10;

  if (tile_variation <= 4) {
    return 0; // 50%
  } else if (tile_variation <= 6) {
    return 1; // 20%
  } else if (tile_variation <= 8) {
    return 2; // 20%
  } else {
    return 3; // 10%
  }
}

bool room_tile_walkable(byte wall_map[MAP_W][MAP_H], int tile_x, int tile_y) {
  switch(wall_map[tile_x][tile_y] ) {
    case ROOM_FLOOR:
    case ROOM_EXIT_N:
    case ROOM_EXIT_E:
    case ROOM_EXIT_S:
    case ROOM_EXIT_W:
      // Goal ok
      return true;
    default:
      // Goal is not reachable
      return false;
  }
}

int clear_dijkstra_map(int dijkstra_map[MAP_W][MAP_H]) {
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      dijkstra_map[i][j] = DIJKSTRA_MAX;
    }
  }

  return 1;
}

int get_dijkstra_value(int dijkstra_map[MAP_W][MAP_H], int tile_x, int tile_y) {
  if (tile_x < 0 || tile_x > MAP_W - 1) { return DIJKSTRA_MAX; }
  if (tile_y < 0 || tile_y > MAP_H - 1) { return DIJKSTRA_MAX; }

  return dijkstra_map[tile_x][tile_y];
}

int get_dijkstra_lowest(int dijkstra_map[MAP_W][MAP_H], int tile_x, int tile_y) {
  int tile_value = DIJKSTRA_MAX;
  int current_lowest = DIJKSTRA_MAX;

  tile_value = get_dijkstra_value(dijkstra_map, tile_x - 1, tile_y);
  if (tile_value < current_lowest) {current_lowest = tile_value;}
  tile_value = get_dijkstra_value(dijkstra_map, tile_x + 1, tile_y);
  if (tile_value < current_lowest) {current_lowest = tile_value;}
  tile_value = get_dijkstra_value(dijkstra_map, tile_x, tile_y - 1);
  if (tile_value < current_lowest) {current_lowest = tile_value;}
  tile_value = get_dijkstra_value(dijkstra_map, tile_x, tile_y + 1);
  if (tile_value < current_lowest) {current_lowest = tile_value;}

  return current_lowest;
}

int build_dijkstra_map(int dijkstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H], int goal_x, int goal_y) {
  // Clear current map
  clear_dijkstra_map(dijkstra_map);

  // Sanity check that goal is not inside a wall
  if (!room_tile_walkable(wall_map, goal_x, goal_y)) {
    return 0;
  }

  // Set goal tile value
  dijkstra_map[goal_x][goal_y] = 0;

  return build_dijkstra_map(dijkstra_map, wall_map);
}

int build_dijkstra_map(int dijkstra_map[MAP_W][MAP_H], byte wall_map[MAP_W][MAP_H]) {
  // Expecting that dijkstra_map is already set up with goal tiles

  // Calculate map tile values
  bool map_changed = true;
  int neighbour_value = DIJKSTRA_MAX;

  while (map_changed) {
    map_changed = false;

    for (int i = 0; i < MAP_W; i++) {
      for (int j = 0; j < MAP_H; j++) {
        if (room_tile_walkable(wall_map, i, j)) {
          neighbour_value = get_dijkstra_lowest(dijkstra_map, i, j);

          if (dijkstra_map[i][j] > neighbour_value + 1) {
            dijkstra_map[i][j] = neighbour_value + 1;
            map_changed = true;
          }
        }
      }
    }
  }

  return 1;
}

int merge_dijkstra_maps(int output_map[MAP_W][MAP_H], int input1_map[MAP_W][MAP_H]) {
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      if (input1_map[i][j] < output_map[i][j]) {
        output_map[i][j] = input1_map[i][j];
      }
    }
  }

  return 1;
}

int get_dijkstra_direction(int dijkstra_map[MAP_W][MAP_H], int tile_x, int tile_y, unsigned int seed) {
  return get_dijkstra_direction(dijkstra_map, tile_x, tile_y, 0, seed);
}

int get_dijkstra_direction(int dijkstra_map[MAP_W][MAP_H], int tile_x, int tile_y, int target_distance, unsigned int seed) {
  int map_value = DIJKSTRA_MAX;
  int tile_value = DIJKSTRA_MAX;
  int current_lowest = DIJKSTRA_MAX;
  int current_dir = 0;

  int d4 = 1 + (squirrel_2d(tile_x, tile_y, seed + 893) % 4);

  // Check current, no move if already in target
  map_value = get_dijkstra_value(dijkstra_map, tile_x, tile_y);
  if (map_value == target_distance) { return current_dir; }

  // Check west
  map_value = get_dijkstra_value(dijkstra_map, tile_x - 1, tile_y);
  tile_value = abs(map_value - target_distance);
  if (map_value < DIJKSTRA_MAX && tile_value < current_lowest) {
    current_lowest = tile_value;
    current_dir = DIR_W;
  }
  if (map_value == target_distance) { return current_dir; }

  // Check east
  map_value = get_dijkstra_value(dijkstra_map, tile_x + 1, tile_y);
  tile_value = abs(map_value - target_distance);
  if (map_value < DIJKSTRA_MAX && tile_value < current_lowest) {
    current_lowest = tile_value;
    current_dir = DIR_E;
  } else if (tile_value == current_lowest && d4 >= 2) {
    // Equally valid direction found, choose randomly
    current_lowest = tile_value;
    current_dir = DIR_E;

    d4 = 1 + (squirrel_2d(tile_x, tile_y, seed + 620 + tile_value) % 4);
  }
  if (map_value == target_distance) { return current_dir; }
  
  // Check north
  map_value = get_dijkstra_value(dijkstra_map, tile_x, tile_y - 1);
  tile_value = abs(map_value - target_distance);
  if (map_value < DIJKSTRA_MAX && tile_value < current_lowest) {
    current_lowest = tile_value;
    current_dir = DIR_N;
  } else if (tile_value == current_lowest && d4 >= 2) {
    // Equally valid direction found, choose randomly
    current_lowest = tile_value;
    current_dir = DIR_N;

    d4 = 1 + (squirrel_2d(tile_x, tile_y, seed + 389 + tile_value) % 4);
  }
  if (map_value == target_distance) { return current_dir; }
  
  // Check south
  map_value = get_dijkstra_value(dijkstra_map, tile_x, tile_y + 1);
  tile_value = abs(map_value - target_distance);
  if (map_value < DIJKSTRA_MAX && tile_value < current_lowest) {
    current_lowest = tile_value;
    current_dir = DIR_S;
  } else if (tile_value == current_lowest && d4 >= 2) {
    // Equally valid direction found, choose randomly
    current_lowest = tile_value;
    current_dir = DIR_S;

    d4 = 1 + (squirrel_2d(tile_x, tile_y, seed + 547 + tile_value) % 4);
  }
  if (map_value == target_distance) { return current_dir; }

  // Reset direction selection if lowest found value is non-valid
  if (current_lowest == DIJKSTRA_MAX) {
    current_dir = 0;
  }

  return current_dir;
}

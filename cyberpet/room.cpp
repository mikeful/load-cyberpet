#include "room.h"

int setup_room(byte wall_map[MAP_W][MAP_H], int tile_map[MAP_W][MAP_H], int world_x, int world_y, int world_tile_data[15], int area_x, int area_y, byte exits, unsigned int seed) {
  // Build edge walls, clear tilemap
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      if (i == 0 || j == 0 || i == MAP_W - 1 || j == MAP_H - 1) {
        wall_map[i][j] = ROOM_WALL;
      } else {
        wall_map[i][j] = ROOM_FLOOR;
      }

      tile_map[i][j] = 0;
    }
  }

  // Build room exit doors to edge walls
  if (exits & EXIT_N) { wall_map[4][0] = ROOM_EXIT_N; }
  if (exits & EXIT_S) { wall_map[4][MAP_H - 1] = ROOM_EXIT_S; }
  if (exits & EXIT_W) { wall_map[0][7] = ROOM_EXIT_W; }
  if (exits & EXIT_E) { wall_map[MAP_W - 1][7] = ROOM_EXIT_E; }

  // TODO Load pillars from template selected with tile density value
  wall_map[2][4] = ROOM_PILLAR1;
  wall_map[2][9] = ROOM_PILLAR2;
  wall_map[5][4] = ROOM_PILLAR3;
  wall_map[5][9] = ROOM_PILLAR4;

  /*
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
  */

  // Pass 1: Extend walls out of pillars to random direction
  int pillars_found = 0;
  int pillar_dir = 0;
  for (int i = 2; i < MAP_W - 2; i++) {
    for (int j = 2; j < MAP_H - 2; j++) {
      if ((wall_map[i][j]) >= ROOM_PILLAR1) {
        // TODO Find way to randomize same pillar ids to same direction for symmetrical pillar templates and double walls
        //pillar_dir = squirrel(wall_map[i][j], seed + 307) % 5;
        pillars_found++;
        pillar_dir = squirrel_3d(i, j, pillars_found, seed + 307) % 5;

        switch (pillar_dir) {
          case 0:  // N
            for (int build_y = 1; wall_map[i][j - build_y] == ROOM_FLOOR; build_y++) {
              wall_map[i][j - build_y] = ROOM_WALL;
            }
            break;
          case 1:  // E
            for (int build_x = 1; wall_map[i + build_x][j] == ROOM_FLOOR; build_x++) {
              wall_map[i + build_x][j] = ROOM_WALL;
            }
            break;
          case 2:  // S
            for (int build_y = 1; wall_map[i][j + build_y] == ROOM_FLOOR; build_y++) {
              wall_map[i][j + build_y] = ROOM_WALL;
            }
            break;
          case 3:  // W
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
  byte d6 = 0;
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
      d6 = 1 + (squirrel_3d(i + floor_neighbours, j + wall_neighbours, tile_type + deco_neighbours, seed + 347) % 6);
      
      if (tile_type >= ROOM_PILLAR1 && tile_type <= ROOM_PILLAR10) {
        if (wall_neighbours == 0) {
          // All isolated pillars to decorative wall or floor
          wall_map[i][j] = d6 >= 5 ? ROOM_WALL_DECO : ROOM_FLOOR;
        } else if (wall_neighbours == 1) {
          // Some wall end pillars to regular or decorative wall
          wall_map[i][j] = d6 == 6 ? ROOM_WALL_DECO : ROOM_WALL;
        //} else if (wall_neighbours == 2) {
            // All connecting pillars to regular or decorative wall
        //  wall_map[i][j] = d6 == 6 ? ROOM_WALL_DECO : ROOM_WALL;
        } else {
          wall_map[i][j] = ROOM_WALL;
        }
      }

      if (wall_map[i][j] == ROOM_WALL) {
        // All inner walls to decorative wall, early stop
        wall_map[i][j] = ROOM_WALL_DECO;
        continue;
      }

      if (tile_type == ROOM_FLOOR) {
        if (floor_neighbours >= 7 && d6 == 6) {
          // Some isolated floors to decorative wall
          wall_map[i][j] = ROOM_WALL_DECO;
        }
      }

    }
  }

  // Pass 3: Convert wallmap to tilemap
  unsigned int tile = 0;
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      tile = 0;
      switch (wall_map[i][j]) {
        case ROOM_FLOOR:
        case ROOM_EXIT_N:
        case ROOM_EXIT_E:
        case ROOM_EXIT_S:
        case ROOM_EXIT_W:
          tile = 432;
          break;
        case ROOM_WALL:
          tile = 1512;
          break;
        case ROOM_WALL_DECO:
          tile = 1510;
          break;
      }

      tile_map[i][j] = tile;
    }
  }

  return 1;
}
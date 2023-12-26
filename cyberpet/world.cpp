#include "world.h"

byte get_room_exits(int world_x, int world_y) {
  return world_tile_exit_patterns[
    world_x % 4 + (4 * (world_y % 4))
  ];
}

unsigned int get_room_level(int world_x, int world_y) {
  return 1 + abs(world_x) + abs(world_y);
}

unsigned int get_room_level(int world_x, int world_y, int world_z) {
  return 1 + abs(world_x) + abs(world_y) + (abs(world_z) * 3);
}

#include "world.h"

byte get_room_exits(int world_x, int world_y) {
  return world_tile_exit_patterns[
    world_x % 4 + (4 * (world_y % 4))
  ];
}

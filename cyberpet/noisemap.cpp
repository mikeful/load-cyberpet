#include "noisemap.h"

#define PI 3.1415926535897932384626433832795

float sine_gauss_filter_2d(int width, int x, int y) {
  /*
  Approximate gaussian blur filter in multiple dimensions using half of sine wave. Takes max
  width of filter in positive direction (half of full width) and x/y coordinate values as
  rest of the arguments. Filter area max total value: width^2
  */

  // Coordinate outside of filter width, return 0 instead of letting value rise with sine
  if (abs(x) >= width || abs(y) >= width) { return 0.0; }

  float value = 0.0;
  value += cos((float(abs(x)) * PI) / (float)width) * 0.5;
  value += cos((float(abs(y)) * PI) / (float)width) * 0.5;

  // TODO Dividing by area cell count gives only 64% of range (due rounding errors?)
  return value / float(pow(width, 2));
}

float sine_gauss_filter_3d(int width, int x, int y, int z) {
  /*
  Approximate gaussian blur filter in multiple dimensions using half of sine wave. Takes max
  width of filter in positive direction (half of full width) and x/y/z/... coordinate values as
  rest of the arguments.
  */

  // Coordinate outside of filter width, return 0 instead of letting value rise with sine
  if (abs(x) >= width || abs(y) >= width || abs(z) >= width) { return 0.0; }

  float value = 0.0;
  value += cos(float(abs(x) * PI) / (float)width) * 0.5;
  value += cos(float(abs(y) * PI) / (float)width) * 0.5;
  value += cos(float(abs(z) * PI) / (float)width) * 0.5;

  // Dividing by area cell count gives only 97% of range due rounding errors
  return value / float(pow(width, 3));
}

int get_cosine_noisepoint(int pos_x, int pos_y, unsigned int seed, float scale) {
  float noise_value = 0.0;
  int filter_width = 5;
  int filter_range = int(float(filter_width) / 2.0);

  float weight = 1.0;

  for (int map_x = -filter_range; map_x < filter_range + 1; map_x++) {
    for (int map_y = -filter_range; map_y < filter_range + 1; map_y++) {
      weight = sine_gauss_filter_2d(filter_width, map_x, map_y);

      noise_value += float(
                       squirrel_2d(
                         int(float(pos_x + map_x) * scale),
                         int(float(pos_y + map_y) * scale),
                         seed)
                       % 255)
                     * weight;
    }
  }

  return int(noise_value);
}

int get_cosine_noisepoint3d(int pos_x, int pos_y, int pos_z, unsigned int seed, float scale) {
  float noise_value = 0.0;
  int filter_width = 5;
  int filter_range = int(float(filter_width) / 2.0);

  float weight = 1.0;

  for (int map_x = -filter_range; map_x < filter_range + 1; map_x++) {
    for (int map_y = -filter_range; map_y < filter_range + 1; map_y++) {
      for (int map_z = -filter_range; map_z < filter_range + 1; map_z++) {
        //weight = sine_gauss_filter_3d(filter_width, map_x, map_y, map_z);

        noise_value += float(
                         squirrel_3d(
                           int(float(pos_x + map_x) * scale),
                           int(float(pos_y + map_y) * scale),
                           int(float(pos_z + map_z) * scale),
                           seed)
                         % 255)
                       * weight;
      }
    }
  }

  return int(noise_value / pow(filter_width, 3));
}

int get_simple_noisepoint(int pos_x, int pos_y, unsigned int seed, float scale) {
  float noise_value = 0.0;
  int filter_width = 5;
  int filter_range = int(float(filter_width) / 2.0);

  float weight = 1.0;
  byte skipped = 0;
  byte lowered = 0;

  for (int map_x = -filter_range; map_x < filter_range + 1; map_x++) {
    for (int map_y = -filter_range; map_y < filter_range + 1; map_y++) {
      weight = 1.0;

      if (abs(map_x) + abs(map_y) >= filter_range + 2) {
        skipped += 1;
        continue;
      }
      if (abs(map_x) + abs(map_y) >= filter_range + 1) {
        lowered += 1;
        weight = 0.5;
      }

      noise_value += float(
                       squirrel_2d(
                         int(float(pos_x + map_x) * scale),
                         int(float(pos_y + map_y) * scale),
                         seed)
                       % 255)
                     * weight;
    }
  }

  float cells = pow(filter_width, 2) - (skipped + (lowered / 2));

  return int(noise_value / cells);
}

int get_simple_noisepoint3d(int pos_x, int pos_y, int pos_z, unsigned int seed, float scale) {
  float noise_value = 0.0;
  int filter_width = 5;
  int filter_range = int(float(filter_width) / 2.0);

  float weight = 1.0;
  byte skipped = 0;
  byte lowered = 0;

  for (int map_x = -filter_range; map_x < filter_range + 1; map_x++) {
    for (int map_y = -filter_range; map_y < filter_range + 1; map_y++) {
      for (int map_z = -filter_range; map_z < filter_range + 1; map_z++) {
        weight = 1.0;

        if (abs(map_x) + abs(map_y) + abs(map_z) >= filter_range + 2) {
          skipped += 1;
          continue;
        }
        if (abs(map_x) + abs(map_y) + abs(map_z) >= filter_range + 1) {
          lowered += 1;
          weight = 0.5;
        }

        noise_value += float(squirrel_3d(
                               int(float(pos_x + map_x) * scale),
                               int(float(pos_y + map_y) * scale),
                               int(float(pos_z + map_z) * scale),
                               seed)
                             % 255)
                       * weight;
      }
    }
  }

  float cells = pow(filter_width, 3) - (skipped + (lowered / 2));

  return int(noise_value / cells);
}

int get_simple_noisemap(byte noisemap2d[MAP_W][MAP_H], int pos_x, int pos_y, int width, int height, unsigned int seed, float scale) {
  int noise_value = 0;

  for (int map_x = 0; map_x < width; map_x++) {     // for map_y in range(0, height):
    for (int map_y = 0; map_y < height; map_y++) {  // for map_x in range(0, width):
      noise_value = get_simple_noisepoint(
        pos_x + map_x, pos_y + map_y, seed = seed, scale = scale);

      noisemap2d[map_x][map_y] = noise_value;
    }
  }

  return 1;
}

int get_simple_noisemap3d(byte noisemap2d[MAP_W][MAP_H], int pos_x, int pos_y, int pos_z, int width, int height, unsigned int seed, float scale) {
  int noise_value = 0;

  for (int map_x = 0; map_x < width; map_x++) {     // for map_y in range(0, height):
    for (int map_y = 0; map_y < height; map_y++) {  // for map_x in range(0, width):
      noise_value = get_simple_noisepoint3d(
        pos_x + map_x, pos_y + map_y, pos_z, seed = seed, scale = scale);

      noisemap2d[map_x][map_y] = noise_value;
    }
  }

  return 1;
}

// Y---Y---Y---Y
// .\./.\./.\./.
// --Y---Y---Y--
// ./.\./.\./.\.
// Y---Y---Y---Y
// .\./.\./.\./.
// --Y---Y---Y--
bool is_triangle_grid_node(int check_x, int check_y, int grid_size) {
  if (mod(check_x, grid_size) == 0 and mod(check_y, grid_size) == 0) {
    return true;
  }

  int half_grid = grid_size / 2;
  if (mod((check_x + half_grid), grid_size) == 0
    && mod((check_y + half_grid), grid_size) == 0
  ) {
    return true;
  }

  return false;
}

bool is_poi_big(int world_x, int world_y, int tile_value, int poi_prob, int poi_grid_size, unsigned int seed) {
  // poi_grid_size 24
  if (tile_value < poi_prob) {
    return false;
  }

  if (is_triangle_grid_node(world_x, world_y, poi_grid_size)) {
    return true;
  }

  // 3x3
  for (int mod_x = -1; mod_x < 2; mod_x++) {
    for (int mod_y = -1; mod_y < 2; mod_y++) {
      if (is_triangle_grid_node(world_x + mod_x, world_y + mod_y, poi_grid_size)) {
        return true;
      }
    }
  }

  // 2x2
  // for mod_x, mod_y in [(1,0), (0,1), (1,1)]: 
  //     if noisemap.is_triangle_grid_node((world_x + mod_x), (world_y + mod_y), poi_grid_size):
  //         return true

  int noise_x = 1 - (squirrel_2d(world_x, world_y, seed + 102) % 3);
  int noise_y = 1 - (squirrel_2d(world_x, world_y, seed + 202) % 3);
  if (is_triangle_grid_node(world_x + noise_x, world_y + noise_y, poi_grid_size)) {
    return true;
  }

  return false;
}

bool is_poi_small(int world_x, int world_y, int tile_value, int poi_prob, int poi_grid_size) {
  // poi_grid_size 12
  if (tile_value < poi_prob) {
    return false;
  }  

  if (is_triangle_grid_node(world_x, world_y, poi_grid_size)) {
    return true;
  }

  return false;
}

int get_world_tile(int tile_data[15], int world_x, int world_y, int time_step, unsigned int seed) {
    unsigned int level = abs(world_x) + abs(world_y);

    // Height and biome
    int height_value = get_simple_noisepoint(
      world_x, world_y, seed + 1, 0.3
    );
    int temperature = get_simple_noisepoint(
      world_x, world_y, seed + 2, 0.3
    );
    int lushness = get_simple_noisepoint(
      world_x, world_y, seed + 3, 0.3
    );

    int height_env = 255 - height_value;
    if (height_value > 160) {
      // Drop temperature near mountains
      temperature = (temperature + height_env) / 2;
    }
    if (height_value < 105) {
      // Raise lushness near oceans
      lushness = (lushness + height_env) / 2;
    }

    height_value = map(height_value, 0, 255, 0, 100);
    temperature = map(temperature, 0, 255, 0, 3);
    lushness = map(lushness, 0, 255, 0, 3);

    int biome = biome_matrix[(temperature * 3) + lushness];

    int height = HEIGHT_REGULAR;
    if (height_value <= 10) {
      height = HEIGHT_DEEP_SEA;
    }
    if (height_value <= 25) {
      height = HEIGHT_SEA;
    }
    // if (height_value == 3) {
    //     height = HEIGHT_SHALLOW;
    // }
    if (height_value >= 65) {
      height = HEIGHT_HILL;
    }
    if (height_value >= 72) {
      height = HEIGHT_MOUNTAIN;
    }
    if (height_value >= 85) {
      height = HEIGHT_PEAK;
    }

    // Human settlements
    int humanity = get_simple_noisepoint(
      world_x, world_y, seed + 4, 0.3
    );

    int buildings = BULDINGS_NONE;
    if (is_poi_big(world_x, world_y, humanity, 166, 24, seed)) {
      buildings = BULDINGS_CITY;
    }
    else if (is_poi_small(world_x, world_y, humanity, 166, 12)) {
      buildings = BULDINGS_TOWN;
    }
    else if (humanity > 217) {  // 85%
      buildings = BULDINGS_HUT;
    }
    else if( humanity > 166) {  // 65%
      buildings = BULDINGS_CAMP;
    }

    // Dungeon enterances
    unsigned int treasure_offset = 3;
    int treasure = get_simple_noisepoint(
      world_x + treasure_offset, world_y + treasure_offset, seed + 5, 0.3
    );

    int dungeon = TREASURE_NONE;
    if (is_poi_small(
      world_x + treasure_offset, world_y + treasure_offset, treasure, 166, 12
    )) {
      dungeon = TREASURE_DUNGEON;
    } else if (treasure > 217) {  // 85%
      dungeon = TREASURE_CAMP;
    }

    // Faction presence/control
    int faction_human = get_simple_noisepoint3d(
      world_x, world_y, time_step / 3, seed + 6, 0.3
    );
    int faction_monster = get_simple_noisepoint3d(
      world_x, world_y, time_step, seed + 7, 0.3
    );
    int faction_nature = get_simple_noisepoint3d(
      world_x, world_y, time_step / 7, seed + 8, 0.3
    );

    faction_human = (faction_human + humanity + humanity) / 3;
    faction_monster = (faction_monster + treasure + treasure) / 3;
    faction_nature *= 1.05;
    float control = 1.09;

    int faction = FACTION_CONFLICT;
    if (
      faction_human > 128
      && faction_human > faction_monster * control
      && faction_human > faction_nature * control
    ) {
      faction = FACTION_HUMAN;
    } else if (
      faction_monster > 128
      && faction_monster > faction_human * control
      && faction_monster > faction_nature * control
    ) {
      faction = FACTION_MONSTER;
    } else if (
      faction_nature > 128
      && faction_nature > faction_human * control
      && faction_nature > faction_monster * control
    ) {
      faction = FACTION_NATURE;
    }
    // elif faction_human < 102 && faction_monster < 102 && faction_nature < 102: // Around 40%
    //    faction = FACTION_NONE;
    // }

    // Room density/zoom
    int density = get_simple_noisepoint(world_x, world_y, seed + 9, 0.3);
    density = map(density, 0, 255, 0, 5);

    tile_data[TILE_LEVEL] = level;
    tile_data[TILE_HEIGHT_VALUE] = height_value;
    tile_data[TILE_TEMPERATURE] = temperature;
    tile_data[TILE_LUSHNESS] = lushness;
    tile_data[TILE_HEIGHT] = height;
    tile_data[TILE_BIOME] = biome;
    tile_data[TILE_DENSITY] = density;
    tile_data[TILE_HUMANITY] = humanity;
    tile_data[TILE_BUILDINGS] = buildings;
    tile_data[TILE_TREASURE] = treasure;
    tile_data[TILE_DUNGEON] = dungeon;
    tile_data[TILE_FACTION] = faction;
    tile_data[TILE_FACTION_HUMAN] = faction_human;
    tile_data[TILE_FACTION_MONSTER] = faction_monster;
    tile_data[TILE_FACTION_NATURE] = faction_nature;

    return 1;
}

unsigned int get_world_draw_tile(int world_tile_data[15]) {
  int tile = 0;

  switch (world_tile_data[TILE_HEIGHT]) {
    case HEIGHT_DEEP_SEA:
      tile = 442;
      break;
    case HEIGHT_SEA:
      tile = 441;
      break;
    //case HEIGHT_SHALLOW:
    //case HEIGHT_REGULAR:
    //case HEIGHT_HILL:
    case HEIGHT_MOUNTAIN:
      tile = 1516;
      break;
    case HEIGHT_PEAK:
      tile = 1517;
      break;
    default:
      switch (world_tile_data[TILE_BIOME]) {
      case BIOME_ICE_FIELD:
        tile = 588;
        break;
      case BIOME_TUNDRA:
        tile = 434;
        break;
      case BIOME_TAIGA:
        tile = 1511;
        break;
      case BIOME_PLAINS:
        tile = 432;
        break;
      case BIOME_FOREST:
        tile = 1512;
        break;
      case BIOME_DEEP_FOREST:
        tile = 1510;
        break;
      case BIOME_DESERT:
        tile = 1920;
        break;
      case BIOME_SAVANNAH:
        tile = 436;
        break;
      case BIOME_JUNGLE:
        tile = 1377;
        break;
      break;
    }
  }

  if (world_tile_data[TILE_BUILDINGS] == BULDINGS_TOWN) {
    if (world_tile_data[TILE_HEIGHT] <= HEIGHT_SEA) {
      tile = 1537;
    } else {
      tile = 1519;
    }
  } else if (world_tile_data[TILE_BUILDINGS] == BULDINGS_CITY) {
    if (world_tile_data[TILE_HEIGHT] <= HEIGHT_SEA) {
      tile = 1537;
    } else {
      tile = 1523;
    }
  }

  if (world_tile_data[TILE_DUNGEON] == TREASURE_DUNGEON) {
    tile = 1531;
  }

  return tile;
}

byte get_room_exits(int world_x, int world_y) {
  return world_tile_exit_patterns[
    mod(world_x, 4) + (4 * mod(world_y, 4))
  ];
}

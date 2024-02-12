#include "Arduino.h"
//#include "heltec.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Preferences.h>
#include "lora32_pins.h"
#include "battery.h"

#include "toolbox.h"
#include "player.h"
#include "medievalish_chonker_mono.h"
#include "tileset.h"
#include "noisemap.h"
#include "room.h"
#include "items.h"
#include "entities.h"

#define STATE_START 0
#define STATE_MAINMENU 1
#define STATE_ROOM 2
#define STATE_WORLDMAP 3
#define STATE_GAMEMENU 4
#define STATE_DEATH 5

#define PLAYER_PROFILE_NONE 0 // No preference
#define PLAYER_PROFILE_MELEE 1 // Prefer melee range
#define PLAYER_PROFILE_RANGED 2 // Perfer ranged range
#define PLAYER_PROFILE_FLEE 3 // Move to previous room

Preferences preferences;

// Game state
unsigned int content_seed = 0; // Determines world content, keep same
unsigned int action_seed = 0; // Determines entity actions/dice/etc, mess as often as possible
byte game_state = STATE_START;

// Player character
int world_time = 0;
unsigned int player_level = 1;
unsigned int player_max_level = 1;
uint64_t player_exp = 0;
unsigned int player_exp_multiplier = 1;
int level_ups = 0;
uint64_t player_gold = 0;

bool player_dead = false;
bool player_dead_setup = false;

byte player_profile = PLAYER_PROFILE_NONE;
int player_profile_ticks = 0;
int target_distance = 1;

uint64_t player_prev_hp = 0;
long long player_diff_hp = 0;
uint64_t player_prev_sp = 0;
long long player_diff_sp = 0;
uint64_t player_prev_exp = 0;
long long player_diff_exp = 0;
uint64_t player_prev_gold = 0;
long long player_diff_gold = 0;

int world_x = 0;
int world_y = 0;
int world_target_x = 0;
int world_target_y = 0;
int world_dir = 0;

int area_x = 1; // 3x3 rooms in world tile
int area_y = 1;
int area_target_x = 0;
int area_target_y = 0;
int area_dir = 0;

unsigned int room_x = 4; // Player position in room
unsigned int room_y = 7;
int ai_room_dir = 0;
byte last_door = 0;

// Room map
bool new_room = true;
byte room_wallmap[MAP_W][MAP_H];
int room_tilemap[MAP_W][MAP_H];
int room_entity_tilemap[MAP_W][MAP_H];
int room_effect_tilemap[MAP_W][MAP_H];

// Player/entity navigation maps
byte room_exits = 0;
byte area_exits = 0;
int room_exit_navmap[MAP_W][MAP_H];
int room_exitn_navmap[MAP_W][MAP_H];
int room_exite_navmap[MAP_W][MAP_H];
int room_exits_navmap[MAP_W][MAP_H];
int room_exitw_navmap[MAP_W][MAP_H];

uint64_t entities[ENTITY_SIZE][ENTITY_ATTRS];
int room_player_navmap[MAP_W][MAP_H];
int room_entity_navmap[MAP_W][MAP_H];
int room_entity_idmap[MAP_W][MAP_H];
bool update_player_navmap = true;
bool update_entity_navmap = false; // Room generation updates at start

// World map
int noisemap[MAP_W][MAP_H];
int world_tile_data[15];

// Entity dead status cache
#define WORLD_DEAD_SIZE 1000
byte world_tile_dead[WORLD_DEAD_SIZE];
byte world_tile_dead_ttl[WORLD_DEAD_SIZE];
byte world_tile_dead_value = 0;
byte entity_index = 0;
int tile_index = 0;

// Misc helper variables
unsigned int counter = 1;
unsigned int counter_at = 0;
unsigned int tile = 0;
int tile_pixel = 0;
unsigned int check_entity_x = 0;
unsigned int check_entity_y = 0;
int entity_distance = 1;
int combat_result = 0;
bool should_combat = false;
int equipment_id = 0;

// Button/input system
#define BUTTON_SIZE 5
#define BUTTON_PIN 0
#define BUTTON_LED_PIN 1
#define BUTTON_STATE 2
#define BUTTON_LAST_STATE 3
#define BUTTON_CHANGED 4

#define BTN_USR 0
#define BTN_EQUIP 1
#define BTN_MAGIC 2
#define BTN_ITEM 3
#define BTN_FLEE 4

#define BUTTON_UP LOW
#define BUTTON_DOWN HIGH

int input_state[BUTTON_SIZE][5] = {
  {-1, -1, BUTTON_UP, BUTTON_UP, 0}, // U - BTN_USR
  {GPIO6, GPIO7, BUTTON_UP, BUTTON_UP, 0}, // A - BTN_EQUIP
  {GPIO42, GPIO41, BUTTON_UP, BUTTON_UP, 0}, // B - BTN_MAGIC
  {GPIO4, GPIO5, BUTTON_UP, BUTTON_UP, 0}, // C - BTN_ITEM
  {GPIO40, GPIO39, BUTTON_UP, BUTTON_UP, 0}, // D - BTN_FLEE
};
unsigned long input_state_time[BUTTON_SIZE];
const unsigned long debounce_delay = 5; // ms

bool ai_active = true;
int button_pin;
int button_value;
int lastButtonState = BUTTON_UP;
int led_pin;

// Short UI message system
String toast_message1 = "";
unsigned int toast_message_ticks1 = 0;
String toast_message2 = "";
unsigned int toast_message_ticks2 = 0;

// Battery stuff
int analogValue = 0;
float floatVoltage = 4.2;
float floatVoltageSlow = 4.2;
int voltage = 4200;

// Chip id
unsigned int chip_id = 0;

// Display
Adafruit_SSD1306 display1(128, 64, &Wire, 21, 500000UL);

void setup() {
  // Setup display
  Wire.begin(17, 18);
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Setup screen rotation
  // Only vertical for now, horizontal support needs code changes to room generators
  //display1.setRotation(1); // 90 degrees, USB on bottom
  display1.setRotation(3); // 270 degrees, USB on top

  display1.clearDisplay();
  display1.display();

  // Setup 8x8 bitmap font
  display1.setFont(&medievalish_chonker_mono_Font);
  display1.setTextSize(1);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0, 0);
  display1.setTextWrap(false);

  // Setup battery
  setup_battery();

  // Setup input system
  for (int i = 0; i < BUTTON_SIZE; i++) {
    if (input_state[i][BUTTON_PIN] != -1) {
      pinMode(input_state[i][BUTTON_PIN], INPUT);
    }

    if (input_state[i][BUTTON_LED_PIN] != -1) {
      pinMode(input_state[i][BUTTON_LED_PIN], OUTPUT);
    }

    input_state_time[i] = 0;
  }

  Serial.begin(115200);

  // Read MAC as chip id and use in content seed
  // https://github.com/espressif/arduino-esp32/blob/master/libraries/ESP32/examples/ChipID/GetChipID/GetChipID.ino
  for(int i = 0; i < 17; i = i + 8) {
	  chip_id |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
  content_seed += chip_id;

  // Read save data
  preferences.begin("cyberpet", true);
  world_time = preferences.getInt("world_t", 0);
  player_level = preferences.getUInt("plr_l", 1);
  player_max_level = preferences.getUInt("plr_max_l", 1);
  player_exp = preferences.getULong64("plr_xp", 0);
  player_exp_multiplier = preferences.getUInt("plr_xp_x", 1);
  level_ups = preferences.getInt("level_ups", 0);
  player_gold = preferences.getULong64("player_gp", 0);
  world_x = preferences.getInt("world_x", 0);
  world_y = preferences.getInt("world_y", 0);
  area_x = preferences.getInt("area_x", 0);
  area_y = preferences.getInt("area_y", 0);
  room_x = preferences.getUInt("room_x", 4);
  room_y = preferences.getUInt("room_y", 7);
  preferences.end();

  // Setup game
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      room_wallmap[i][j] = 0;
      room_tilemap[i][j] = -1;
      room_entity_idmap[i][j] = 0;
      room_entity_tilemap[i][j] = -1;
      room_effect_tilemap[i][j] = -1;
    }
  }

  clear_dijkstra_map(room_exit_navmap);
  clear_dijkstra_map(room_exitn_navmap);
  clear_dijkstra_map(room_exite_navmap);
  clear_dijkstra_map(room_exits_navmap);
  clear_dijkstra_map(room_exitw_navmap);
  clear_dijkstra_map(room_player_navmap);
  clear_dijkstra_map(room_entity_navmap);
}

void loop() {
  counter++;

  if (counter % 3 == 0) {
    // Battery stuff
    analogValue = read_battery();
    floatVoltage = batFactor * (float)analogValue;
    floatVoltageSlow = (floatVoltageSlow * 0.98) + (floatVoltage * 0.02);

    voltage = (int)(floatVoltageSlow * 1000.0);
    progress = map(voltage, 2200, 4200, 0, 100);

    // Mess action random seed with jittering battery value
    action_seed += (unsigned int)analogValue;

    // Display low battery warnings
    if (progress == 20) {
      toast_message1 = "Bat " + String(progress) + "%";
      toast_message2 = "Charge";
      toast_message_ticks1 = 6;
      toast_message_ticks2 = 6;
    } else if (progress <= 15) {
      toast_message1 = "Bat " + String(progress) + "%";
      toast_message2 = "Critical";
      toast_message_ticks1 = 6;
      toast_message_ticks2 = 6;
    }
  }

  // Clear the display
  display1.clearDisplay();

  // Read input
  for (int i = 0; i < BUTTON_SIZE; i++) {
    button_pin = input_state[i][BUTTON_PIN];
    led_pin = input_state[i][BUTTON_LED_PIN];

    if (button_pin != -1) {
      digitalWrite(led_pin, LOW);
    }

    if (button_pin != -1) {
      input_state[i][BUTTON_CHANGED] = 0;
      button_value = digitalRead(button_pin);

      if (button_value != input_state[i][BUTTON_LAST_STATE]) {
        // Reset debounce timer
        input_state_time[i] = millis();
      }

      // Update value if debounce delay has passed with same value
      if ((millis() - input_state_time[i]) > debounce_delay) {
        if (button_value != input_state[i][BUTTON_STATE]) {
          input_state[i][BUTTON_STATE] = button_value;
          input_state[i][BUTTON_CHANGED] = 1;

          if (button_pin != -1 && button_value == LOW) {
            digitalWrite(led_pin, HIGH);
          }
        }
      }

      input_state[i][BUTTON_LAST_STATE] = button_value;
    }
  }

  switch(game_state) {
    case STATE_START:
      // TODO Load from save
      setup_player_entity(entities, player_level);
      game_state = STATE_MAINMENU;

      counter_at = counter + 50;

      break; // STATE_START
    case STATE_MAINMENU:
      // TODO Display logo + help
      // TODO Equipment selector for the next run

      display1.setCursor(0, 8);
      display1.println(" Legend ");
      display1.setCursor(0, 16);
      display1.println(" of the ");
      display1.setCursor(0, 24);
      display1.println("Advnture");
      display1.setCursor(0, 32);
      display1.println("  Dude  ");

      display1.drawBitmap(4 * 8, 7 * 8, tiles[entities[ENTITY_ID_PLAYER][ENTITY_ICON]], 8, 8, WHITE);

      if (counter >= counter_at) {
        // Start menu screen timer ran out, clear stuff and start again
        game_state = STATE_ROOM;
      }

      break; // STATE_MAINMENU
    case STATE_ROOM:
      // Run player character profile timer
      if (player_profile_ticks > 0) {
        player_profile_ticks--;

        if (player_profile_ticks == 0) {
          player_profile = PLAYER_PROFILE_NONE;
        }
      }

      // TODO Handle commands from input
      if (input_state[BTN_EQUIP][BUTTON_CHANGED] == 1 && input_state[BTN_EQUIP][BUTTON_STATE] == BUTTON_DOWN) {
        player_profile = PLAYER_PROFILE_MELEE;
        player_profile_ticks = 600;

        toast_message1 = "Command";
        toast_message2 = "melee";
        toast_message_ticks1 = 6;
        toast_message_ticks2 = 6;
      }
      if (input_state[BTN_MAGIC][BUTTON_CHANGED] == 1 && input_state[BTN_MAGIC][BUTTON_STATE] == BUTTON_DOWN) {
        player_profile = PLAYER_PROFILE_RANGED;
        player_profile_ticks = 600;
        
        toast_message1 = "Command";
        toast_message2 = "ranged";
        toast_message_ticks1 = 6;
        toast_message_ticks2 = 6;
      }
      if (input_state[BTN_ITEM][BUTTON_CHANGED] == 1 && input_state[BTN_ITEM][BUTTON_STATE] == BUTTON_DOWN) {
        toast_message1 = "Command";
        toast_message2 = "item";
        toast_message_ticks1 = 6;
        toast_message_ticks2 = 6;
      }
      if (input_state[BTN_FLEE][BUTTON_CHANGED] == 1 && input_state[BTN_FLEE][BUTTON_STATE] == BUTTON_DOWN) {
        toast_message1 = "Command";
        toast_message2 = "flee";
        toast_message_ticks1 = 6;
        toast_message_ticks2 = 6;
      }

      // Clear effects
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          room_effect_tilemap[i][j] = -1;
        }
      }

      // Regen tick
      if (counter % 4 == 0) {
        // Regen player
        process_regen_tick(entities, ENTITY_ID_PLAYER);
      }
      if (counter % 8 == 0) {
        // Regen entities
        for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
          process_regen_tick(entities, entity_id);
        }
      }

      // Setup new room if entered
      if (new_room) {
        // Write save data
        preferences.begin("cyberpet", false);
        preferences.putInt("world_t", world_time);
        preferences.putUInt("plr_l", player_level);
        preferences.putUInt("plr_max_l", player_max_level);
        preferences.putULong64("plr_xp", player_exp);
        preferences.putUInt("plr_xp_x", player_exp_multiplier);
        preferences.putInt("level_ups", level_ups);
        preferences.putULong64("player_gp", player_gold);
        preferences.putInt("world_x", world_x);
        preferences.putInt("world_y", world_y);
        preferences.putInt("area_x", area_x);
        preferences.putInt("area_y", area_y);
        preferences.putUInt("room_x", room_x);
        preferences.putUInt("room_y", room_y);
        preferences.end();

        // Get current world tile and area
        get_world_tile(world_tile_data, world_x, world_y, world_time, content_seed);

        // Add area exits to room exits on edges
        area_exits = get_area_exits(world_x, world_y);
        room_exits = get_room_exits(area_x, area_y);
        if (area_x == 1 && area_y == 0) { room_exits = room_exits | area_exits; }
        else if (area_x == 2 && area_y == 1) { room_exits = room_exits | area_exits; }
        else if (area_x == 1 && area_y == 2) { room_exits = room_exits | area_exits; }
        else if (area_x == 0 && area_y == 1) { room_exits = room_exits | area_exits; }

        // Setup player visible room
        setup_room(room_wallmap, room_tilemap, world_x, world_y, world_tile_data, area_x, area_y, room_exits, content_seed);

        // Setup player navigation maps
        build_dijkstra_map(room_exitn_navmap, room_wallmap, 4, 0, true);
        build_dijkstra_map(room_exits_navmap, room_wallmap, 4, MAP_H - 1, true);
        build_dijkstra_map(room_exitw_navmap, room_wallmap, 0, 7, true);
        build_dijkstra_map(room_exite_navmap, room_wallmap, MAP_W - 1, 7, true);

        clear_dijkstra_map(room_exit_navmap);
        if (room_exits & EXIT_N) { merge_dijkstra_maps(room_exit_navmap, room_exitn_navmap); }
        if (room_exits & EXIT_S) { merge_dijkstra_maps(room_exit_navmap, room_exits_navmap); }
        if (room_exits & EXIT_W) { merge_dijkstra_maps(room_exit_navmap, room_exitw_navmap); }
        if (room_exits & EXIT_E) { merge_dijkstra_maps(room_exit_navmap, room_exite_navmap); }

        // Get entity dead bitmask for room
        tile_index = squirrel_4d(world_x, world_y, area_x, area_y, content_seed) % WORLD_DEAD_SIZE;
        world_tile_dead_value = 0;
        if (world_tile_dead_ttl[tile_index] > 0) {
          world_tile_dead_value = world_tile_dead[tile_index];
        }

        // Setup entities and update navigation map leading to them
        clear_dijkstra_map(room_entity_navmap);
        for (int i = 0; i < MAP_W; i++) {
          for (int j = 0; j < MAP_H; j++) {
            room_entity_idmap[i][j] = 0;
          }
        }
        setup_room_entities(
          entities,
          room_wallmap,
          room_exit_navmap,
          room_entity_navmap,
          room_entity_idmap,
          world_x,
          world_y,
          world_tile_data,
          area_x,
          area_y,
          world_tile_dead_value,
          content_seed
        );
        build_dijkstra_map(room_entity_navmap, room_wallmap, false);
        update_entity_navmap = false;

        // Setup upkeep
        area_dir = 0;
        ai_room_dir = 0;

        toast_message1 = "  Room";
        toast_message2 = " lvl" + format_number3(world_tile_data[TILE_LEVEL]);
        toast_message_ticks1 = 6;
        toast_message_ticks2 = 6;

        new_room = false;
      }

      // Update entities navigation map if needed
      if (update_entity_navmap) {
        clear_dijkstra_map(room_entity_navmap);
        for (int i = 0; i < MAP_W; i++) {
          for (int j = 0; j < MAP_H; j++) {
            room_entity_idmap[i][j] = 0;
          }
        }

        for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
          if (entities[entity_id][ENTITY_ALIVE] == 1) {
            check_entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
            check_entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];

            room_entity_navmap[check_entity_x][check_entity_y] = 0;
            room_entity_idmap[check_entity_x][check_entity_y] = entity_id;
          }
        }

        build_dijkstra_map(room_entity_navmap, room_wallmap, false);
        update_entity_navmap = false;
      }

      // Player movement/action
      if (ai_active && counter % 2 == 0) {
        player_prev_hp = entities[ENTITY_ID_PLAYER][ENTITY_HP];
        player_prev_sp = entities[ENTITY_ID_PLAYER][ENTITY_SP];
        player_prev_exp = player_exp;
        player_prev_gold = player_gold;

        // TODO World tile target navigation, try to go to tile with equal level

        // TODO Area room target navigation, clear all rooms in area
        // Decide next room direction if not selected
        if (area_dir == 0) {
          int rand_dir = 1 + (squirrel_2d(world_x, world_y, action_seed + counter) % 4);
          if (rand_dir == 1) { area_dir = DIR_N; }
          else if (rand_dir == 2) { area_dir = DIR_E; }
          else if (rand_dir == 3) { area_dir = DIR_S; }
          else if (rand_dir == 4) { area_dir = DIR_W; }

          if (! (room_exits & area_dir) || last_door & area_dir) {
            // Selected door does not exist in room or leads to last visited room
            area_dir = 0;
          }
        }

        // Navigate player in current room
        target_distance = 1;
        if (player_profile == PLAYER_PROFILE_NONE) {
          // No profile active, select based on equipment
          equipment_id = entities[ENTITY_ID_PLAYER][ENTITY_ITEM1];

          switch(equipments[equipment_id][EQUIP_TYPE]) {
            case EQUIP_TYPE_UNARMED:
            case EQUIP_TYPE_WEAPON:
              target_distance = 2;
            break;
            case EQUIP_TYPE_RANGED:
              target_distance = 2;
            break;
            case EQUIP_TYPE_MAGIC:
              if (squirrel(ENTITY_ID_PLAYER, action_seed + counter + 49) % 3 == 0) {
                target_distance = 1;
              } else {
                target_distance = 2;
              }
            break;
          }
        } else if (player_profile == PLAYER_PROFILE_RANGED) {
          // Ranged profile active, stay at distance
          target_distance = 2;
        }
        
        ai_room_dir = 0;
        if ( 
          room_entity_navmap[4][0] < DIJKSTRA_MAX
          || room_entity_navmap[4][MAP_H - 2] < DIJKSTRA_MAX
          || room_entity_navmap[0][7] < DIJKSTRA_MAX
          || room_entity_navmap[MAP_W - 2][7] < DIJKSTRA_MAX
        ) {
          if (entities[ENTITY_ID_PLAYER][ENTITY_HP] > 2 * (get_entity_max_hp(entities, ENTITY_ID_PLAYER) / 3)) {
            // Entities in room, try to visit them
            ai_room_dir = get_dijkstra_direction(room_entity_navmap, room_x, room_y, target_distance, action_seed + counter);
          } else if (last_door != 0 && entities[ENTITY_ID_PLAYER][ENTITY_HP] < get_entity_max_hp(entities, ENTITY_ID_PLAYER) / 5) {
            // Entities in room but really health low, flee to previous room
            if (last_door == DIR_N) { ai_room_dir = get_dijkstra_direction(room_exitn_navmap, room_x, room_y, action_seed + counter); }
            else if (last_door == DIR_S) { ai_room_dir = get_dijkstra_direction(room_exits_navmap, room_x, room_y, action_seed + counter); }
            else if (last_door == DIR_W) { ai_room_dir = get_dijkstra_direction(room_exitw_navmap, room_x, room_y, action_seed + counter); }
            else if (last_door == DIR_E) { ai_room_dir = get_dijkstra_direction(room_exite_navmap, room_x, room_y, action_seed + counter); }
          } else {
            // Entities in room but health low, flee
            ai_room_dir = get_dijkstra_direction(room_entity_navmap, room_x, room_y, 5, action_seed + counter);
          }
        } else {
          if (entities[ENTITY_ID_PLAYER][ENTITY_HP] == get_entity_max_hp(entities, ENTITY_ID_PLAYER)) {
            // No entities in room and health full, move to next room
            if (area_dir == DIR_N) { ai_room_dir = get_dijkstra_direction(room_exitn_navmap, room_x, room_y, action_seed + counter); }
            else if (area_dir == DIR_S) { ai_room_dir = get_dijkstra_direction(room_exits_navmap, room_x, room_y, action_seed + counter); }
            else if (area_dir == DIR_W) { ai_room_dir = get_dijkstra_direction(room_exitw_navmap, room_x, room_y, action_seed + counter); }
            else if (area_dir == DIR_E) { ai_room_dir = get_dijkstra_direction(room_exite_navmap, room_x, room_y, action_seed + counter); }
          }
        }

        if (ai_room_dir == DIR_N) { room_y--; update_player_navmap = true; }
        else if (ai_room_dir == DIR_S) { room_y++; update_player_navmap = true; }
        else if (ai_room_dir == DIR_W) { room_x--; update_player_navmap = true; }
        else if (ai_room_dir == DIR_E) { room_x++; update_player_navmap = true; }

        entities[ENTITY_ID_PLAYER][ENTITY_ROOM_X] = room_x;
        entities[ENTITY_ID_PLAYER][ENTITY_ROOM_Y] = room_y;
      }

      // Update player navigation map if needed
      if (update_player_navmap) {
        clear_dijkstra_map(room_player_navmap);
        room_player_navmap[room_x][room_y] = 0;
        build_dijkstra_map(room_player_navmap, room_wallmap, false);
        update_player_navmap = false;
      }

      // Scan melee combat targets and attack
      should_combat = true && ai_active;
      combat_result = 0;
      level_ups = 0;
      for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
        if (should_combat && entities[entity_id][ENTITY_ALIVE] == 1) {
          check_entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
          check_entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];
          entity_distance = room_player_navmap[check_entity_x][check_entity_y];

          if (entity_distance == 0 || entity_distance == 1) {
            combat_result = resolve_combat(
              entities,
              ENTITY_ID_PLAYER,
              entity_id,
              true,
              room_effect_tilemap,
              action_seed + counter + (unsigned int)entity_id
            );

            if (combat_result == 1) {
              // Enemy died

              // Update entity dead bitmask for room
              tile_index = squirrel_4d(world_x, world_y, area_x, area_y, content_seed) % WORLD_DEAD_SIZE;
              world_tile_dead_value = 0;
              if (world_tile_dead_ttl[tile_index] > 0) {
                world_tile_dead_value = world_tile_dead[tile_index];
              }
              entity_index = (byte)1 << (byte)entity_id;
              world_tile_dead[tile_index] = world_tile_dead_value | entity_index;
              world_tile_dead_ttl[tile_index] = 255;

              // Handle exp and leveling
              level_ups = gain_exp(
                (int)entities[entity_id][ENTITY_LEVEL],
                &player_level,
                &player_exp,
                &player_exp_multiplier,
                action_seed + counter
              );

              // Add gold
              player_gold = player_gold + entities[entity_id][ENTITY_LEVEL];

              update_entity_navmap = true;
            }

            should_combat = false;
          }
        }
      }

      // Scan ranged combat targets if there was no melee attack
      // TODO Allow ranged attacks only if player has ranged equipment?
      if (should_combat) {
        for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
          if (should_combat && entities[entity_id][ENTITY_ALIVE] == 1) {
            check_entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
            check_entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];
            entity_distance = room_player_navmap[check_entity_x][check_entity_y];

            if (entity_distance == 2) {
              combat_result = resolve_combat(
                entities,
                ENTITY_ID_PLAYER,
                entity_id,
                true,
                room_effect_tilemap,
                action_seed + counter + (unsigned int)entity_id
              );

              if (combat_result == 1) {
                // Enemy died

                // Update entity dead bitmask for room
                tile_index = squirrel_4d(world_x, world_y, area_x, area_y, content_seed) % WORLD_DEAD_SIZE;
                world_tile_dead_value = 0;
                if (world_tile_dead_ttl[tile_index] > 0) {
                  world_tile_dead_value = world_tile_dead[tile_index];
                }
                entity_index = (byte)1 << (byte)entity_id;
                world_tile_dead[tile_index] = world_tile_dead_value | entity_index;
                world_tile_dead_ttl[tile_index] = 255;

                // Handle exp and leveling
                level_ups = gain_exp(
                  (int)entities[entity_id][ENTITY_LEVEL],
                  &player_level,
                  &player_exp,
                  &player_exp_multiplier,
                  action_seed + counter
                );

                // Add gold
                player_gold = player_gold + entities[entity_id][ENTITY_LEVEL];

                update_entity_navmap = true;
              }

              should_combat = false;
            }
          }
        }
      }

      // Check to display messages
      if (level_ups > 0) {
        // Player leveled up
        entities[ENTITY_ID_PLAYER][ENTITY_LEVEL] = player_level;
        update_entity_stats(entities, ENTITY_ID_PLAYER);

        if (player_level > player_max_level) {
          player_max_level = player_level;
        }

        toast_message1 = "Level up";
        toast_message2 = "  " + format_number3(player_level);
        toast_message_ticks1 = 10;
        toast_message_ticks2 = 10;
      } else if (combat_result == 1) {
        player_diff_exp = player_exp - player_prev_exp;
        player_diff_gold = player_gold - player_prev_gold;

        toast_message1 = "  Kill";
        toast_message2 = format_number3(player_diff_exp) + "XP" + format_number2(player_diff_gold) + "G";
        toast_message_ticks1 = 4;
        toast_message_ticks2 = 4;
      }

      // Entity movement/action
      if (ai_active && counter % 2 == 0) {
        for (int entity_id = 1; entity_id < ENTITY_SIZE; entity_id++) {
          // Update AI state
          int state_update_result = update_ai_state(
            entities,
            entity_id,
            room_entity_navmap,
            room_player_navmap,
            room_entity_idmap,
            world_tile_data,
            action_seed + counter + (unsigned int)entity_id
          );

          // Run AI state movement
          state_update_result = run_ai_state_movement(
            entities,
            entity_id,
            room_entity_navmap,
            room_player_navmap,
            room_entity_idmap,
            world_tile_data,
            action_seed + counter + (unsigned int)entity_id + 539
          );
          if (state_update_result) { update_entity_navmap = true; }

          // Run AI state action
          state_update_result = run_ai_state_action(
            entities,
            entity_id,
            room_entity_navmap,
            room_player_navmap,
            room_entity_idmap,
            room_effect_tilemap,
            world_tile_data,
            action_seed + counter + (unsigned int)entity_id + 591
          );
        }
      }

      // Check if player died from entity combat
      if (entities[ENTITY_ID_PLAYER][ENTITY_ALIVE] == 0) {
        player_dead = true;
      }

      // Update entity sprite map
      // TODO Only update if someone has moved/died/etc
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          room_entity_tilemap[i][j] = -1;
        }
      }
      for (int entity_id = 0; entity_id < ENTITY_SIZE; entity_id++) {
        if (entities[entity_id][ENTITY_ALIVE] == 1) {
          check_entity_x = (int)entities[entity_id][ENTITY_ROOM_X];
          check_entity_y = (int)entities[entity_id][ENTITY_ROOM_Y];

          room_entity_tilemap[check_entity_x][check_entity_y] = (int)entities[entity_id][ENTITY_ICON];
        }
      }

      // Draw room
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          tile_pixel = 0;
          if (room_effect_tilemap[i][j] != -1) {
            // Draw effect
            tile = room_effect_tilemap[i][j];
          } else if (room_entity_tilemap[i][j] != -1) {
            // Draw entity
            tile = room_entity_tilemap[i][j];
            tile_pixel = (((j * 9) + i) + counter) % 2 == 0;
          } else {
            // Draw room tile
            tile = room_tilemap[i][j];
          }

          if (tile != -1) {
            display1.drawBitmap(i * 8, (j * 8) - tile_pixel, tiles[tile], 8, 8, WHITE);
          }
        }
      }

      // Check for door
      if (room_y == 0) {
        // North
        room_y = MAP_H - 2;
        last_door = EXIT_S;
        new_room = true;

        area_y--;
        if (area_y < 0) {
          area_y = 2;
          world_y--;
        }
      } else if (room_y == MAP_H - 1) {
        // South
        room_y = 1;
        last_door = EXIT_N;
        new_room = true;

        area_y++;
        if (area_y > 2) {
          area_y = 0;
          world_y++;
        }
      } else if (room_x == 0) {
        // West
        room_x = MAP_W - 2;
        last_door = EXIT_E;
        new_room = true;

        area_x--;
        if (area_x < 0) {
          area_x = 2;
          world_x--;
        }
      } else if (room_x == MAP_W - 1) {
        // East
        room_x = 1;
        last_door = EXIT_W;
        new_room = true;

        area_x++;
        if (area_x > 2) {
          area_x = 0;
          world_x++;
        }
      }

      if (player_dead) {
        // Adventurer died
        player_dead = false;
        player_dead_setup = false;
        counter_at = counter + 50;

        game_state = STATE_DEATH;
      }

      entities[ENTITY_ID_PLAYER][ENTITY_ROOM_X] = room_x;
      entities[ENTITY_ID_PLAYER][ENTITY_ROOM_Y] = room_y;

      // Tick upkeep
      if (counter % 10 == 0) {
        for (tile_index = 0; tile_index < WORLD_DEAD_SIZE; tile_index++) {
          if (world_tile_dead_ttl[tile_index] > 0) {
            world_tile_dead_ttl[tile_index]--;
          }
        }
      }

      // Draw status bars
      display1.setCursor(0, 120);
      if (toast_message_ticks1 > 0) {
        // Message active, display it
        display1.println(toast_message1);
      } else {
        // No message, display health
        display1.println(format_number4(entities[ENTITY_ID_PLAYER][ENTITY_HP]));
        
        player_diff_hp = (long long)entities[ENTITY_ID_PLAYER][ENTITY_HP] - (long long)player_prev_hp;
        display1.setCursor(4 * 8, 120);

        if (player_diff_hp < 0) {
          display1.println("-" + format_number3(abs(player_diff_hp)));
        } else if (player_diff_hp > 0) {
          display1.println("+" + format_number3(player_diff_hp));
        } else {
          display1.println("/" + format_number3(get_entity_max_hp(entities, ENTITY_ID_PLAYER)));
        }
      }

      if (toast_message_ticks2 > 0) {
        // Message active, display it
        display1.println(toast_message2);
      } else {
        // No message, display skill points
        display1.setCursor(0, 128);
        display1.println(format_number3(player_level) + " <" + format_number3(player_max_level));
        //display1.println(format_number4(entities[ENTITY_ID_PLAYER][ENTITY_SP]) + "/" + format_number3(get_entity_max_sp(entities, ENTITY_ID_PLAYER)));
      }

      if (toast_message_ticks1 > 0) { toast_message_ticks1--; }
      if (toast_message_ticks2 > 0) { toast_message_ticks2--; }

      break; // STATE_ROOM
    case STATE_WORLDMAP:
      //int ok = get_simple_noisemap(noisemap2d, 0, counter, MAP_W, MAP_H, content_seed, 0.3);
      //int ok = get_simple_noisemap3d(noisemap2d, 0, counter, 0, MAP_W, MAP_H, content_seed, 0.3);

      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          //value = noisemap2d[i][j];
          if (i == 4 && j == 7) {
            // Draw player
            tile = 326;
          } else {
            // Draw map tile
            get_world_tile(world_tile_data, i + (world_x - 4), j + (world_y - 7), 0, content_seed);
            tile = get_world_draw_tile(world_tile_data);
          }

          display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
        }
      }

      break; // STATE_WORLDMAP
    case STATE_GAMEMENU:
      break; // STATE_GAMEMENU
    case STATE_DEATH:
      if (player_dead_setup == false) {
        // Advance world clock
        world_time++;

        // Run one time setup for death screen
        player_level = 1;
        player_exp = 0;
        setup_player_entity(entities, player_level);

        world_x = 0;
        world_y = 0;
        area_x = 1;
        area_y = 1;
        room_x = 4;
        room_y = 7;
        last_door = 0;

        for (tile_index = 0; tile_index < WORLD_DEAD_SIZE; tile_index++) {
          world_tile_dead_ttl[tile_index] = 0;
        }
        toast_message_ticks1 = 0;
        toast_message_ticks2 = 0;

        new_room = true;

        // TODO Handle bonus rolls with gold instead of static bonus
        player_exp_multiplier++;

        player_dead_setup = true;
      }

      display1.setCursor(0, 8);
      display1.println("You died");

      display1.drawBitmap(2 * 8, 3 * 8, tiles[1018], 8, 8, WHITE);
      display1.drawBitmap(2 * 8, 4 * 8, tiles[1427], 8, 8, WHITE);
      display1.drawBitmap(3 * 8, 4 * 8, tiles[117], 8, 8, WHITE);
      display1.drawBitmap(5 * 8, 4 * 8, tiles[entities[ENTITY_ID_PLAYER][ENTITY_ICON]], 8, 8, WHITE);

      display1.setCursor(0, 7 * 8);
      display1.println("XP");
      display1.setCursor(0, 8 * 8);
      display1.println("bonus +1");

      display1.setCursor(0, 10 * 8);
      display1.println("Now X" + format_number3(player_exp_multiplier));

      display1.setCursor(0, 128);
      display1.println("Go again");

      if (counter >= counter_at) {
        // Death screen timer ran out, clear stuff and start again
        player_gold = 0;

        game_state = STATE_ROOM;
      }
      
      break; // STATE_DEATH
  }

  // write the buffer to the display
  display1.display();

  //delay(100);
  esp_sleep_enable_timer_wakeup(100000);
  esp_light_sleep_start();
}

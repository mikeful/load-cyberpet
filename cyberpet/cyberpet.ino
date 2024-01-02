#include "Arduino.h"
//#include "heltec.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lora32_pins.h"

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

int seed = 0;
byte game_state = STATE_START;
unsigned int player_level = 1;
unsigned int player_exp = 0;
unsigned int player_exp_multiplier = 1;
int level_ups = 0;

unsigned int tile = 0;
int world_x = 0;
int world_y = 0;
int area_x = 1; // 3x3 rooms in world tile
int area_y = 1;
unsigned int room_x = 4; // Player position in room
unsigned int room_y = 7;
unsigned int entity_x = 0;
unsigned int entity_y = 0;

int ai_world_dir = 0;
int ai_room_dir = 0;
byte last_door = 0;

bool new_room = true;
byte room_wallmap[MAP_W][MAP_H];
int room_tilemap[MAP_W][MAP_H];
int room_entity_tilemap[MAP_W][MAP_H];

byte room_exits = 0;
byte area_exits = 0;
int room_exit_navmap[MAP_W][MAP_H];
int room_exitn_navmap[MAP_W][MAP_H];
int room_exite_navmap[MAP_W][MAP_H];
int room_exits_navmap[MAP_W][MAP_H];
int room_exitw_navmap[MAP_W][MAP_H];

unsigned int entities[ENTITY_SIZE][ENTITY_ATTRS];
int room_player_navmap[MAP_W][MAP_H];
int room_entity_navmap[MAP_W][MAP_H];
bool update_entity_navmap = false;

int noisemap[MAP_W][MAP_H];
int world_tile_data[15];

unsigned int counter = 1;

// Battery stuff
int volt = 0;
int volt2 = 0;
int progress = 0;
float XS = 0.0025;

Adafruit_SSD1306 display1(128, 64, &Wire, 21, 500000UL);

void setup() {
  // Setup display
  Wire.begin(17, 18);
  display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display1.setRotation(1);

  display1.clearDisplay();
  display1.display();

  // Setup 8x8 bitmap font
  display1.setFont(&medievalish_chonker_mono_Font);
  display1.setTextSize(1);
  display1.setTextColor(SSD1306_WHITE);
  display1.setCursor(0, 0);
  display1.setTextWrap(false);
  //display1.cp437(true);

  // Setup battery
  pinMode(ADC_Ctrl, OUTPUT); // pin 37
  pinMode(VBAT_Read, INPUT); // pin 1
  adcAttachPin(VBAT_Read); // default ADC_11db
  analogReadResolution(12);

  //Serial.begin(115200);

  // Setup game
  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      room_wallmap[i][j] = 0;
      room_tilemap[i][j] = 0;
    }
  }
}

void loop() {
  counter++;

  // clear the display
  display1.clearDisplay();

  /*
  // Battery stuff
  // ADC resolution
  const int resolution = 12;
  const int adcMax = pow(2, resolution) - 1;
  const float adcMaxVoltage = 3.3;
  // On-board voltage divider
  const int R1 = 390;
  const int R2 = 100;
  // Calibration measurements
  const float measuredVoltage = 4.2;
  const float reportedVoltage = 4.095;
  // Calibration factor
  const float factor = (adcMaxVoltage / adcMax) * ((R1 + R2)/(float)R2) * (measuredVoltage / reportedVoltage);
  digitalWrite(ADC_Ctrl, LOW);
  delay(10);
  int analogValue = analogRead(VBAT_Read);
  digitalWrite(ADC_Ctrl, HIGH);

  float floatVoltage = factor * analogValue;
  uint16_t voltage = (int)(floatVoltage * 1000.0);

  progress = int((floatVoltage / 4.2) * 100);
  display1->drawProgressBar(1, 64, 62, 10, progress);
  display1->setTextAlignment(TEXT_ALIGN_CENTER);
  display1->drawString(32, 15, String(progress) + "%");
  display1->setTextAlignment(TEXT_ALIGN_CENTER);
  display1->drawString(32, 5, String(floatVoltage) + "V");
  */

  switch(game_state) {
    case STATE_START:
      setup_player_entity(entities, player_level);
      game_state = STATE_ROOM;

      break; // STATE_START
    case STATE_MAINMENU:
      break; // STATE_MAINMENU
    case STATE_ROOM:
      // Setup new room if entered
      if (new_room) {
        // Get current world tile and area
        get_world_tile(world_tile_data, world_x, world_y, 0, seed);

        // Add area exits to room exits on edges
        area_exits = get_area_exits(world_x, world_y);
        room_exits = get_room_exits(area_x, area_y);
        if (area_x == 1 && area_y == 0) { room_exits = room_exits | area_exits; }
        if (area_x == 2 && area_y == 1) { room_exits = room_exits | area_exits; }
        if (area_x == 1 && area_y == 2) { room_exits = room_exits | area_exits; }
        if (area_x == 0 && area_y == 1) { room_exits = room_exits | area_exits; }

        // Setup player visible room
        setup_room(room_wallmap, room_tilemap, world_x, world_y, world_tile_data, area_x, area_y, room_exits, seed);

        // Setup player navigation maps
        build_djikstra_map(room_exitn_navmap, room_wallmap, 4, 0);
        build_djikstra_map(room_exits_navmap, room_wallmap, 4, MAP_H - 1);
        build_djikstra_map(room_exitw_navmap, room_wallmap, 0, 7);
        build_djikstra_map(room_exite_navmap, room_wallmap, MAP_W - 1, 7);

        clear_djikstra_map(room_exit_navmap);
        if (room_exits & EXIT_N) {
          merge_djikstra_maps(room_exit_navmap, room_exitn_navmap);
        }
        if (room_exits & EXIT_S) {
          merge_djikstra_maps(room_exit_navmap, room_exits_navmap);
        }
        if (room_exits & EXIT_W) {
          merge_djikstra_maps(room_exit_navmap, room_exitw_navmap);
        }
        if (room_exits & EXIT_E) {
          merge_djikstra_maps(room_exit_navmap, room_exite_navmap);
        }

        // Setup entities and update navigation map leading to them
        clear_djikstra_map(room_entity_navmap);
        setup_room_entities(entities, room_wallmap, room_exit_navmap, room_entity_navmap, world_x, world_y, world_tile_data, area_x, area_y, seed);
        build_djikstra_map(room_entity_navmap, room_wallmap);

        // Setup upkeep
        ai_world_dir = 0;
        ai_room_dir = 0;

        new_room = false;
      }

      // Update entities navigation map if needed
      if (update_entity_navmap) {
        clear_djikstra_map(room_entity_navmap);

        for (int entity_id = 1; entity_id < 8; entity_id++) {
          if (entities[entity_id][ENTITY_ALIVE] == 1) {
            entity_x = entities[entity_id][ENTITY_ROOM_X];
            entity_y = entities[entity_id][ENTITY_ROOM_Y];

            room_entity_navmap[entity_x][entity_y] = 0;
          }
        }

        build_djikstra_map(room_entity_navmap, room_wallmap);
        update_entity_navmap = false;
      }

      if (counter % 2 == 0) {
        // Decide next direction
        if (ai_world_dir == 0) {
          int rand_dir = 1 + (squirrel_2d(world_x, world_y, seed + counter) % 4);
          if (rand_dir == 1) { ai_world_dir = DIR_N; }
          if (rand_dir == 2) { ai_world_dir = DIR_E; }
          if (rand_dir == 3) { ai_world_dir = DIR_S; }
          if (rand_dir == 4) { ai_world_dir = DIR_W; }

          if (! (room_exits & ai_world_dir) || last_door & ai_world_dir) {
            // Selected door does not exist in room or leads to last visited room
            ai_world_dir = 0;
          }
        }

        // Navigate player in current room
        if (room_entity_navmap[4][7] < DJIKSTRA_MAX) {
          // Entities in room, try to visit them
          ai_room_dir = get_djikstra_direction(room_entity_navmap, room_x, room_y, seed + counter);
        } else {
          // No entities in room, move to next room
          if (ai_world_dir == DIR_N) { ai_room_dir = get_djikstra_direction(room_exitn_navmap, room_x, room_y, seed + counter); }
          if (ai_world_dir == DIR_S) { ai_room_dir = get_djikstra_direction(room_exits_navmap, room_x, room_y, seed + counter); }
          if (ai_world_dir == DIR_W) { ai_room_dir = get_djikstra_direction(room_exitw_navmap, room_x, room_y, seed + counter); }
          if (ai_world_dir == DIR_E) { ai_room_dir = get_djikstra_direction(room_exite_navmap, room_x, room_y, seed + counter); }
        }

        if (ai_room_dir == DIR_N) { room_y--; }
        if (ai_room_dir == DIR_S) { room_y++; }
        if (ai_room_dir == DIR_W) { room_x--; }
        if (ai_room_dir == DIR_E) { room_x++; }

        entities[ENTITY_ID_PLAYER][ENTITY_ROOM_X] = room_x;
        entities[ENTITY_ID_PLAYER][ENTITY_ROOM_Y] = room_y;
      }

      // Collision/combat
      for (int entity_id = 1; entity_id < 8; entity_id++) {
        if (entities[entity_id][ENTITY_ALIVE] == 1) {
          entity_x = entities[entity_id][ENTITY_ROOM_X];
          entity_y = entities[entity_id][ENTITY_ROOM_Y];

          if (entity_x == room_x && entity_y == room_y) {
            entities[entity_id][ENTITY_ALIVE] = 0;
            update_entity_navmap = true;

            level_ups = gain_exp(entities[entity_id][ENTITY_LEVEL], &player_level, &player_exp, &player_exp_multiplier, seed+counter);
            if (level_ups > 0) {
              entities[ENTITY_ID_PLAYER][ENTITY_LEVEL];
              update_entity_stats(entities, ENTITY_ID_PLAYER);
            }
          }
        }
      }

      // Update entity sprite map
      // TODO Only update if someone has moved/died/etc
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          room_entity_tilemap[i][j] = -1;
        }
      }
      for (int entity_id = 0; entity_id < 8; entity_id++) {
        if (entities[entity_id][ENTITY_ALIVE] == 1) {
          entity_x = entities[entity_id][ENTITY_ROOM_X];
          entity_y = entities[entity_id][ENTITY_ROOM_Y];

          room_entity_tilemap[entity_x][entity_y] = (int)entities[entity_id][ENTITY_ICON];
        }
      }

      // Draw room
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          if (room_entity_tilemap[i][j] != -1) {
            // Draw entity
            tile = room_entity_tilemap[i][j];
          } else {
            // Draw room tile
            tile = room_tilemap[i][j];
          }

          if (tile != -1) {
            display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
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

      entities[ENTITY_ID_PLAYER][ENTITY_ROOM_X] = room_x;
      entities[ENTITY_ID_PLAYER][ENTITY_ROOM_Y] = room_y;

      break; // STATE_ROOM
    case STATE_WORLDMAP:
      //int ok = get_simple_noisemap(noisemap2d, 0, counter, MAP_W, MAP_H, seed, 0.3);
      //int ok = get_simple_noisemap3d(noisemap2d, 0, counter, 0, MAP_W, MAP_H, seed, 0.3);

      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          //value = noisemap2d[i][j];
          if (i == 4 && j == 7) {
            // Draw player
            tile = 326;
          } else {
            // Draw map tile
            get_world_tile(world_tile_data, i + (world_x - 4), j + (world_y - 7), 0, seed);
            tile = get_world_draw_tile(world_tile_data);
          }

          display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
        }
      }

      break; // STATE_WORLDMAP
    case STATE_GAMEMENU:
      break; // STATE_GAMEMENU
  }

  display1.setCursor(0, 120);
  display1.println("X" + String(world_x));
  display1.setCursor(32, 120);
  display1.println("Y" + String(world_y));

  display1.setCursor(0, 128);
  display1.println(format_number3(player_level) + " " + format_number4(player_exp_multiplier));
  //display1.setCursor(32, 128);
  //display1.println(format_number4(counter));

  // write the buffer to the display
  display1.display();

  //delay(100);
  esp_sleep_enable_timer_wakeup(100000);
  esp_light_sleep_start();
}

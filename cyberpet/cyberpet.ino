#include "Arduino.h"
//#include "heltec.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "lora32_pins.h"

#include "toolbox.h"
#include "medievalish_chonker_mono.h"
#include "tileset.h"
#include "noisemap.h"
#include "room.h"

#define STATE_START 0
#define STATE_MAINMENU 1
#define STATE_ROOM 2
#define STATE_WORLDMAP 3
#define STATE_GAMEMENU 4

int seed = 0;
byte game_state = STATE_START;
unsigned int tile = 0;
int world_x = 0;
int world_y = 0;
int area_x = 1; // 3x3 rooms in world tile
int area_y = 1;
int room_x = 4; // Player positon in room
int room_y = 7;

int ai_world_dir = 0;
int ai_room_dir = 0;
byte last_door = 0;

bool new_room = true;
byte room_wallmap[MAP_W][MAP_H];
int room_tilemap[MAP_W][MAP_H];

byte room_exits = 0;
byte area_exits = 0;
int room_exit_map[MAP_W][MAP_H];
int room_exitn_map[MAP_W][MAP_H];
int room_exite_map[MAP_W][MAP_H];
int room_exits_map[MAP_W][MAP_H];
int room_exitw_map[MAP_W][MAP_H];

int noisemap[MAP_W][MAP_H];
int world_tile_data[15];

int map_min = 999;
int map_max = 0;

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
      game_state = STATE_ROOM;

      break; // STATE_START
    case STATE_MAINMENU:
      break; // STATE_MAINMENU
    case STATE_ROOM:
      // Setup new room if entered
      if (new_room) {
        //Serial.println("Start room generation: X" + String(world_x) + " Y" + String(world_y) + ", x" + String(area_x) +  " y" + String(area_y));

        // Get current world tile and area
        get_world_tile(world_tile_data, world_x, world_y, 0, seed);
        area_exits = get_area_exits(world_x, world_y);
        room_exits = get_room_exits(area_x, area_y);
        //room_exits = 15;

        // Add area exits to room exits on edges
        if (area_x == 1 && area_y == 0) { room_exits = room_exits | area_exits; }
        if (area_x == 2 && area_y == 1) { room_exits = room_exits | area_exits; }
        if (area_x == 1 && area_y == 2) { room_exits = room_exits | area_exits; }
        if (area_x == 0 && area_y == 1) { room_exits = room_exits | area_exits; }

        // Setup player visible room
        setup_room(room_wallmap, room_tilemap, world_x, world_y, world_tile_data, area_x, area_y, room_exits, seed);
        build_djikstra_map(room_exitn_map, room_wallmap, 4, 0);
        build_djikstra_map(room_exits_map, room_wallmap, 4, MAP_H - 1);
        build_djikstra_map(room_exitw_map, room_wallmap, 0, 7);
        build_djikstra_map(room_exite_map, room_wallmap, MAP_W - 1, 7);

        clear_djikstra_map(room_exit_map);
        if (room_exits & EXIT_N) {
          merge_djikstra_maps(room_exit_map, room_exitn_map);
        }
        if (room_exits & EXIT_S) {
          merge_djikstra_maps(room_exit_map, room_exits_map);
        }
        if (room_exits & EXIT_W) {
          merge_djikstra_maps(room_exit_map, room_exitw_map);
        }
        if (room_exits & EXIT_E) {
          merge_djikstra_maps(room_exit_map, room_exite_map);
        }

        ai_world_dir = 0;
        ai_room_dir = 0;
        new_room = false;
      }

      if (counter % 5 == 0) {
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
        if (ai_world_dir == DIR_N) { ai_room_dir = get_djikstra_direction(room_exitn_map, room_x, room_y, seed); }
        if (ai_world_dir == DIR_S) { ai_room_dir = get_djikstra_direction(room_exits_map, room_x, room_y, seed); }
        if (ai_world_dir == DIR_W) { ai_room_dir = get_djikstra_direction(room_exitw_map, room_x, room_y, seed); }
        if (ai_world_dir == DIR_E) { ai_room_dir = get_djikstra_direction(room_exite_map, room_x, room_y, seed); }

        if (ai_room_dir == DIR_N) { room_y--; }
        if (ai_room_dir == DIR_S) { room_y++; }
        if (ai_room_dir == DIR_W) { room_x--; }
        if (ai_room_dir == DIR_E) { room_x++; }
      }

      // Draw room
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          if (i == room_x && j == room_y) {
            // Draw player
            tile = 326;
            display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
          } else {
            // Draw room tile
            /*if (room_exit_map[i][j] < 30) {
              display1.setCursor(i * 8, 8 + (j * 8));
              display1.println(String(min(room_exitn_map[i][j], 15), HEX));
            } else {*/
              tile = room_tilemap[i][j];
              display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
            /*}*/
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
  display1.println(String(world_tile_data[TILE_DENSITY]));
  display1.setCursor(32, 128);
  display1.println(format_number(counter));

  // write the buffer to the display
  display1.display();

  //delay(100);
  esp_sleep_enable_timer_wakeup(100000);
  esp_light_sleep_start();
}

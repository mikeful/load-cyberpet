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

byte room_wallmap[MAP_W][MAP_H];
int room_tilemap[MAP_W][MAP_H];

byte room_exits = 0;
int room_exit_map[MAP_W][MAP_H];
int room_exitn_map[MAP_W][MAP_H];
int room_exite_map[MAP_W][MAP_H];
int room_exits_map[MAP_W][MAP_H];
int room_exitw_map[MAP_W][MAP_H];

int noisemap[MAP_W][MAP_H];
int world_tile_data[15];

int map_min = 999;
int map_max = 0;

int counter = 1;

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

  setup_room(room_wallmap, room_tilemap, 0, 0, world_tile_data, 0, 0, 15, 0);
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

  // Map stuff
  //int ok = get_simple_noisemap(noisemap2d, 0, counter, MAP_W, MAP_H, 0, 0.3);
  //int ok = get_simple_noisemap3d(noisemap2d, 0, counter, 0, MAP_W, MAP_H, 0, 0.3);

  if (counter % 50 == 0) {
    room_exits = get_room_exits(0, 0);
    setup_room(room_wallmap, room_tilemap, 0, 0, world_tile_data, 0, 0, room_exits, counter);
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
  }

  int value = 0;
  unsigned int tile = 0;

  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      //tile = room_tilemap[i][j];
      //display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);

      if (room_exit_map[i][j] < 50) {
        display1.setCursor(i * 8, 8 + (j * 8));
        display1.println(String(min(room_exit_map[i][j], 15), HEX));
      } else {
        tile = room_tilemap[i][j];
        //display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
      }
    }
  }

  for (int i = 0; i < MAP_W; i++) {
    for (int j = 0; j < MAP_H; j++) {
      //value = noisemap2d[i][j];
      get_world_tile(world_tile_data, i + 0, j + counter, 0, 0);
      
      value = world_tile_data[TILE_HEIGHT_VALUE];
      if (value > map_max) { map_max = value; }
      if (value < map_min) { map_min = value; }

      tile = get_world_draw_tile(world_tile_data);

      display1.drawBitmap(i * 8, j * 8, tiles[tile], 8, 8, WHITE);
    }
  }

  display1.setCursor(0, 120);
  display1.println(">" + String(map_min));
  display1.setCursor(32, 120);
  display1.println("<" + String(map_max));

  display1.setCursor(0, 128);
  display1.println(format_number(counter));

  // write the buffer to the display
  display1.display();

  //delay(100);
  esp_sleep_enable_timer_wakeup(100000);
  esp_light_sleep_start();
}

# Legend Of the Adventure Dude

Have you achieved everything with your other virtual pets? Tired of grinding endless progression systems in bigger games? No problem. Here is Legend Of the Adventure Dude (LOAD), autoplaying roguelike virtual pet with incremental game mechanics. It grinds for you!

This is entry for [Cyberdeck Cafe](https://cyberdeck.cafe/) cyberpet contest of 2023 and placed 2nd. I dumped bunch of small gamedev and procedural generation ideas/experiments into this and focused mostly on the software side. Enemy stats will scale faster than your characters so at some point they will get you. Death resets your pet to level 1 and back to coordinates 0,0 but you will keep your experience point multiplier.


## Software

1. Get Heltec LoRa 32 V3 devboard.
2. Install Heltec Arduino stuff following instructions here https://github.com/HelTecAutomation/Heltec_ESP32. I think this is correct one. When I started I installed probably 3 different toolchains/plugins various ways and one of them worked. It's a mess.
3. Open new project in Arduino IDE and figure out devboard programming settings.
4. Get code from this repo.
5. Get 8x8 1bit graphics and font pack from here https://dennisbusch-de.itch.io/0xdbs-gasp-8x8-1bpp-rpg-icons-personal. You can also support great artist and buy commercial license for the pack https://dennisbusch-de.itch.io/0xdbs-gasp-8x8-1bpp-rpg-icons.
6. If you got personal version of the pack, you need get the font separately from here https://dennisbusch-de.itch.io/0xdbs-medievalish-chonker-8x8-1bpp-bitmap-font.
7. Next we'll convert graphics pack tilesheet for Arduino. Install Python dependencies in `tileset/` directory (Python virtual environment/venv recommended). Copy `0xDB_GASP_8x8_1bpp_RPG_icons_v18_packed_1bpp.png` and `0xDB_GASP_8x8_1bpp_RPG_icons_v18_packed.csv` files to same directory (change version number if there is update) and run `python convert.py > tileset.h`.
8. Check generated file against `tileset/tileset-example.h`. If starts of the files look same, copy `tileset.h` to `cyberpet/` directory.
9. Convert personal version of the font: copy `0xDB_medievalish_chonker_8x8_1bpp_bmp_font_packed_1bpp.png` and `0xDB_medievalish_chonker_8x8_1bpp_bmp_font_packed.csv` files to `font/` directory and run `python convert_font.py > medievalish_chonker_mono.h`.
10. Check generated file against `font/medievalish_chonker_mono-example.h`. If starts of the files look same, rename the file as `medievalish_chonker_mono.h` and copy it into to `cyberpet/` directory.
11. Open the project `cyberpet/` directory in Arduino IDE. Select 80Mhz CPU frequency to save bit of battery and select LoRa settings for your region.
12. Hit upload and hope for the best. If everything worked, you should see adventure dude running around on you devboard screen.


## Hardware

Coming soon.


## Other stuff/thanks

Useful links I found during the project.

* http://digitalconcepts.net.au/arduino/index.php?op=ESP32#wl32v3
* http://community.heltec.cn/t/heltec-lora-32-v3-nightmares/12133/10
* https://www.rosietheredrobot.com/2019/04/lora-wan-kenobi.html
* https://github.com/meshtastic/firmware/blob/master/variants/heltec_v3/variant.h

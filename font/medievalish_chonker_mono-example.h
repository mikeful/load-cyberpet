// FontEdit, export Arduino, "Reverse bits (MSB)" = true

const unsigned char medievalish_chonker_mono_Bitmap [] PROGMEM = {
  // '0xDB_medievalish, 376x24px
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // Character 0x20 (32: ' ')
  0x18,0x18,0x18,0x10,0x00,0x18,0x18,0x00, // Character 0x21 (33: '!')
  // ...
};

const GFXglyph medievalish_chonker_mono_Glyphs [] PROGMEM = {
  //Index,  W, H,xAdv,dX,dY
  {     0,  8, 8,   8, 0,-8}, // Character 0x20 (32: ' ')
  {     8,  8, 8,   8, 0,-8}, // Character 0x21 (33: '!')
  // ...
};

const GFXfont medievalish_chonker_mono_Font PROGMEM = {
  (uint8_t *)medievalish_chonker_mono_Bitmap,
  (GFXglyph *)medievalish_chonker_mono_Glyphs,
  32, // ASCII start
  126, // ASCII stop
  8 // y Advance
};

import csv
from PIL import Image


character_set = Image.open("0xDB_medievalish_chonker_8x8_1bpp_bmp_font_packed_1bpp.png")

total_characters = 0

with open('0xDB_medievalish_chonker_8x8_1bpp_bmp_font_packed.csv', newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')
    all_characters = []

    # Process CSV and print bitmap content table
    print("const unsigned char medievalish_chonker_mono_Bitmap [] PROGMEM = {")
    print("  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Character 0x20 (32: ' ')")

    for row_index, row in enumerate(reader):
        if row_index == 0:
            # Skip title row
            continue
        else:
            # Add character from file

            # Get character data from CSV
            variant = row[0].strip()
            char_code = int(row[1])
            x = int(row[2])
            y = int(row[3])
            width = int(row[4])
            height = int(row[5])

            # Skip variable width characters in CSV
            if variant != "8x8":
                continue

            # Skip space as it is in wrong row on CSV
            if char_code == 32:
                continue

            # Get 8x8 1bit slice of character from character set image
            character_img = character_set.crop((x, y, x + width, y + height))
            character_pixels = character_img.load()

            # Convert row of pixels into bits of byte
            character_bytes = []
            for pixel_y in range(0, height):
                row_byte = 0

                for pixel_x in range(0, width):
                    pixel_value = 1 if character_pixels[7 - pixel_x, pixel_y] > 0 else 0
                    if pixel_value == 1:
                        row_byte |= (1 << pixel_x)

                character_bytes.append('0x{:02x}'.format(row_byte))

        # Set character glyph info
        all_characters.append((
            str(row_index * 8).rjust(3),
            '0x{:02x}'.format(char_code),
            char_code,
            chr(char_code)
        ))
        total_characters += 1

        # Print character bytes and info
        print(
            "  %s, // Character %s (%s: '%s')" %
            (
                ", ".join(character_bytes),
                '0x{:02x}'.format(char_code),
                char_code,
                chr(char_code)
            )
        )

    print("};")
    print()

    # Print bitmap to glyph conversion table
    print("const GFXglyph medievalish_chonker_mono_Glyphs [] PROGMEM = {")
    print("  // Index, W, H,xAdv,dX,dY")
    print("  {     0,  8, 8,   8, 0,-8}, // Character 0x20 (32: ' ')")

    for char_data in all_characters:
        print("  {   %s,  8, 8,   8, 0,-8}, // Character %s (%s: '%s')" % char_data)

    print("};")
    print()

    # Print Adafruit GFXfont config
    print("const GFXfont medievalish_chonker_mono_Font PROGMEM = {")
    print("  (uint8_t *)medievalish_chonker_mono_Bitmap,")
    print("  (GFXglyph *)medievalish_chonker_mono_Glyphs,")
    print("  32, // ASCII start")
    print("  126, // ASCII stop")
    print("  8 // y Advance")
    print("};")

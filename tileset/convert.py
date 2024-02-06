import csv
from PIL import Image


tileset = Image.open("0xDB_GASP_8x8_1bpp_RPG_icons_v18_packed_1bpp.png")

with open('0xDB_GASP_8x8_1bpp_RPG_icons_v18_packed.csv', newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')
    all_tiles = []

    # Process CSV and print tile content table
    for row_index, row in enumerate(reader):
        if row_index == 0:
            # Skip header row
            continue
        else:
            # Add tile from file

            # Get tile data from CSV
            category = row[0].strip().replace(" ", "")
            name = row[1].strip().replace("\\", "'\\'")  # Wrap backslashes as they break C++ comments
            x = int(row[2])
            y = int(row[3])
            width = int(row[4])
            height = int(row[5])

            # Get 8x8 1bit slice of tile from tileset image
            tile = tileset.crop((x, y, x + width, y + height))
            tile_pixels = tile.load()

            # Convert row of pixels into bits of byte
            tile_bytes = []
            for pixel_y in range(0, height):
                row_byte = 0

                for pixel_x in range(0, width):
                    pixel_value = 1 if tile_pixels[7 - pixel_x, pixel_y] > 0 else 0
                    if pixel_value == 1:
                        row_byte |= (1 << pixel_x)

                tile_bytes.append('0x{:02x}'.format(row_byte))

        all_tiles.append((row_index, category + " " + name))

        # Print tile bytes and info
        print("//", category, name)
        print("static const uint8_t PROGMEM tile" + str(row_index) + " [] = {")
        print("  ", ", ".join(tile_bytes))
        print("};")
        print()

    # Print tileset table
    print("const uint8_t *tiles[] = {")
    for row_index, name in all_tiles:
        print("  tile%s," % str(row_index), "//", name)
    print("};")

import csv
import subprocess
import pygame

all_tiles = {}
with open('0xDB_GASP_8x8_1bpp_RPG_icons_v18_packed.csv', newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')

    for row_index, row in enumerate(reader):
        if row_index == 0:
            continue

        tile_data = {
            "category": row[0].strip().replace(" ", ""),
            "name": row[1].strip().replace("\\", "'\\'"),  # Wrap backslashes as they break C++ comments
            "x": int(row[2]),
            "y": int(row[3]),
            "width": int(row[4]),
            "height": int(row[5])
        }

        if tile_data["y"] not in all_tiles:
            all_tiles[tile_data["y"]] = {}

        all_tiles[tile_data["y"]][tile_data["x"]] = (row_index, tile_data)

pygame.init()

tileset = pygame.image.load("0xDB_GASP_8x8_1bpp_RPG_icons_v18_packed_1bpp.png")
display_width = tileset.get_height() * 2
display_height = tileset.get_height() * 2
tileset = pygame.transform.scale(tileset, (display_width, display_height))

display = pygame.display.set_mode((display_width,display_height))
pygame.display.set_caption("Tile inspector")

clock = pygame.time.Clock()
shutdown = False
tile_index = 0
tile_data = None

while not shutdown:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            shutdown = True
        if event.type == pygame.MOUSEBUTTONUP:
            if tile_data is not None:
                # https://stackoverflow.com/questions/7105433/windows-batch-echo-without-new-line
                cmd = 'echo | set /p tempName="' + str(tile_index - 1) + '"|clip'
                subprocess.check_call(cmd, shell=True)

    display.blit(tileset, (0, 0))

    mouse_x, mouse_y = pygame.mouse.get_pos()
    tile_x = mouse_x // 16
    tile_y = mouse_y // 16

    try:
        tile_index, tile_data = all_tiles[tile_y * 8][tile_x * 8]
    except KeyError as e:
        pass

    if tile_data is not None:
        pygame.display.set_caption(
            "%03d, %03d: %s, %s, %s"
            % (tile_x, tile_y, tile_index - 1, tile_data["category"], tile_data["name"])
        )
    else:
        pygame.display.set_caption("Tile inspector")

    pygame.display.update()
    clock.tick(60)

exit()

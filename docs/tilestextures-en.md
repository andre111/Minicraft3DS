# how textures are assigned to tiles

## general architecture

1. **source image**: icons.png (512*512 pixels) - contains all the textures
2. **Texture arrays**: textures are split into arrays:
    - tile8[(512/8)*(512/8)] = 64\*64=4096 textures of 8\*8px
    - tile16[(512/16)*(512/16)] = 32\*32=1024 textures of 16\*16px
    - tile32 and tile64 for other sizes
3. Index system: each texture is accessed by index = xTile + yTile * (512/size)

## How to add a new tile

.1 Define the tile ID -> [TilesData.h](../source/data/TilesData.h)

```c
#define TILE_YOUR_TILE 10  // the value is the tile's unique id
````

.2 Register the name -> [TilesData.c](../source/data/TilesData.c)

```c
void tilesDataInit() {
    // ... other registrations
    _tileRegister(TILE_YOUR_TILE, "Your Tile Name");
}
```

.3 Add the rendering logic -> [RenderTiles.c](../source/render/RenderTiles.c) function renderTile():

```c
case TILE_YOUR_TILE:
    // Simple example (a single 16*16 texture at tile 20.7)
    renderTile(TILE_WOOD_DOOR_CLOSED, 0, level, x, y);
    renderTile16(x, y, 20, 7, 0);
    break;

// Or with connected tiling (like GRASS/ROCK)
case TILE_YOUR_TILE:
    checkSurrTiles8(level, x >> 4, y >> 4, TILE_YOUR_TILE);
    renderConnectedTile8(x, y, 22, 5); // column 22, row 5
    break;
    // Note: use renderConnectedTile4() for 4-direction tiling (up/down/left/right)
    // checkSurrTiles4/8 are internal static functions of RenderTiles.c
```

.4 Update `MAX_TILE_ID_USED` -> [TilesData.h](../source/data/TilesData.h)

```c
#define MAX_TILE_ID_USED 36 // matches the number of tiles (currently TILE_ICE = 36)
```

.5 add the pixels in [icon.png](../resources/icons.png)

- The coordinates `renderTile16(x, y, **xTile**, **yTile**, 0)` match the grid

- for a 16*16 texture at pixels (320, 112): `xTile = 320/16 = 20`, `yTile = 112/16 = 7`

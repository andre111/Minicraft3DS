# comment les textures sotn attribuées aux tiles

## architecture générale

1. **image source** : icons.png (512*512 pixels) - contient toutes les textures
2. **Tableaux de textures** : Les textures sotn découpéees en tableaux :
    - tile8[(512/8)*(512/8)] = 64\*64=4096 textures de 8\*8px
    - tile16[(512/16)*(512/16)] = 32\*32=1024 textures de 16\*16px
    - tile32 et tile64 pour d'autres tailles
3. Système d'index : chaque texture est accédée par index = xTile + yTile * (512/size)

## Comment ajouter une nouvelle tile

.1 Définir l'ID de la tile -> [TilesData.h](../source/data/TilesData.h)

```c
#define TILE_YOUR_TILE 10  // la valeur est l'id unique de la tile
````

.2 Enregistrer le nom -> [TilesData.c](../source/data/TilesData.c)

```c
void tilesDataInit() {
    // ... autres registrations
    _tileRegister(TILE_YOUR_TILE, "Your Tile Name");
}
```

.3 Ajouter la logique de rendu -> [RenderTiles.c](../source/render/RenderTiles.c) fonction renderTile() :

```c
case TILE_YOUR_TILE:
    // Exemple simple (une seul texture 16*16 au pixel 20.7)
    renderTile(TILE_WOOD_DOOR_CLOSED, 0, level, x, y);
    renderTile16(x, y, 20, 7, 0);
    break;

// Ou avec tiling connecté (comme GRASS/ROCK)
case TILE_YOUR_TILE:
    checkSurrTiles8(level, x >> 4, y >> 4, TILE_YOUR_TILE);
    renderConnected(x, y, 22, 5); // colones 22, ligne 5
    break;
```

.4 Mettre a jour `MAX_TILE_ID_USED` -> [TilesData.h](../source/data/TilesData.h)

```c
#define MAX_TILE_ID_USED 39 // adapté au nombre de tile
```

.5 ajouter les pixels dans [icon.png](../resources/icons.png)

- Les coordonnées `renderTile16(x, y, **xTile**, **yTile**, 0)` correspondent à la grille

- pour une texture 16*16 aux pixels (320, 112) : `xTile = 320/16 = 20`, `yTile = 112/16 = 7`

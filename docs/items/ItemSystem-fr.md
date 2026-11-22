# Système d'Item

## Choix d'architecture

### Pourquoi une table dynamique plutôt qu'un tableau statique

L'ancienne approche utilisait un tableau `ItemsTables[]` à taille fixe, connu à la compilation. Ça fonctionnait pour les items vanilla, mais rendait l'ajout d'items moddés impossible sans recompiler le jeu.

Le choix a été fait de passer à une table allouée dynamiquement sur le heap via `itemsTableBuild(uint16_t modCount)`. La taille est calculée à l'initialisation en fonction du nombre d'items vanilla **plus** le nombre d'items moddés détectés. Cela permet d'étendre la table au runtime sans toucher au code vanilla.

### Pourquoi des IDs runtime séparés des noms logiques

Chaque item a deux identifiants :
- un **ID runtime** (`ItemId`) — entier assigné séquentiellement lors du build, utilisé dans le code et en mémoire pour la rapidité
- un **nom logique stable** (`name`) — chaîne comme `"TOOL_SWORD"`, utilisée pour la sérialisation

Cette séparation est intentionnelle : l'ordre des IDs peut varier selon les mods chargés (et leur ordre), mais les noms restent constants. Les sauvegardes écrivent les noms, pas les IDs, ce qui les rend robustes face à un changement d'ordre de la table.

### Pourquoi une union par catégorie

Les données spécifiques à chaque type d'item (`countLevel` pour les outils, `health` pour la nourriture, etc.) sont stockées dans une `union` dans `ItemData`. Cela évite de réserver de la mémoire pour des champs inutilisés selon la catégorie, tout en gardant un seul type de structure pour toute la table.

### Pourquoi `g_itemCount` au lieu d'une constante

Les vérifications de bornes utilisent `g_itemCount` (valeur runtime) plutôt qu'une constante compile-time. Cela garantit que les bounds checks restent valides même quand la table est étendue avec des items moddés — sans avoir à maintenir une constante à jour manuellement.

---

## Architecture actuelle

### Définition des items
Les items vanilla sont définis dans `source/data/items/ItemsTypes.c` via un tableau statique interne `_vanillaDefs[]`.
Ce tableau contient uniquement les définitions des items du jeu de base et n'est pas exposé directement aux autres modules.

La table d'items accessible globalement est :
- `ItemData *g_itemTable` : pointeur vers la table active
- `unsigned int g_itemCount` : taille actuelle de la table

Ces deux symboles sont initialisés par `itemsTableBuild(uint16_t modCount)`.

### Construction de la table
Le chargement d'items se fait en deux phases :
1. `itemsTableBuild(0)` alloue `g_itemTable` sur le heap et copie les définitions vanilla de `_vanillaDefs[]`
2. `itemsDataInit()` initialise les structures dérivées (`_itemIconX`, `_itemIconY`) selon la taille de `g_itemTable`

L'appel est effectué dans `source/Data.c` :
```c
void initData() {
    uint16_t modCount = 0;
    // TODO: scanner sdmc:/3ds/Minicraft3ds/mods/ ici
    itemsTableBuild(modCount);
    effectsTableBuild(modCount);
    tilesDataInit();
    entityDataInit();
}
```

### Structure des items
Chaque item est décrit par `ItemData` :
- `id` : identifiant runtime unique, assigné séquentiellement lors du build
- `name` : identifiant logique stable (ex: `"TOOL_SWORD"`)
- `displayName` : nom à afficher à l'écran
- `category` : type d'item (`ITEM_CAT_TOOL`, `ITEM_CAT_FOOD`, `ITEM_CAT_FURNITURE`, `ITEM_CAT_GENERIC`, `ITEM_CAT_SPELL`)
- `isStackable` : si l'item peut s'empiler
- `texX`, `texY` : coordonnées de la texture dans le spritesheet
- `data` : union contenant les données spécifiques à la catégorie

### Lookups
- `getIdFromName(const char *name)` → ID runtime
- `getNameFromId(ItemId id)` → nom logique

### Données d'affichage
Les coordonnées d'icônes sont calculées à la volée via :
- `itemGetIconX(id, countLevel)` → coordonnée X dans le spritesheet
- `itemGetIconY(id, countLevel)` → coordonnée Y dans le spritesheet

Ces fonctions lisent directement `texX` et `texY` depuis `g_itemTable[id]`, avec une logique spéciale pour les outils multi-niveaux (sword, shovel, etc.).

> `_itemNames` et `_itemSingle` ont été supprimés — ces données sont désormais directement dans `ItemData` (`displayName` et `isStackable`).

### Sécurité des bornes
Les vérifications utilisent `g_itemCount` au lieu d'une constante fixe.
Cela permet d'étendre la table à des items moddés tout en conservant des vérifications valides.

### Sérialisation (saves)
Le système de sauvegarde reste basé sur des noms stables, ce qui protège les parties contre un changement d'ordre dans la table d'items.
La compatibilité est maintenue tant que les noms vanilla restent inchangés.

---

## Vision modding (data-driven)

### Principe

Le système actuel est *code-driven* pour les items vanilla : les définitions sont écrites directement en C dans `_vanillaDefs[]`. Ce choix est acceptable pour le vanilla car les items ne changent pas sans recompilation.

Pour les mods, l'approche sera *data-driven* : les moddeurs déclarent leurs items dans des **fichiers de données** (format à définir) placés dans `sdmc:/3ds/Minicraft3ds/mods/`. Le jeu les lit au démarrage, sans que le moddeur ait besoin de compiler quoi que ce soit.

### Flux d'initialisation prévu

1. Scanner `sdmc:/3ds/Minicraft3ds/mods/` pour détecter les mods actifs
2. Parser les fichiers de données de chaque mod pour compter et lire les items moddés
3. Appeler `itemsTableBuild(modCount)` avec le nombre total d'items moddés
4. Copier les définitions moddées après les items vanilla dans `g_itemTable`
5. Les IDs sont assignés séquentiellement à la suite des IDs vanilla

### Pourquoi le data-driven pour les mods

- Les moddeurs n'ont pas besoin d'une toolchain C ni de recompiler le jeu
- Les fichiers de données sont plus simples à écrire et à versionner qu'du code C
- L'architecture de la table (dynamique, noms stables, `g_itemCount` runtime) est déjà prête pour recevoir ce flux — seul le scanner et le parser restent à implémenter

### État actuel

Le point d'entrée est présent dans `source/Data.c` avec un TODO :
```c
// TODO: scanner sdmc:/3ds/Minicraft3ds/mods/ ici
```

Le reste de la chaîne (`itemsTableBuild`, lookup, sérialisation) fonctionne déjà et n'aura pas besoin d'être modifié.

---

## Ce qui a changé (historique refacto)

- `ItemsTypes.c`
  - `_vanillaDefs[]` contient les définitions vanilla
  - `itemsTableBuild(uint16_t modCount)` alloue `g_itemTable` dynamiquement
  - `g_itemCount` est défini en runtime
  - `ItemsTables[]` n'existe plus
- `ItemsTypes.h`
  - plus d'`extern ItemData ItemsTables[]`
  - plus d'`extern const unsigned int itemCount`
  - exporte `g_itemTable`, `g_itemCount`, `itemsTableBuild()`
- `ItemsData.c`
  - `_itemNames` et `_itemSingle` supprimés (données migrées dans `ItemData`)
  - `itemGetIconX()` et `itemGetIconY()` remplacent les anciens tableaux (`_itemIconX`, `_itemIconY`)
  - `getNameFromId()` et les bornes utilisent `g_itemCount`
- `Data.c`
  - appelle `itemsTableBuild(0)` puis `effectsTableBuild(0)` avant les autres inits

---

## Fichiers clés

| Fichier | Rôle |
|---|---|
| `source/data/items/ItemsData.h` | Déclaration de `ItemData`, `ItemId`, `ItemCategory`, fonctions liées aux items |
| `source/data/items/ItemsTypes.h` | Expose `g_itemTable`, `g_itemCount` et `itemsTableBuild()` |
| `source/data/items/ItemsTypes.c` | Définitions vanilla, build dynamique de la table d'items |
| `source/data/items/ItemsData.c` | Recherche d'items, initialisation des arrays dérivés, bounds checks |
| `source/Data.c` | Ordre d'initialisation : build de la table puis init des données |
| `source/SaveLoad.c` | Sérialisation et désérialisation des items via noms logiques |

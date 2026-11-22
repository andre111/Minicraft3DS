# Format FNT

## Objectif

Minicraft utilisera, pour la taille des fonts, une technique de rendu « SDF », et du bitmap classique pour certaines tailles.
Le but est de pouvoir redimensionner à la volée la taille des fonts afin d'avoir des textes plus petits, de rendre possible une meilleure UI de debug, l'affichage d'un changelog, et de créer une base qui permettra de rajouter des fonts personnalisées à partir de mods à l'avenir.
Pour pouvoir utiliser ensuite ces fonts, il faut y associer des métadonnées (position des glyphes dans l'atlas, avances, kerning...) ; celles-ci sont décrites dans un format JSON facile à écrire/éditer.

> **Statut** : le format et l'outil de conversion (`fonts/fonts.sh`) existent, mais le jeu ne charge pas encore les fichiers `.fnt` : le rendu actuel (`source/render/RenderText.c`) utilise toujours la police bitmap `resources/font.png`.

## Pourquoi un format dédié ?

On pourrait tout à fait, mais la 3DS est extrêmement lente en parsing. Le format FNT est donc un format binaire dédié à stocker ces valeurs afin de :

- Éviter de reparser du JSON à chaque lancement du jeu.
- Permettre un chargement quasi instantané : le buffer lu depuis la carte SD/ROM peut être casté directement en tableaux de structs C.
- Permettre une recherche de glyphe/kerning en O(log n) via recherche binaire, grâce à des tables pré-triées.
- Garder un format extensible (zone réservée dans l'en-tête) sans casser la compatibilité binaire.

## Vue d'ensemble du fichier

L'extension `.fnt` est une convention propre au projet pour identifier ces fichiers : le contenu est un format binaire générique (rien n'empêcherait de l'enregistrer en `.bin`), mais `.fnt` permet de le distinguer au premier coup d'œil des autres blobs binaires du projet.

Le fichier `.fnt` est une simple concaténation binaire, sans table des matières ni sections de taille variable :

```txt
Offset 0                          : FontBinaryHeader (64 octets, taille fixe)
Offset 64                         : FontGlyph[glyph_count]        (20 octets chacun, triés par codepoint croissant)
Offset 64 + glyph_count*20        : FontKerningPair[kerning_count] (12 octets chacune, triées par (first, second) croissants)
```

Taille totale du fichier :

```txt
taille = 64 + glyph_count * 20 + kerning_count * 12
```

C'est exactement le calcul effectué par l'option `--verify` de l'outil de conversion pour valider l'intégrité d'un fichier généré.

> **Endianness / alignement** : toutes les structures sont déclarées avec `#pragma pack(push, 1)`, donc sans padding ajouté par le compilateur. Les offsets ci-dessous sont stables sur n'importe quel compilateur respectant ce pragma. Le format suppose une plateforme little-endian (PC générateur et 3DS/ARM11 lecteur sont tous deux little-endian).

## `FontBinaryHeader` — 64 octets

| Offset | Taille | Champ | Type | Description |
|---|---|---|---|---|
| 0x00 | 4 | `magic` | `char[4]` | `"FONT"`, sans `\0` stocké. Signature à vérifier avant tout traitement. |
| 0x04 | 2 | `version` | `uint16_t` | Version du format binaire (actuellement `1`). À incrémenter si le layout change. |
| 0x06 | 2 | `flags` | `uint16_t` | Bit 0 (`FONT_FLAG_IS_SDF`) : police en Signed Distance Field. Bit 1 (`FONT_FLAG_KERNING_ENABLED`) : kerning actif. |
| 0x08 | 2 | `glyph_count` | `uint16_t` | Nombre de glyphes (max 65 535). |
| 0x0A | 2 | `kerning_count` | `uint16_t` | Nombre de paires de kerning (max 65 535). |
| 0x0C | 2 | `atlas_width` | `uint16_t` | Largeur en pixels de la texture d'atlas associée. |
| 0x0E | 2 | `atlas_height` | `uint16_t` | Hauteur en pixels de la texture d'atlas associée. |
| 0x10 | 2 | `reference_size_px` | `uint16_t` | Taille de cuisson (bitmap) ou taille de référence (SDF) à laquelle les coordonnées de l'atlas sont valables. |
| 0x12 | 2 | `sdf_spread_px` | `uint16_t` | Rayon de propagation du SDF en pixels ; vaut `0` si `FONT_FLAG_IS_SDF` n'est pas actif. |
| 0x14 | 2 | `line_height` | `uint16_t` | Hauteur de ligne pour la mise en page du texte. |
| 0x16 | 2 | `baseline_offset` | `uint16_t` | Décalage de la ligne de base depuis le haut de la ligne. |
| 0x18 | 4 | `default_char` | `uint32_t` | Codepoint Unicode de fallback, garanti présent dans la table de glyphes. |
| 0x1C | 4 | `font_id_hash` | `uint32_t` | Hash FNV-1a 32 bits du nom logique de la police (voir `font_ids.h`), permet l'identification sans stocker de chaîne dans le binaire. |
| 0x20 | 32 | `reserved` | `uint8_t[32]` | Réservé, mis à `0` à l'écriture. Permet d'étendre le format sans casser la compatibilité binaire. |

## `FontGlyph` — 20 octets, répété `glyph_count` fois

| Offset relatif | Taille | Champ | Type | Description |
|---|---|---|---|---|
| 0x00 | 4 | `codepoint` | `uint32_t` | Point de code Unicode de ce glyphe. |
| 0x04 | 2 | `x` | `uint16_t` | Position X du glyphe dans l'atlas (px). |
| 0x06 | 2 | `y` | `uint16_t` | Position Y du glyphe dans l'atlas (px). |
| 0x08 | 2 | `w` | `uint16_t` | Largeur du glyphe dans l'atlas (px). |
| 0x0A | 2 | `h` | `uint16_t` | Hauteur du glyphe dans l'atlas (px). |
| 0x0C | 2 | `xoffset` | `int16_t` | Décalage horizontal signé à appliquer par rapport au curseur d'écriture. |
| 0x0E | 2 | `yoffset` | `int16_t` | Décalage vertical signé à appliquer par rapport au curseur d'écriture. |
| 0x10 | 2 | `xadvance` | `uint16_t` | Distance dont il faut avancer le curseur horizontal après ce glyphe. |
| 0x12 | 2 | `_pad` | `uint16_t` | Padding explicite, toujours `0`. |

La table est **triée par `codepoint` croissant**, ce qui permet une recherche binaire (`bsearch`) en O(log n) au runtime plutôt qu'un parcours linéaire de tous les glyphes.

## `FontKerningPair` — 12 octets, répété `kerning_count` fois
 
| Offset relatif | Taille | Champ | Type | Description |
|---|---|---|---|---|
| 0x00 | 4 | `first` | `uint32_t` | Codepoint du premier caractère de la paire. |
| 0x04 | 4 | `second` | `uint32_t` | Codepoint du caractère qui suit. |
| 0x08 | 2 | `amount` | `int16_t` | Ajustement signé (px) appliqué à l'avance quand `second` suit `first`. Une valeur négative rapproche les glyphes. |
| 0x0A | 2 | `_pad` | `uint16_t` | Padding explicite, toujours `0`. |

Cette table optionnelle (peut être vide si `kerning_count == 0`) est **triée par `(first, second)` croissants** (`first` en clé primaire), ce qui permet aussi une recherche binaire au runtime.

## Constantes du format

| Nom | Valeur | Rôle |
|---|---|---|
| `FONT_MAGIC` | `"FONT"` | Signature à vérifier en premier à la lecture d'un fichier. |
| `FONT_FORMAT_VERSION` | `1` | Valeur courante de `version`. |
| `FONT_FLAG_IS_SDF` | `1 << 0` | Bit de `flags` indiquant une police SDF. |
| `FONT_FLAG_KERNING_ENABLED` | `1 << 1` | Bit de `flags` indiquant que le kerning doit être appliqué. |

## Invariants garantis par le convertisseur

Un fichier `.fnt` produit par l'outil de conversion respecte toujours les contraintes suivantes, validées avant l'écriture :

- `magic == "FONT"` et `version == FONT_FORMAT_VERSION`.
- Tous les `codepoint` de la table de glyphes sont **uniques** (rejet en cas de doublon).
- `default_char` correspond à un glyphe **effectivement présent** dans la table.
- Si `FONT_FLAG_IS_SDF` est actif, `sdf_spread_px > 0` (vérifié dès le parsing du JSON).
- Les glyphes sont triés par `codepoint` croissant.
- Les paires de kerning sont triées par `(first, second)` croissants.
- `reserved[32]` est mis à zéro (aucune extension utilisée pour l'instant).
- Taille du fichier == `64 + glyph_count * 20 + kerning_count * 12` (vérifiable avec l'option `--verify`, voir section 9).

## Format source JSON attendu

Le convertisseur attend un JSON avec trois sections : `header` (objet), `glyphs` (tableau non vide) et `kerning` (tableau optionnel).

```json
{
  "header": {
    "font_id_name": "main_ui",
    "is_sdf": true,
    "atlas_width": 512,
    "atlas_height": 512,
    "reference_size_px": 32,
    "sdf_spread_px": 4,
    "line_height": 40,
    "baseline_offset": 30,
    "default_char": 63,
    "kerning_enabled": true
  },
  "glyphs": [
    { "codepoint": 65, "x": 0, "y": 0, "w": 20, "h": 24, "xoffset": 0, "yoffset": 0, "xadvance": 22 },
    { "codepoint": 66, "x": 20, "y": 0, "w": 18, "h": 24, "xadvance": 20 }
  ],
  "kerning": [
    { "first": 65, "second": 86, "amount": -2 }
  ]
}
```

### Champ `header`

| Champ | Type | Obligatoire | Défaut | Description |
|---|---|---|---|---|
| `font_id_name` | string | **oui** | — | Nom logique de la police, hashé en FNV-1a 32 bits pour produire `font_id_hash`. |
| `is_sdf` | bool | non | `false` | Active `FONT_FLAG_IS_SDF`. Nécessite `sdf_spread_px > 0` si vrai. |
| `atlas_width` | int | **oui** | — | Largeur de l'atlas en px. |
| `atlas_height` | int | **oui** | — | Hauteur de l'atlas en px. |
| `reference_size_px` | int | non | `0` | Taille de référence/cuisson en px. |
| `sdf_spread_px` | int | non | `0` | Rayon de propagation SDF en px. |
| `line_height` | int | non | `0` | Hauteur de ligne. |
| `baseline_offset` | int | non | `0` | Décalage de la ligne de base. |
| `default_char` | int | non | `63` (`?`) | Codepoint de fallback. |
| `kerning_enabled` | bool | non | `false` | Active `FONT_FLAG_KERNING_ENABLED`. |

### Éléments de `glyphs[]`

| Champ | Type | Obligatoire | Défaut |
|---|---|---|---|
| `codepoint` | int | **oui** | — |
| `x` | int | **oui** | — |
| `y` | int | **oui** | — |
| `w` | int | **oui** | — |
| `h` | int | **oui** | — |
| `xadvance` | int | **oui** | — |
| `xoffset` | int | non | `0` |
| `yoffset` | int | non | `0` |

### Éléments de `kerning[]`

| Champ | Type | Obligatoire |
|---|---|---|
| `first` | int | **oui** |
| `second` | int | **oui** |
| `amount` | int | **oui** |

## Utilisation de l'outil de conversion

```bash
minicraft3ds-font-json2bin <input.json> <output.fnt> [--verify]
```

- `input.json` : fichier source décrivant la police (voir section 8).
- `output.fnt` : chemin du fichier binaire à générer.
- `--verify` (optionnel) : relit immédiatement le fichier généré, vérifie le `magic` et recalcule la taille attendue (`64 + glyph_count*20 + kerning_count*12`) pour la comparer à la taille réelle du fichier. Utile en local et en CI pour détecter une régression du format ou du convertisseur.

En cas d'erreur (JSON invalide, champ requis manquant, codepoint dupliqué, `default_char` introuvable, `is_sdf` sans `sdf_spread_px` valide...), l'outil affiche un message explicite sur `stderr` et quitte avec un code de retour non nul, sans écrire de fichier de sortie partiel.

## 10. Utilisation côté runtime (lecture sur 3DS)

1. Charger le fichier `.fnt` en mémoire (lecture brute, pas de parsing).
2. Caster les 64 premiers octets en `FontBinaryHeader*` ; vérifier `magic` et `version` avant tout usage.
3. Caster les `glyph_count` blocs de 20 octets suivants en tableau de `FontGlyph`.
4. Caster les `kerning_count` blocs de 12 octets suivants en tableau de `FontKerningPair`.
5. Pour dessiner un caractère : recherche binaire (`bsearch`) par `codepoint` dans le tableau de glyphes trié. Si le codepoint est absent, utiliser le glyphe de `default_char`.
6. Pour l'espacement entre deux caractères consécutifs : recherche binaire par `(first, second)` dans la table de kerning triée ; si aucune paire ne correspond, avance standard (`xadvance`) sans ajustement.
7. Pour le rendu à une taille d'affichage donnée : calculer `scale = taille_voulue_px / reference_size_px`, puis utiliser `sdf_spread_px` dans le shader SDF pour déterminer le seuillage des bords à cette échelle et obtenir un rendu net à n'importe quelle taille sans regénérer l'atlas.

Comme le format est entièrement POD et packé sur 1 octet, aucune étape de désérialisation n'est nécessaire : le buffer lu depuis le stockage peut être utilisé directement comme tableaux de structs C, ce qui rend le chargement quasi instantané.

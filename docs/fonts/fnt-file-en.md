# FNT Format

## objective

Minicraft will use an “SDF” rendering technique for font sizes and traditional bitmaps for certain sizes.
The goal is to be able to resize fonts on the fly to display smaller text, enable a better debug UI, display a changelog, and create a foundation that will allow for the addition of custom fonts from mods in the future.
To be able to use these fonts later, you need to associate metadata with them (glyph positions in the atlas, tracking, kerning, etc.); this metadata is described in a JSON format that is easy to write and edit.

> **Status**: the format and the conversion tool (`fonts/fonts.sh`) exist, but the game does not load `.fnt` files yet: current rendering (`source/render/RenderText.c`) still uses the `resources/font.png` bitmap font.

## Why a dedicated format?

That would certainly be possible, but the 3DS is extremely slow at parsing. The FNT format is therefore a binary format designed to store these values in order to:

- Avoid parsing JSON again every time the game launches.
- Enable near-instant loading: data read from the SD card or ROM can be cast directly into arrays of C structs.
- Enable glyph/kerning lookups in O(log n) using binary search, thanks to pre-sorted tables.
- Maintain an extensible format (reserved area in the header) without breaking binary compatibility.

## File Overview

The `.fnt` extension is a project-specific convention for identifying these files: the content is a generic binary format (there’s nothing stopping you from saving it as `.bin`), but `.fnt` makes it easy to distinguish at a glance from other binary blobs in the project.

The `.fnt` file is a simple binary concatenation, with no table of contents or sections of varying sizes:

```txt
Offset 0                          : FontBinaryHeader (64 bytes, fixed size)
Offset 64                         : FontGlyph[glyph_count]        (20 bytes each, sorted by ascending codepoint)
Offset 64 + glyph_count*20        : FontKerningPair[kerning_count] (12 bytes each, sorted in ascending order by (first, second))
```

Total file size :

```txt
size = 64 + glyph_count * 20 + kerning_count * 12
```

This is exactly the calculation performed by the `--verify` option of the conversion tool to validate the integrity of a generated file.

> **Endianness / alignement** : All structures are declared with `#pragma pack(push, 1)`, so no padding is added by the compiler. The offsets below are consistent across any compiler that adheres to this pragma. The format assumes a little-endian platform (both the generator PC and the 3DS/ARM11 reader are little-endian).

## `FontBinaryHeader` — 64 bytes

| Offset | size | Field | Type | Description |
|---|---|---|---|---|
| 0x00 | 4 | `magic` | `char[4]` | `"FONT"`, sans `\0` Stored. Signature must be verified before any processing. |
| 0x04 | 2 | `version` | `uint16_t` | Binary format version (actuellement `1`). Binary format version (currently `1`). Increment this value if the layout changes. |
| 0x06 | 2 | `flags` | `uint16_t` | Bit 0 (`FONT_FLAG_IS_SDF`) : Font in Signed Distance Field. Bit 1 (`FONT_FLAG_KERNING_ENABLED`): kerning enabled |
| 0x08 | 2 | `glyph_count` | `uint16_t` | Number of glyphs (max 65,535). |
| 0x0A | 2 | `kerning_count` | `uint16_t` | Number of kerning pairs (max 65,535). |
| 0x0C | 2 | `atlas_width` | `uint16_t` | Width in pixels of the associated atlas texture. |
| 0x0E | 2 | `atlas_height` | `uint16_t` | Height in pixels of the associated atlas texture. |
| 0x10 | 2 | `reference_size_px` | `uint16_t` | Rendering size (bitmap) or reference size (SDF) for which the atlas coordinates are valid. |
| 0x12 | 2 | `sdf_spread_px` | `uint16_t` | SDF propagation radius in pixels; equals `0` if `FONT_FLAG_IS_SDF` is not enabled. |
| 0x14 | 2 | `line_height` | `uint16_t` | Line height for text layout. |
| 0x16 | 2 | `baseline_offset` | `uint16_t` | Baseline offset from the top of the line. |
| 0x18 | 4 | `default_char` | `uint32_t` | Unicode fallback codepoint, guaranteed to be present in the glyph table. |
| 0x1C | 4 | `font_id_hash` | `uint32_t` | A 32-bit FNV-1a hash of the font's logical name (see `font_ids.h`), which allows for identification without storing a string in the binary. |
| 0x20 | 32 | `reserved` | `uint8_t[32]` | Reserved; set to `0` on write. Allows the format to be extended without breaking binary compatibility. |

## `FontGlyph` — 20 bytes, repeated `glyph_count` times

| Relative offset | Size | Field | Type | Description |
|---|---|---|---|---|
| 0x00 | 4 | `codepoint` | `uint32_t` | Unicode code point for this glyph. |
| 0x04 | 2 | `x` | `uint16_t` | Position X of the glyph in the atlas (px). |
| 0x06 | 2 | `y` | `uint16_t` | Y-position of the glyph in the atlas (px). |
| 0x08 | 2 | `w` | `uint16_t` | Glyph width in the atlas (px). |
| 0x0A | 2 | `h` | `uint16_t` | Glyph height in the atlas (px). |
| 0x0C | 2 | `xoffset` | `int16_t` | Specified horizontal offset to be applied relative to the writing cursor. |
| 0x0E | 2 | `yoffset` | `int16_t` | Specified vertical offset to be applied relative to the writing cursor. |
| 0x10 | 2 | `xadvance` | `uint16_t` | The distance by which the horizontal cursor must be moved after this glyph. |
| 0x12 | 2 | `_pad` | `uint16_t` | Explicit padding, always `0`. |

## `FontKerningPair` — 12 bytes, repeated `kerning_count` times

| Relative offset | Size | Field | Type | Description |
|---|---|---|---|---|
| 0x00 | 4 | `first` | `uint32_t` | Codepoint of the first character in the pair. |
| 0x04 | 4 | `second` | `uint32_t` | Codepoint of the following character. |
| 0x08 | 2 | `amount` | `int16_t` | Signed adjustment (px) applied in advance when `second` follows `first`. A negative value brings the glyphs closer together. |
| 0x0A | 2 | `_pad` | `uint16_t` | Explicit padding, always `0`. |

This optional table (which may be empty if `kerning_count == 0`) is **sorted in ascending order by `(first, second)`** (`first` as the primary key), which also allows for a binary search at runtime.

## Format Constants

| Name | Value | role |
|---|---|---|
| `FONT_MAGIC` | `"FONT"` | The first thing to check when reading a file is the signature. |
| `FONT_FORMAT_VERSION` | `1` | Current value of `version`. |
| `FONT_FLAG_IS_SDF` | `1 << 0` | Bit in `flags` indicating an SDF font. |
| `FONT_FLAG_KERNING_ENABLED` | `1 << 1` | A bit in `flags` indicating that kerning should be applied. |

## Invariants Guaranteed by the Converter

A `.fnt` file generated by the conversion tool always complies with the following constraints, which are validated before the file is written:

- `magic == “FONT”` and `version == FONT_FORMAT_VERSION`.
- All `codepoints` in the glyph table are **unique** (duplicates are rejected).
- `default_char` corresponds to a glyph that is **actually present** in the table.
- If `FONT_FLAG_IS_SDF` is enabled, `sdf_spread_px > 0` (checked during JSON parsing).
- The glyphs are sorted in ascending order by `codepoint`.
- Kerning pairs are sorted in ascending order by `(first, second)`.
- `reserved[32]` is set to zero (no extensions are currently in use).
- File size == `64 + glyph_count * 20 + kerning_count * 12` (can be verified using the `--verify` option; see Section 9).

## Expected JSON source format

The converter expects a JSON object with three sections: `header` (object), `glyphs` (non-empty array), and `kerning` (optional array).

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

### Field `header`

| Fiedl | Type | Required | Default | Description |
|---|---|---|---|---|
| `font_id_name` | string | **yes** | — | Logical font name, hashed using 32-bit FNV-1a to produce `font_id_hash`. |
| `is_sdf` | bool | no | `false` | Enables `FONT_FLAG_IS_SDF`. Requires `sdf_spread_px > 0` if true. |
| `atlas_width` | int | **yes** | — | Width of the atlas in px. |
| `atlas_height` | int | **yes** | — | Height of the header in px. |
| `reference_size_px` | int | no | `0` | Reference/cooking size in px. |
| `sdf_spread_px` | int | no | `0` | SDF propagation radius in px. |
| `line_height` | int | no | `0` | Line height. |
| `baseline_offset` | int | no | `0` | Baseline Shift. |
| `default_char` | int | no | `63` (`?`) | Fallback codepoint. |
| `kerning_enabled` | bool | no | `false` | enable `FONT_FLAG_KERNING_ENABLED`. |

### Elements of `glyphs[]`

| Field | Type | Required | Default |
|---|---|---|---|
| `codepoint` | int | **yes** | — |
| `x` | int | **yes** | — |
| `y` | int | **yes** | — |
| `w` | int | **yes** | — |
| `h` | int | **yes** | — |
| `xadvance` | int | **yes** | — |
| `xoffset` | int | no | `0` |
| `yoffset` | int | no | `0` |

### Elements of `kerning[]`

| Field | Type | Required |
|---|---|---|
| `first` | int | **yes** |
| `second` | int | **yes** |
| `amount` | int | **yes** |

## Using the Conversion Tool

```bash
minicraft3ds-font-json2bin <input.json> <output.fnt> [--verify]
```


- `input.json` : source file describing the font (see Section 8).
- `output.fnt` : path to the binary file to be generated.
- `--verify` (optional): Immediately re-reads the generated file, checks the `magic`, and recalculates the expected size (`64 + glyph_count*20 + kerning_count*12`) to compare it with the file's actual size. Useful locally and in CI to detect regressions in the format or the converter.

If an error occurs (invalid JSON, missing required field, duplicate codepoint, `default_char` not found, `is_sdf` without a valid `sdf_spread_px`...), the tool displays a clear message on `stderr` and exits with a non-zero return code, without writing a partial output file.

## 10. Runtime Usage (Playing on the 3DS)

1. Load the `.fnt` file into memory (raw read, no parsing).
2. Cast the first 64 bytes to `FontBinaryHeader*`; check `magic` and `version` before using it.
3. Cast the next `glyph_count` 20-byte blocks into an array of `FontGlyph` objects.
4. Cast the next `kerning_count` 12-byte blocks into an array of `FontKerningPair` objects.
5. To draw a character: Perform a binary search (`bsearch`) by `codepoint` in the sorted glyph array. If the codepoint is not found, use the glyph for `default_char`.
6. For the spacing between two consecutive characters: perform a binary search using `(first, second)` in the sorted kerning table; if no matching pair is found, use the standard advance (`xadvance`) without adjustment.
7. To render at a given display size: calculate `scale = desired_size_px / reference_size_px`, then use `sdf_spread_px` in the SDF shader to determine the edge threshold at that scale and obtain a sharp rendering at any size without regenerating the atlas.

Since the format is entirely POD and packed into 1 byte, no deserialization step is required: the buffer read from storage can be used directly as arrays of C structs, making loading virtually instantaneous.

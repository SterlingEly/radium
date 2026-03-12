# RADIUM 2 — CONTEXT SEED FOR NEW THREAD
*Everything a fresh Claude session needs to resume Radium 2.x development*

---

## 1. WHAT IS THIS PROJECT?

**Radium 2** is a Pebble watchface built around a *radial bar graph* concept — the entire screen acts as a starburst of wedge-shaped tick marks radiating from the center, with hours occupying the bottom half and minutes the top half of the circle. The "Radium/radial" naming is a happy coincidence. The design originated ~9 years ago (Sterling's concept), was first implemented by MathewReiss, then completed by MicroByte. Radium 2 is a from-scratch rebuild by Sterling Ely + Claude (2026), modernizing the watchface for all 7 Pebble platforms and the Rebble ecosystem.

**Sterling's role:** Design/concept lead.  
**Claude's role:** Technical implementation partner.

---

## 2. HISTORY & ATTRIBUTION

| Project | Year | Designer | Developer |
|---------|------|----------|-----------|
| Bar Graph (v1) | 2013–2014 | Sterling Ely | Cameron MacFarland (distantcam) |
| Radium (v1) | 2015–2016 | Sterling Ely | MathewReiss + MicroByte |
| Radium 2 | 2026 | Sterling Ely | Sterling Ely + Claude |

**Original Radium repo:** https://github.com/MathewReiss/radium  
**Original Radium app store:** https://apps.repebble.com/en_US/application/584b212dce45dc907d00008f

---

## 3. LIVE STATUS

- **Radium 2 is LIVE on the Rebble app store as v2.1**
- Store URL: https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926
- Rebble portal: https://apps.repebble.com/en_US/application/69a6531826cc4f0009c65926
- GitHub repo: https://github.com/SterlingEly/Radium2 (branch: `master`)

---

## 4. REPO STRUCTURE

```
SterlingEly/Radium2 (master)
├── CHANGELOG.md
├── README.md
├── STORE_LISTING.md
├── package.json          ← appinfo equivalent (new SDK format)
├── wscript
├── assets/
│   ├── banner.py         ← Python/Pillow banner generator
│   └── radium2_banner.png
└── src/
    ├── c/
    │   └── main.c        ← SHA: a0ba62a725df53c6534b0b075362f1fd82c25a5c
    └── pkjs/
        ├── config.js     ← SHA: 3fec1f37dc32648cca8cbb0a3f6f4b1125b8e487
        └── index.js      ← SHA: 6fc73a2fcbd341cc6b91c859d806140998dc660b
```

**Note:** There are orphaned font files at `resources/fonts/Roboto-Bold.ttf` and `resources/fonts/Roboto-Regular.ttf` — these are unused and noted in README. They don't affect the build.

---

## 5. CURRENT VERSION SPEC (v2.1)

### package.json
- `version`: `"2.1.0"`
- `uuid`: `2609e817-f8f2-4ad2-8846-cb05bb67d047`
- `displayName`: `"Radium 2"`
- `sdkVersion`: `"3"`
- `capabilities`: `["configurable", "health"]`
- `targetPlatforms`: `["aplite", "basalt", "chalk", "diorite", "emery", "gabbro", "flint"]`
- **messageKeys** (16, in order): `BackgroundColor`, `TimeTextColor`, `DateTextColor`, `LitHourColor`, `LitMinuteColor`, `LitBatteryColor`, `LitStepsColor`, `DimHourColor`, `DimMinuteColor`, `DimBatteryColor`, `DimStepsColor`, `OverlayBgColor`, `StepGoal`, `OverlayMode`, `InvertBW`, `ShowRing`

### C constants (main.c)
```c
#define SETTINGS_KEY      2   // bumped from 1 in v2.1 (struct changed)
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2   // px gap between outer ring and tick radials
#define RING_THICK        6   // outer ring thickness

#define OVERLAY_ON     0
#define OVERLAY_OFF    1
#define OVERLAY_SHAKE  2
```

### Settings struct (RadiumSettings)
```c
typedef struct {
  GColor TimeTextColor, DateTextColor;            // Text (2)
  GColor LitHourColor, LitMinuteColor, LitBatteryColor, LitStepsColor;  // Lit (4)
  GColor DimHourColor, DimMinuteColor, DimBatteryColor, DimStepsColor;  // Unlit (4)
  GColor BackgroundColor, OverlayBgColor;          // Base (2)
  int    StepGoal;
  int    OverlayMode;  // OVERLAY_ON/OFF/SHAKE
  bool   InvertBW;     // B&W only
  bool   ShowRing;     // show/hide outer battery+steps ring
} RadiumSettings;
```

### Defaults
- Background: black; Overlay: black
- Color watches: lit = GColorMintGreen, dim = GColorDarkGray, text = white
- B&W watches: lit = white, dim = DarkGray
- OverlayMode: OVERLAY_SHAKE (overlay starts visible on boot, shake toggles)
- ShowRing: true

---

## 6. DRAWING ARCHITECTURE

### Layer stack (bottom to top)
1. Background fill (BackgroundColor)
2. Tick wedges / radial arcs (hours + minutes)
3. Inner gap strip (rect only, separates ticks from ring)
4. Outer ring: battery (right) + steps (left)
5. Center overlay circle (filled with OverlayBgColor)
6. Text overlay: DAY / TIME / DATE (GOTHIC_18 / LECO_36 / GOTHIC_18_BOLD)

### Tick geometry — RECT platforms
- **Minutes:** 12 groups of 5 ticks; 15° pitch (9° tick + 6° gap); start at 3°
  - Each group: 5 individual 1° ticks (color only), 1° gaps between them
  - B&W: 2° wide ticks per minute (no within-group gaps)
  - Color: 1° ticks with bg-color gap cuts
- **Hours:** 12 slots; 15° pitch (9° tick + 6° gap); start at 183°
  - 24h mode: each slot split into two 3° sub-ticks with 3° gap (perfect thirds of 9°)
  - Inter-slot gap: 1° cut on color, none on B&W
- **Outer ring (rect):** Perimeter strip, 5px thick, 5px center gap
  - Battery: right half, fills from bottom
  - Steps: left half, fills from bottom

### Tick geometry — ROUND platforms
- Uses `graphics_fill_radial()` directly
- Same angular layout as rect

### Overlay
- Rect: radius 64px (≥200px wide) or 58px
- Round: 110px (≥260px wide), 76px (≥180px), 58px otherwise
- When ShowRing is false: inset=0, ticks extend to screen edge (starburst mode)

### Tick thickness formula
```c
// OVERLAY_OFF: ticks fill entire screen (full starburst, tick_thick = inner_short)
// OVERLAY_SHAKE/ON:
// Round: (h > 180) ? inner_short * 36/164 : inner_short * 18/164
// Rect:  inner_short * 19/164
```

---

## 7. CONFIG PAGE (config.js)

### Color model
12 fully independent color slots, cascading UI:
- **Text:** TimeTextColor, DateTextColor
- **Lit:** LitHourColor, LitMinuteColor, LitBatteryColor, LitStepsColor
- **Unlit:** DimHourColor, DimMinuteColor, DimBatteryColor, DimStepsColor
- **Base:** BackgroundColor, OverlayBgColor

### Cascade hierarchy
- TextAll → TimeTextColor + DateTextColor
- LitAll → LitTicks + LitRing → individual colors
- DimAll → DimTicks + DimRing → individual colors
- BaseAll → BackgroundColor + OverlayBgColor

### Settings persistence
- `index.js` uses `localStorage` with key `'radium2_settings'`
- `location.search` is NOT reliable — fixed to use localStorage

### Platform detection
- `'bw'` = B&W (aplite, diorite, flint, gabbro)
- Color section hidden on B&W; B&W section hidden on color

---

## 8. PRESET SYSTEM (40 presets, 5 rows × 8)

**Dark (0–7):** Radium, Ember, Cobalt, Slate, Radium+, Crimson, Ocean, Volt  
**Dark+ (8–15):** Dusk, Horizon, Reactor, Venom, Blossom, Solar, Aurora, Neon  
**Light (16–23):** Paper, Jade, Sapphire, Ruby, Mint, Rose, Sky, Sepia  
**Color (24–31):** Teal, Flame, Plum, Forest, Midnight, Cinnabar, Ultraviolet, Ash  
**Special (32–39):** Hearth, Boreal, GoldEye, Viper, Inferno, Cosmos, Triadic, Rainbow

All presets define all 12 color slots.

---

## 9. CLOUDPEBBLE / BUILD RULES (CRITICAL)

1. **package.json format** (current): `messageKeys` as array is correct
2. **Old appinfo.json format:** use `appKeys` as key/value object — NEVER array
3. **No resources/media block** in appinfo.json
4. **Menu icons** via CloudPebble UI only
5. **No tilde (~) in resource filenames**
6. CloudPebble imports from GitHub

### Platform matrix
| Platform | Screen | Colors | Health |
|----------|--------|--------|--------|
| aplite   | 144×168 rect | B&W | No |
| basalt   | 144×168 rect | 64 color | Yes |
| chalk    | 180×180 round | 64 color | Yes |
| diorite  | 144×168 rect | B&W | Yes |
| emery    | 200×228 rect | 64 color | Yes |
| flint    | 144×168 rect | B&W | No |
| gabbro   | 144×168 rect | B&W | Yes |

---

## 10. KEY BUGS FIXED

| Bug | Fix |
|-----|-----|
| Platform detection inversion | Fixed v2.0, re-audited v2.1 |
| Settings not persisting | localStorage in index.js |
| Overlay default was always-on | Changed to OVERLAY_SHAKE |
| Noon/midnight shows 0 | Fixed: `(s_hour % 12) ?: 12` |
| B&W tick rasterization gaps | 2° wide ticks, group-based rendering |
| Round battery ring direction | Fixed direction logic |
| 24h separator too large | 3° cut (perfect thirds of 9° tick) |

---

## 11. DESIGN PHILOSOPHY

- **Radial bar graph:** The watchface IS a bar graph, arranged radially. Minutes = top half, hours = bottom half. Pure geometric data visualization, no hands or numerals.
- **Overlay duality:** Shake toggles center circle with day/time/date. Pure art mode without it.
- **Nine-year backstory:** Core part of the store identity.
- **Ring toggle:** ShowRing=false → pure starburst mode, ticks reach screen edge.

---

## 12. BANNER

`assets/banner.py` — Python/Pillow, same trigonometry as C drawing code.

**Final approved design:**
- 470×320px, text LEFT (x=32), face RIGHT (cx=W-150)
- Blurred 24h starburst background; dark mask circle behind face
- "Radium" 78px mint-white + "2" superscript 44px pure mint
- Tagline at 19px below title
- "Ra" Easter egg: brighter glow treatment (atomic symbol)
- Saved to repo as `assets/radium2_banner.png`

---

## 13. OPEN ITEMS (v2.2+)

1. Dual-color preset themes
2. GoldenEye HUD preset refinement
3. Independent outer ring colors per preset
4. Better hardware mockups (no watch name labels)
5. High-res 24h starburst for banner background
6. Re-verify gabbro platform builds correctly

---

## 14. QUICK REFERENCE

```
Repo:           https://github.com/SterlingEly/Radium2
Branch:         master
Live store:     https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926
UUID:           2609e817-f8f2-4ad2-8846-cb05bb67d047
Version:        2.1.0
SETTINGS_KEY:   2
localStorage:   'radium2_settings'
main.c SHA:     a0ba62a725df53c6534b0b075362f1fd82c25a5c
config.js SHA:  3fec1f37dc32648cca8cbb0a3f6f4b1125b8e487
index.js SHA:   6fc73a2fcbd341cc6b91c859d806140998dc660b
```

---

*Radium 2.1 is LIVE. Next: implement v2.2+ features from section 13.*

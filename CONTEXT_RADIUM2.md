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
**Original Radium appstore:** https://apps.repebble.com/en_US/application/584b212dce45dc907d00008f

---

## 3. LIVE STATUS

- **Radium 2 is LIVE on the Rebble app store as v2.1** (v2.2 is ready to submit)
- Store URL: https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926
- GitHub repo: https://github.com/SterlingEly/Radium2 (branch: `master`)
- HEAD: `374cc2ae` — v2.2 polish: preset overhaul, dim color consistency

---

## 4. REPO STRUCTURE

```
SterlingEly/Radium2 (master)
├── CHANGELOG.md
├── README.md
├── STORE_LISTING.md
├── CONTEXT_RADIUM2.md     ← this file
├── CONTEXT_MONOGRAM.md    ← separate project
├── CONTEXT_BARGRAPH2.md   ← separate project
├── package.json           ← appinfo equivalent (new SDK format)
├── wscript
└── src/
    ├── c/
    │   └── main.c
    └── pkjs/
        ├── config.js      ← config page HTML/JS built as a data URL
        └── index.js       ← PebbleKit JS: platform detection, settings relay, weather
```

**Note:** `resources/fonts/` contains orphaned Roboto font files from an earlier iteration. They are unused and do not affect the build.

---

## 5. CURRENT VERSION SPEC (v2.2, ready to submit)

### package.json
- `version`: `"2.2.0"`
- `uuid`: `2609e817-f8f2-4ad2-8846-cb05bb67d047`
- `displayName`: `"Radium 2"`
- `sdkVersion`: `"3"`
- `capabilities`: `["configurable", "health"]`
- `targetPlatforms`: `["aplite", "basalt", "chalk", "diorite", "emery", "gabbro", "flint"]`
- **messageKeys** (29, in order):
  `BackgroundColor`, `OverlayColor`, `TimeColor`,
  `LitHourColor`, `LitMinuteColor`, `LitBatteryColor`, `LitStepsColor`,
  `DimHourColor`, `DimMinuteColor`, `DimBatteryColor`, `DimStepsColor`,
  `HourTipColor`, `MinuteTipColor`,
  `Line1Color`, `Line2Color`, `Line3Color`, `Line4Color`,
  `Line1Field`, `Line2Field`, `Line3Field`, `Line4Field`,
  `StepGoal`, `OverlayMode`, `OverlaySize`, `InvertBW`, `ShowRing`,
  `WeatherTempF`, `WeatherTempC`, `WeatherCode`

### C constants (main.c)
```c
#define SETTINGS_KEY      7          // bumped each time struct layout changes
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2          // px gap between outer ring and tick radials
#define RING_THICK        6          // outer ring thickness (px)

// Overlay visibility modes
#define OVERLAY_ALWAYS_ON   0
#define OVERLAY_OFF         1
#define OVERLAY_SHAKE       2
#define OVERLAY_AUTO        3        // shake to show for ~60s, then art mode

#define OVERLAY_AUTO_HIDE_MS  60000  // auto-hide timeout for OVERLAY_AUTO (ms)

// Overlay size
#define OVERLAY_SMALL  0
#define OVERLAY_LARGE  1

// Info line field IDs
#define FIELD_NONE      0
#define FIELD_DAY_LONG  1   // "SATURDAY"
#define FIELD_DATE      2   // "MAR 21"
#define FIELD_DAY_DATE  3   // "SAT MAR 21"
#define FIELD_STEPS     4   // footprint icon + step count
#define FIELD_TEMP_F    5   // weather icon + temperature in F
#define FIELD_TEMP_C    6   // weather icon + temperature in C
#define FIELD_BATTERY   7   // battery icon + charge %
#define FIELD_DISTANCE  8   // footprint icon + walked distance (mi or km)
#define FIELD_CALORIES  9   // flame icon + active kcal
```

### Settings struct (RadiumSettings) — SETTINGS_KEY 7
```c
typedef struct {
  GColor BackgroundColor;   // watchface background
  GColor OverlayColor;      // center overlay circle fill
  GColor TimeColor;         // LECO time digits
  GColor LitHourColor;      // lit hour ticks
  GColor LitMinuteColor;    // lit minute ticks
  GColor LitBatteryColor;   // lit battery ring
  GColor LitStepsColor;     // lit steps ring
  GColor DimHourColor;      // dim hour ticks
  GColor DimMinuteColor;    // dim minute ticks
  GColor DimBatteryColor;   // dim battery ring
  GColor DimStepsColor;     // dim steps ring
  GColor HourTipColor;      // leading hour tick highlight
  GColor MinuteTipColor;    // leading minute tick highlight
  GColor Line1Color;        // top-outer info line
  GColor Line2Color;        // top-inner info line
  GColor Line3Color;        // bottom-inner info line
  GColor Line4Color;        // bottom-outer info line
  int Line1Field;           // FIELD_* value for line 1
  int Line2Field;
  int Line3Field;
  int Line4Field;
  int  StepGoal;
  int  OverlayMode;
  int  OverlaySize;
  bool InvertBW;
  bool ShowRing;
} RadiumSettings;
```

### Defaults (prv_default_settings) — matches Radium preset
- Background/Overlay: black
- Color: lit = GColorGreen (#00ff00), dim = GColorDarkGreen (#005500), tips = GColorMintGreen (#aaffaa), time = White
- Color lines: outer (1,4) = GColorGreen; inner (2,3) = GColorMintGreen
- B&W: lit = White, dim = DarkGray
- Line1=NONE, Line2=DAY_LONG, Line3=DATE, Line4=NONE
- OverlayMode = OVERLAY_SHAKE
- OverlaySize = OVERLAY_LARGE on emery/gabbro; OVERLAY_SMALL elsewhere
- ShowRing = false on aplite; true elsewhere

---

## 6. DRAWING ARCHITECTURE

### Layer stack (bottom to top)
1. Background fill (BackgroundColor)
2. Tick wedges / radial arcs (hours + minutes)
3. Inner gap strip (rect only — cleans up tick bleed inside ring gap)
4. Outer ring: battery (right) + steps (left)
5. Center overlay circle (filled with OverlayColor)
6. Info lines + time digits (GOTHIC_18_BOLD or GOTHIC_24_BOLD + LECO_36 or LECO_42)

### Tick geometry — RECT platforms
- **Minutes:** 12 groups × 5 ticks; 15° pitch (9° tick + 6° gap); start at 3°
  - Color: 1° sub-ticks with 1° bg-color gap cuts between them
  - B&W: 2° wide ticks, no within-group gaps
- **Hours:** 12 slots; 15° pitch (9° tick + 6° gap); start at 183°
  - 12h: solid 9° per slot
  - 24h: each slot split into two 3° sub-ticks with 3° gap (perfect thirds)
  - Inter-slot separator: 1° bg-color cut
- Tick thickness: `inner_short * 19/164` when overlay is showing; full inner_short otherwise
- Wedge radius extends beyond screen edge (full-bleed)

### Tick geometry — ROUND platforms
- Uses `graphics_fill_radial()` on the inset tick_rect
- 60 individual 1° minute arcs; same 12-slot × 9° hour arcs
- Ring: `graphics_fill_radial` on bounds, RING_THICK=6px

### Leading-tick tip highlights (color only)
- **Hour tip bug fixed (v2.2):** Round watch 12h mode was using the 24h formula for tip slot
  calculation, causing an extra lit tick at odd hours. Fix: `if (!is_24h)` branch added,
  mirroring the rect branch. Both now use `filled_slots - 1` for 12h mode.
- Tip recolors the last lit slot rather than painting an additional tick.

### Overlay
- Small: 58px radius, LECO_36_BOLD, GOTHIC_18_BOLD (all platforms except emery/gabbro default)
- Large: 70px radius, LECO_42, GOTHIC_24_BOLD (emery/gabbro default)
- All icon+text fields (steps, battery, weather) use dynamic centering via
  `graphics_text_layout_get_content_size`.

### Info line positioning (single-line per block)
- Large overlay: top nudge +5px, bottom nudge -8px
- Small overlay: top nudge +5px, bottom nudge -1px
- Double-line: large nudge -2px top / -7px bottom; small nudge 0

---

## 7. CONFIG PAGE (config.js)

### Color model — 17 independent slots
```
TimeColor
LitHourColor, LitMinuteColor, LitBatteryColor, LitStepsColor
HourTipColor, MinuteTipColor
DimHourColor, DimMinuteColor, DimBatteryColor, DimStepsColor
Line1Color, Line2Color, Line3Color, Line4Color
BackgroundColor, OverlayColor
```

### Cascade hierarchy
```
LitAll    → LitHourColor + LitMinuteColor + LitBatteryColor + LitStepsColor + HourTipColor + MinuteTipColor
LitTicks  → LitHourColor + LitMinuteColor + HourTipColor + MinuteTipColor
LitHourColor   → LitHourColor + HourTipColor
LitMinuteColor → LitMinuteColor + MinuteTipColor
LitRing   → LitBatteryColor + LitStepsColor
DimAll    → DimHourColor + DimMinuteColor + DimBatteryColor + DimStepsColor
DimTicks  → DimHourColor + DimMinuteColor
DimRing   → DimBatteryColor + DimStepsColor
TextAll   → TimeColor + Line1Color + Line2Color + Line3Color + Line4Color
InfoLinesAll → Line1Color + Line2Color + Line3Color + Line4Color
BaseAll   → BackgroundColor + OverlayColor
```

### UI sections
1. **Info Overlay** — 4 radio buttons: Always On / Always Off / Shake / 1 min (OVERLAY_AUTO)
2. **Info Lines** — 4 dropdowns (Line1–4), inner lines get full field list, outer lines get compact list
3. **Presets** — 40 presets in 5 rows of 8
4. **Colors** — 4-level expandable tree (Text, Lit, Unlit, Base)
5. **Display** — InvertBW toggle (B&W only)
6. **Outer Ring** — ShowRing toggle
7. **Health** — StepGoal slider (hidden on aplite)
8. **Save to Watch** button

### Large overlay toggle
Shown only when `platform === 'emery' || platform === 'chalk'`  
(`chalk` = gabbro in the CloudPebble simulator)

### Field options
- Inner lines (2 & 3): None, Day, Date, Day+Date, Steps, Temp(F), Temp(C), Battery, Distance, Calories
- Outer lines (1 & 4): None, Date, Steps, Temp(F), Temp(C), Battery, Distance, Calories

### Settings persistence
- `index.js` uses `localStorage` with key `'radium2_settings'`
- `location.search` is NOT reliable for Pebble settings — localStorage is the correct approach

### Platform detection (in config.js)
- `platform === 'aplite'` → hide health slider, ring unchecked by default
- `platform === 'bw'` (aplite/diorite/flint) → hide color section, show B&W section
- `platform === 'emery' || 'chalk'` → show large overlay toggle

---

## 8. PRESET SYSTEM (40 presets, 5 groups of 8)

All presets define: bg, obg, tt, lH, lM, lB, lS, dH, dM, dB, dS, tH, tM, l1, l2, l3, l4

**Dark (0–7):** Radium, Scarlet, Ember, Cobalt, Volt, Slate, Violet, Dusk  
**Dark+ (8–15):** Ocean, Aurora, Solar, Venom, Reactor, Neon, Blossom, Jungle  
**Light (16–23):** Paper, Jade, Sapphire, Ruby, Mint, Rose, Lavender, Sepia  
**Color (24–31):** Teal, Flame, Midnight, Forest, Plum, Poison, Ultraviolet, Ash  
**Special (32–39):** Boreal, Cosmos, Prism, Inferno, Triadic, GoldEye, Rainbow, Radium+

### Dim color convention (established v2.2)
- Monochromatic dark themes: dim = dark shade of the same hue family (e.g. #550000 for red, not gray)
- Achromatic themes (Slate, Ash): dim = DarkGray (#555555) — only exceptions
- Light bg themes: dim = pale/pastel version of the accent color
- Split themes: each channel's dim = dark of that channel's hue

### Key preset notes
- **Radium** (Dark #1, system default): GColorGreen (#00ff00) lit everywhere, GColorMintGreen (#aaffaa) tips
- **Radium+** (Special #8): green hours/battery + cyan minutes/steps, white tips, DukeBlue (#0000aa) dim minutes
- **Volt**: SpringBud/Yellow split hours/minutes, PastelYellow tips for contrast
- **Dusk**: pure Magenta (#ff00ff) — sharpened from #ff55ff
- **Violet**: VividViolet (#aa00ff) mono — fills indigo gap (replaced Crimson)
- **Jungle**: BrightGreen/Malachite analogous — fills yellow-green gap (replaced Hearth)
- **Lavender**: VividViolet on white — fills purple gap in Light row (replaced Navy)
- **Poison**: SpringBud/Malachite on ImperialPurple — max complement contrast (replaced Cinnabar)
- **Prism**: Red hours / Cyan minutes / SpringBud battery / VividViolet steps — full spectrum quad (replaced Horizon)
- **GoldEye**: all 4 info lines = #aaff00 (matching lit outer ring)
- Preset pip: split gradient showing lH (left) / lM (right); solid if same

---

## 9. HEALTH DATA (PBL_HEALTH guard)

All health metrics are gated with `#if defined(PBL_HEALTH)` — platforms without health (aplite, flint) compile cleanly.

`update_health_data()` reads all three metrics together on every `HealthEventMovementUpdate`:
- **Steps** → `HealthMetricStepCount` → `s_steps` → `s_steps_buffer` ("12,345")
- **Distance** → `HealthMetricWalkedDistanceMeters` → `s_distance_m` → locale check:
  `strcmp(i18n_get_system_locale(), "en_US") == 0` → miles, otherwise km
- **Calories** → `HealthMetricActiveKCalories` → `s_calories` → `s_calories_buffer` ("847cal")

HR note: basalt (PT), diorite (PT Steel), and emery (PT2) have optical HR sensors. Gabbro (Round 2) does NOT have HR. Heart rate is not yet implemented.

---

## 10. WEATHER (phone → watch via AppMessage)

Weather data is fetched by `index.js` (phone JS) using Open-Meteo API and sent to the watch via AppMessage:
- `WeatherTempF` → `s_weather_temp_f` (INT_MIN = not yet received)
- `WeatherTempC` → `s_weather_temp_c`
- `WeatherCode` → `s_weather_code` (WMO code, mapped to icon type 0–5)

Temperature display: plain `"72F"` / `"22C"` — NO degree symbol (0xB0 in format string causes weather text to silently not render).

Weather icon types: 0=sun, 1=partly-cloudy, 2=cloud, 3=rain, 4=snow, 5=storm (mapped from WMO codes in `weather_icon_for_code()`).

---

## 11. CLOUDPEBBLE / BUILD RULES (CRITICAL)

1. **Remove `resources/media` block** from appinfo.json — causes "Unsupported published resource type" build errors
2. **Menu icons** via CloudPebble UI only, not GitHub import
3. **No tilde (~) in resource filenames** — breaks CloudPebble GitHub import
4. CloudPebble imports from GitHub `master` branch; Sterling pastes raw content manually
5. **Always give Sterling full files** to paste — never surgical diffs or partial replacements
6. `chalk` = gabbro in the CloudPebble simulator

### Platform table
| Platform | Watch | Screen | Colors | Health |
|----------|-------|--------|--------|--------|
| aplite | Pebble Classic / Steel | 144×168 rect | B&W | No |
| basalt | Pebble Time | 144×168 rect | 64 color | Yes |
| chalk | Pebble Time Round | 180×180 round | 64 color | Yes |
| diorite | Pebble 2 SE | 144×168 rect | B&W | Yes |
| emery | Pebble Time 2 | 200×228 rect | 64 color | Yes |
| flint | Pebble 2 | 144×168 rect | B&W | No |
| gabbro | Pebble Round 2 (Core Devices, 2026) | 260×260 round | 64 color | Yes |

**gabbro** is the Pebble Round 2 — 260×260 circular color e-paper, round rendering, health-capable. NOT B&W, NOT rect. `chalk` = gabbro in CloudPebble simulator.

---

## 12. KEY BUGS FIXED (history)

| Bug | Fix |
|-----|-----|
| Platform detection inversion (B&W vs color) | Fixed in v2.0 |
| Settings not persisting across config opens | Fixed: localStorage in index.js |
| Overlay default was "always on" | Changed to OVERLAY_SHAKE |
| Noon/midnight bug (showed 0 instead of 12) | `(s_hour % 12) ?: 12` |
| Tick rasterization gaps on B&W | Group-based rendering, 2° B&W ticks |
| Round battery ring going wrong way | Fixed direction logic |
| 24h separator sizing too large | 3° cut (perfect thirds of 9° slot) |
| `prv_overlay_auto_hide` undeclared identifier | Forward declaration added before inbox_received |
| Degree symbol in weather format string | Removed — use plain "72F"/"22C" |
| gabbro listed as B&W in platform table | Corrected: gabbro is color + round + health |
| Calories icon small: 1px overflow on bottom | base rect height 4→3 (fits 11px slot) |
| Round watch hour tip off-by-one in 12h mode | Added `if (!is_24h)` branch in round section |
| Ember/Cobalt dim using neutral gray | Fixed: Ember dim = #550000, Cobalt dim = #000055 |

---

## 13. DESIGN PHILOSOPHY

- **Radial bar graph:** The watchface IS a bar graph, arranged radially. Minutes = top half, hours = bottom half. No hands, no numerals.
- **Overlay / art mode duality:** Center circle shows day/time/date. Without it: pure starburst geometry. Four overlay modes: Always On, Always Off, Shake, 1 min (auto-hide).
- **"Radium" naming:** Accidental pun — glowing green dial + "radial" design. Both are correct.
- **Ring toggle:** ShowRing=false → ticks extend to screen edge → pure starburst mode.
- **Nine-year backstory:** Concept from 2015, finally properly realized in 2026.
- **64-color palette structure:** 2 bits per channel (0x00/0x55/0xAA/0xFF). Hue families form natural columns: bright/mid/dark within each hue. Preset design follows: lit=bright, tip=mid, dim=dark.

---

## 14. BANNER / STORE ASSETS

**Banner script:** `assets/banner_mockup.py` — Python/Pillow  
**Banner PNG:** `assets/radium2_banner_mockup.png` — committed to repo

Design: 470×320px, text left / watch mockup right, blurred starburst bg, mint glow on "Ra".

---

## 15. OPEN ITEMS / FUTURE WORK (post-v2.2)

1. **Store submission** — v2.2 is ready; needs final CloudPebble build + submission to Rebble portal
2. **Store listing update** — STORE_LISTING.md needs v2.2 feature update
3. **Heart rate** — basalt/diorite/emery have HR sensors; implement once tested on real hardware
4. **Sleep data** — available via `HealthMetricSleepSeconds`; lower priority
5. **Banner update** — may want new banner image for v2.2
6. **Monogram watchface** — 40 digit bitmap assets to be designed in Photoshop, then JS/TypeScript implementation using Alloy SDK

---

## 16. DEV ENVIRONMENT

- **CloudPebble:** https://cloudpebble.repebble.com — primary build/test (Core Devices account)
- **GitHub MCP connector:** Live on Mac desktop — Claude commits directly to GitHub
- **Rebble Developer Portal:** https://dev.rebble.io — for store submissions
- **Python / Pillow:** Banner/asset generation
- **Alloy/XS SDK:** For Monogram (JavaScript/TypeScript, ES2025, Moddable's XS engine)

---

## 17. QUICK REFERENCE

```
Repo:         https://github.com/SterlingEly/Radium2
Branch:       master
HEAD:         374cc2ae
Live store:   https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926
UUID:         2609e817-f8f2-4ad2-8846-cb05bb67d047
Version:      2.2.0 (ready to submit) — 2.1 is live
SETTINGS_KEY: 7
messageKeys:  29 (BackgroundColor … WeatherCode)
github:push_files preferred for large files; create_or_update_file fails on large payloads
```

---

*End of Radium 2 context seed. v2.1 is live; v2.2 is complete and ready to submit.*

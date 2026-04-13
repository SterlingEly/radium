# RADIUM 2 — CONTEXT SEED FOR NEW THREAD
*Everything a fresh Claude session needs to resume Radium 2.x development*

---

## 1. WHAT IS THIS PROJECT?

**Radium 2** is a Pebble watchface built around a *radial bar graph* concept — the entire screen is a starburst of wedge-shaped tick marks radiating from the center, hours on the bottom half, minutes on the top. Design by Sterling Ely (originated ~2015), rebuilt from scratch in 2026 with Claude as technical partner.

**Sterling:** Design/concept lead. **Claude:** Code, GitHub commits, documentation.

---

## 2. HISTORY

| Project | Year | Designer | Developer |
|---------|------|----------|-----------|
| Bar Graph v1 | 2013–2014 | Sterling Ely | Cameron MacFarland |
| Radium v1 | 2015–2016 | Sterling Ely | MathewReiss + MicroByte |
| Radium 2 | 2026 | Sterling Ely | Sterling Ely + Claude |

Original Radium repo: https://github.com/MathewReiss/radium

---

## 3. LIVE STATUS

- **v2.2 is LIVE** on Rebble/Repebble store
- Store URL: https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926
- GitHub repo: https://github.com/SterlingEly/Radium2 (branch: `master`)
- **v2.3 is in development on `master`** (not yet submitted to store)

---

## 4. REPO STRUCTURE

```
SterlingEly/Radium2 (master)
├── CHANGELOG.md
├── README.md
├── STORE_LISTING.md
├── CONTEXT_RADIUM2.md     ← this file
├── package.json           ← appinfo equivalent
├── wscript
└── src/
    ├── c/
    │   └── main.c         ← ~1650 lines, entire watchface
    └── pkjs/
        ├── config.js      ← config page (data URL)
        └── index.js       ← PebbleKit JS: weather + solar fetch, settings relay
```

---

## 5. VERSION SPEC

### v2.3 (in development)
- `SETTINGS_KEY = 8` (bumped from 7; added `RingMode`)
- `SOLAR_KEY = 9` (separate persist key for solar timestamps)
- **messageKeys** (33 total): all v2.2 keys plus `RingMode`, `SunriseTime`, `SunsetTime`, `SunriseTomorrow`
- New info line fields: `FIELD_BT=10`, `FIELD_HEART_RATE=11`, `FIELD_SUNRISE=12`, `FIELD_SUNSET=13`, `FIELD_DAYLIGHT=14`
- New ring mode: `RING_SOLAR=1` (right=day progress, left=night progress)

### v2.2 (live)
- `SETTINGS_KEY = 7`
- **messageKeys** (29): BackgroundColor … WeatherCode
- `uuid`: `2609e817-f8f2-4ad2-8846-cb05bb67d047`

---

## 6. C CONSTANTS (main.c, v2.3)

```c
#define SETTINGS_KEY      8
#define SOLAR_KEY         9
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2
#define RING_THICK        6

#define OVERLAY_ALWAYS_ON   0
#define OVERLAY_OFF         1
#define OVERLAY_SHAKE       2
#define OVERLAY_AUTO        3
#define OVERLAY_AUTO_HIDE_MS  60000
#define OVERLAY_SMALL  0
#define OVERLAY_LARGE  1

// Info line field IDs
#define FIELD_NONE      0
#define FIELD_DAY_LONG  1   // "SATURDAY"
#define FIELD_DATE      2   // "MAR 21"
#define FIELD_DAY_DATE  3   // "SAT MAR 21"
#define FIELD_STEPS     4   // footprint + step count
#define FIELD_TEMP_F    5   // weather icon + °F
#define FIELD_TEMP_C    6   // weather icon + °C
#define FIELD_BATTERY   7   // battery icon + %
#define FIELD_DISTANCE  8   // footprint + mi/km
#define FIELD_CALORIES  9   // flame + kcal
#define FIELD_BT        10  // BT rune+! when disconnected
#define FIELD_HEART_RATE 11 // heart + BPM
#define FIELD_SUNRISE   12  // sun + "6:23am"
#define FIELD_SUNSET    13  // sun + "7:41pm"
#define FIELD_DAYLIGHT  14  // sun + "13h18m"

#define RING_STEPS_BATTERY  0
#define RING_SOLAR          1
```

---

## 7. SETTINGS STRUCT (v2.3)

```c
typedef struct {
  GColor BackgroundColor, OverlayColor, TimeColor;
  GColor LitHourColor, LitMinuteColor, LitBatteryColor, LitStepsColor;
  GColor DimHourColor, DimMinuteColor, DimBatteryColor, DimStepsColor;
  GColor HourTipColor, MinuteTipColor;
  GColor Line1Color, Line2Color, Line3Color, Line4Color;
  int Line1Field, Line2Field, Line3Field, Line4Field;
  int StepGoal, OverlayMode, OverlaySize;
  bool InvertBW, ShowRing;
  int RingMode;   // NEW in v2.3
} RadiumSettings;
```

Defaults: Radium preset colors (GColorGreen lit, GColorDarkGreen dim, GColorMintGreen tips), OVERLAY_SHAKE, OVERLAY_SMALL (LARGE on emery/gabbro), ShowRing=false on aplite, RingMode=RING_STEPS_BATTERY.

---

## 8. SOLAR DATA SYSTEM (v2.3)

### Solar cache (SOLAR_KEY=9)
```c
typedef struct { time_t sunrise; time_t sunset; time_t sunrise_tomorrow; } SolarCache;
```
- **Load:** Always restores any cached data (no stale gate on boot)
- **Display gate:** `prv_solar_present()` = `s_sunrise_tomorrow > 0` — never expires once received
- **Fetch gate:** `prv_solar_valid()` = within 36h of `s_sunrise_tomorrow` — triggers JS re-fetch on reconnect

### Stale data ring math
The ring uses `eff_*` timestamps computed by rolling forward from cached values:
```c
time_t eff_sunrise = s_sunrise, eff_sunset = s_sunset, eff_sunrise_tomorrow = s_sunrise_tomorrow;
// Roll forward one day at a time until the window is current (~1min/day drift, negligible)
while (now_t > eff_sunrise_tomorrow) {
    time_t day_dur       = eff_sunset - eff_sunrise;
    eff_sunrise          = eff_sunrise_tomorrow;
    eff_sunset           = eff_sunrise_tomorrow + day_dur;
    eff_sunrise_tomorrow = eff_sunrise_tomorrow + 86400;
}
```
This keeps ring math valid for any number of days of stale data. Info lines and ring therefore degrade together (both persist indefinitely; both show `--` only before first sync).

### JS (index.js) — Open-Meteo request
```
?latitude=…&longitude=…
&current=temperature_2m,weather_code
&daily=sunrise,sunset
&timezone=auto
&forecast_days=2
```
Sends: `SunriseTime`, `SunsetTime`, `SunriseTomorrow` (Unix timestamps), `WeatherTempF`, `WeatherTempC`, `WeatherCode`.

### Solar ring fill direction
- **Right arc (day):** 100% at sunrise, drains CW toward 6-o'clock (same as battery arc)
- **Left arc (night):** 100% at sunset, drains CW toward 6-o'clock — anchored at 12-end (opposite of steps arc)
- During daytime: `left_pct = 0`; during nighttime: `right_pct = 0`

---

## 9. DRAWING ARCHITECTURE

### Layer stack (bottom to top)
1. Background fill
2. Tick wedges / radial arcs (minutes top half, hours bottom half)
3. Inner gap strip (rect only)
4. Outer ring: right=battery/day, left=steps/night
5. Center overlay circle
6. Info lines + time digits

### Tick geometry — RECT
- **Minutes:** 12 groups × 5; 15° pitch (9° tick + 6° gap); start at 3°. Color: 1° sub-ticks with bg cuts. B&W: 2° solid blocks.
- **Hours:** 12 slots; 15° pitch; start at 183°. 12h: solid 9°/slot. 24h: two 3° sub-ticks + 3° gap.
- Thickness: `inner_short * 19/164` (overlay on); full `inner_short` (overlay off)

### Tick geometry — ROUND
- `graphics_fill_radial()` on inset tick_rect
- 60 × 1° minute arcs; 12 × 9° hour arcs

### Outer ring — RECT
- `gap=5`, `RING_THICK=6`, `half_w=cx-gap`, `total=half_w+h+half_w`
- Battery/day (right): left-anchored at `(cx+gap, h-t)`, fills CW (bottom→right→top)
- Steps (left): right-anchored at `(cx-gap, h-t)`, fills CCW (bottom→left→top)
- Solar night (left): anchored at 12-end, fills toward 6 (opposite of steps)

### Overlay
- Small: 58px, LECO_36_BOLD, GOTHIC_18_BOLD
- Large: 70px, LECO_42, GOTHIC_24_BOLD (default on emery/gabbro)
- Icon+text: dynamic centering via `graphics_text_layout_get_content_size`

---

## 10. ICONS (drawn in C, two sizes: 11px small / 14px large)

| Icon | Function | Notes |
|------|----------|-------|
| Footprint pair | `draw_steps_icon` | Steps + distance |
| Battery | `draw_battery_icon` | Fill level; charging = lightning bolt |
| BT rune | `draw_bt_icon` | BT symbol + `!`; invisible when connected |
| Heart | `draw_heart_icon` | Heart rate |
| Flame | `draw_calories_icon` | Active calories |
| Sun | `draw_sun_icon` | Solar fields; drawn at `iy-1` for vertical alignment |
| Weather | `draw_weather_icon` | Dispatches: sun/partly-cloudy/cloud/rain/snow/storm |

**BT icon pixel notes (11px):**
- Spine at col 3, chevron peak at col 6
- Upper-left diagonal: `(ox+2, oy+4)`, `(ox+1, oy+3)`
- Lower-left diagonal: `(ox+2, oy+7)`, `(ox+1, oy+8)`
- Exclamation stem rows 1–6, dot: 4 explicit pixels at `(ox+9..10, oy+8..9)`
  (NOT `fill_rect` — causes diagonal artifact on e-paper at stroke context)

---

## 11. CONFIG PAGE (config.js)

### Color slots (17)
TimeColor · LitHourColor · LitMinuteColor · LitBatteryColor · LitStepsColor · HourTipColor · MinuteTipColor · DimHourColor · DimMinuteColor · DimBatteryColor · DimStepsColor · Line1–4Color · BackgroundColor · OverlayColor

### Cascade hierarchy
```
LitAll → all lit + tips       LitTicks → Lit H/M + tips
LitHourColor → Lit + HourTip  LitMinuteColor → Lit + MinuteTip
LitRing → LitBattery+Steps    DimAll → all dim
DimTicks → Dim H/M            DimRing → Dim Battery+Steps
TextAll → TimeColor + Line1-4  InfoLinesAll → Line1-4
BaseAll → Background + Overlay
```

### Field options (v2.3)
- Inner lines: None, Day, Date, Day+Date, Steps, Temp°F, Temp°C, Battery, Distance, Calories, Heart Rate, Sunrise, Sunset, Daylight, BT
- Outer lines: same compact list (no Day/Date text fields)

### Settings persistence
- `localStorage` key `'radium2_settings'` in `index.js`

---

## 12. PRESET SYSTEM (40 presets, 5 × 8)

**Dark (0–7):** Radium, Scarlet, Ember, Cobalt, Volt, Slate, Violet, Dusk
**Dark+ (8–15):** Ocean, Aurora, Solar, Venom, Reactor, Neon, Blossom, Jungle
**Light (16–23):** Paper, Jade, Sapphire, Ruby, Mint, Rose, Lavender, Sepia
**Color (24–31):** Teal, Flame, Midnight, Forest, Plum, Poison, Ultraviolet, Ash
**Special (32–39):** Boreal, Cosmos, Prism, Inferno, Triadic, GoldEye, Rainbow, Radium+

**Dim convention:** monochromatic = dark same-hue; achromatic (Slate, Ash) = DarkGray #555555; light-bg = pale accent; split = dark of each channel's hue.

**Key presets:**
- **Radium** (slot 0, default): GColorGreen lit, GColorMintGreen tips, GColorDarkGreen dim
- **Radium+** (slot 39): green hours/battery + cyan minutes/steps, white tips

---

## 13. CLOUDPEBBLE / BUILD RULES

1. Remove `resources/media` block from appinfo.json
2. Menu icons via CloudPebble UI only
3. No tilde in resource filenames
4. CloudPebble import is additive only — use manual copy-paste for code updates
5. Always give full files for copy-paste; never partial diffs

### Platform table
| Platform | Watch | Screen | Colors | Health | Touch |
|----------|-------|--------|--------|--------|-------|
| aplite | Pebble Classic/Steel | 144×168 rect | B&W | No | No |
| basalt | Pebble Time | 144×168 rect | 64-color | Yes | No |
| chalk | Pebble Time Round | 180×180 round | 64-color | Yes | No |
| diorite | Pebble 2 SE | 144×168 rect | B&W | Yes | No |
| emery | Pebble Time 2 | 200×228 rect | 64-color | Yes | **Yes** |
| flint | Pebble 2 | 144×168 rect | B&W | Yes | No |
| gabbro | Pebble Round 2 (Core Devices) | 260×260 round | 64-color | Yes | No |

Note: `chalk` = gabbro in CloudPebble simulator. Only emery has touchscreen.

---

## 14. KEY BUGS FIXED

| Bug | Fixed |
|-----|-------|
| Battery ring bottom fill right-anchored from corner | v2.2 |
| Round hour tick off-by-one in 12h mode | v2.2 |
| Degree symbol in weather string caused silent render failure | v2.2 |
| Calories icon 1px overflow on small overlay | v2.2 |
| BT icon exclamation dot: diagonal artifact on e-paper | v2.3 |
| Solar ring dying after 1 day stale (single `if`) | v2.3 |
| Solar ring dying after 2+ days (changed to `while` loop) | v2.3 |

---

## 15. GITHUB MCP NOTES

- `create_or_update_file` for all pushes — requires current file SHA
- `push_files` tool sends empty content — do NOT use it
- Cannot delete files via MCP — use GitHub web UI
- Radium2 repo uses `master` branch

---

## 16. QUICK REFERENCE

```
Repo:         https://github.com/SterlingEly/Radium2
Branch:       master
Live store:   https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926
UUID:         2609e817-f8f2-4ad2-8846-cb05bb67d047
v2.2 live:    SETTINGS_KEY=7, 29 messageKeys
v2.3 dev:     SETTINGS_KEY=8, SOLAR_KEY=9, 33 messageKeys
```

---

*End of context seed. v2.2 live; v2.3 in development on master.*

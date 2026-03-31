# Radium 2 — Changelog

---

## v2.2

### Preset overhaul
- 5 new themes: Violet (indigo mono), Jungle (yellow-green analogous), Lavender (violet on white), Poison (green on purple), Prism (full-spectrum quad split)
- 7 themes tweaked: Volt (split lH/lM, better tips), Dusk (sharpened to pure Magenta), Ember/Cobalt (hue-family dim), Jade/Sapphire/Ruby (accent-toned dim on light backgrounds)
- Retired: Crimson, Hearth, Navy, Cinnabar, Horizon (all near-duplicates of other presets)
- Principled dim color rule established: monochromatic themes use dark shade of same hue; only Slate and Ash use neutral gray

### Info lines — 4 configurable data fields
- 4 independently configurable info lines (2 above, 2 below the time)
- Each line can display: None, Day, Date, Day+Date, Steps, Distance, Calories, Temp °F, Temp °C, or Battery
- Dynamic centering via `graphics_text_layout_get_content_size` — icon+text units always centered on overlay
- Default layout: None / Day / Date / None

### Live weather
- Phone JS fetches current weather from Open-Meteo (free, no API key)
- Sends temperature (°F and °C) + WMO weather code to watch via AppMessage
- 6 custom weather icons in C at both overlay sizes: sun, partly cloudy, cloud, rain, snow, storm
- Temperature display uses plain ASCII (e.g. "72F") — no degree symbol

### Health fields
- Distance: walked distance today in mi or km, auto-detected via locale
- Active calories: active kcal burned today (excludes resting metabolic rate), custom flame icon
- Steps + distance + calories consolidated into single update per movement event

### Overlay improvements
- Two overlay sizes: Small (58px, LECO_36_BOLD) and Large (70px, LECO_42)
- Large is default on emery/gabbro (Pebble Time 2 and Round 2)
- 1-min mode (OVERLAY_AUTO): shake to show, auto-hides after 60 seconds

### Color system
- 17 color slots (up from 12): added HourTipColor, MinuteTipColor, Line1–4Color
- Leading-tick highlights: current hour and minute ticks independently colorable
- Cascade: setting Hours/Minutes also sets their respective tip colors

### Bug fixes
- Rect ring: battery fill at low charge was filling from the corner inward instead of
  from the 6-o’clock origin outward, leaving a dark gap at the origin (affects all rect
  platforms, color and B&W; present since v2.0)
- Round watch: leading hour tick off-by-one in 12h mode (wrong slot formula)
- Calories flame icon: 1px overflow on small overlay (base rect height 4→3)
- Weather text: removed degree symbol from format string (caused silent render failure)
- Distance locale: switched to `i18n_get_system_locale()` for correct mi/km detection
- Forward declaration for `prv_overlay_auto_hide` (undeclared identifier on gabbro)

---

## v2.1 (2026-03-11)

- Initial public release on Rebble appstore
- 40 color presets across five rows (Dark, Dark+, Light, Color, Special)
- 12 color slots, cascade color picker
- 3 overlay modes: Always On, Shake, Always Off
- Battery + steps outer ring
- 24h clock support, all 7 Pebble platforms
- B&W invert option

---

## v2.0 (2026-03-03, internal)

- From-scratch rebuild by Sterling Ely + Claude
- Full color customization, cross-platform layout (rect + round)
- Config page with preset system
- 24 color presets (Dark, Light, Color), 9 color slots

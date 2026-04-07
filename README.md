# Radium 2

**A radium dial watchface for Pebble — glowing tick marks, clean geometry, and a hidden clock.**

---

## The idea

Old radium dial watches are beautiful objects. The hour and minute markings glow independently of the hands, giving you a sense of time without needing to parse a needle position. Radium 2 brings that idea to Pebble: the right half of the screen fills with minute ticks, the left half with hour blocks, a thin outer ring tracks battery and steps — and in the center, optionally, a clean digital readout.

The face works on two levels. With the overlay on, it reads like a normal watchface: time, date, day. Shake it, and the overlay disappears to reveal the full radial display — a pure starburst of glowing geometry that tells time in a completely different way. Shake again to bring it back.

---

## Features

- **Radial time display** — right half: 60 minute ticks in groups of 5. Left half: 12 hour blocks (or 24 in 24h mode, each block split in two)
- **Overlay modes** — Always On, Shake, 1 min auto-hide, or Always Off (pure art mode)
- **4 configurable info lines** — choose from: Day, Date, Day+Date, Steps, Distance, Calories, Temp °F, Temp °C, Battery, Bluetooth status, Heart rate, or None
- **Live weather** — current conditions fetched from Open-Meteo via phone; displayed with a small custom icon
- **Outer ring** — battery level on the right, step count on the left, both filling from 6 o'clock toward 12; hideable
- **Leading tick highlights** — the current hour and minute ticks glow brighter, independently colorable
- **Charging indicator** — battery icon swaps to a lightning bolt when the watch is on the charger
- **Bluetooth alert** — Bluetooth rune appears on a configured info line when phone connection is lost, with a double vibration alert
- **Heart rate** — live BPM display on supported hardware (Pebble Time, Time 2, Pebble 2)
- **24h support** — each hour slot splits into two segments with a gap between them
- **17 independently configurable color slots** across time, ticks, tips, ring, info lines, and base
- **40 presets** in five rows (Dark, Dark+, Light, Color, Special) — one tap to apply
- **B&W support** — Aplite, Diorite, and Flint get a clean high-contrast layout with optional invert toggle
- **All 7 platforms** — Aplite, Basalt, Chalk, Diorite, Emery, Flint, Gabbro

---

## Reading the face

```
         12
    ███░░░░░░░░░
  █               ░
 █    WEDNESDAY    ░
 █     10:42       ░
 █     MAR 09      ░
  █               ░
    ░░░████████
         6
```

- **Right half (3→12 o'clock):** minutes, filling clockwise. Each group of 5 ticks is one 5-minute block
- **Left half (9→12 o'clock):** hours, filling counter-clockwise. Each block = 1 hour (12h) or half-block = 1 hour (24h)
- **Outer ring right:** battery, filling up from 6 toward 12
- **Outer ring left:** steps toward daily step goal, filling up from 6 toward 12
- **Center:** configurable info lines above and below the time, with optional overlay circle

---

## Overlay modes

| Mode | Behavior |
|---|---|
| Always On | Overlay always visible |
| Shake | Starts visible; shake toggles. Resets to visible on reboot |
| 1 min | Shake to show; auto-hides after 60 seconds |
| Always Off | Pure art mode — full starburst, no center hole |

---

## Info lines

Four independently configurable lines surround the time display (two above, two below). Each can show:

| Option | Display |
|---|---|
| None | blank |
| Day | "WEDNESDAY" |
| Date | "MAR 21" |
| Day + Date | "WED MAR 21" |
| Steps | footprint icon + step count |
| Distance | footprint icon + walked distance (mi or km) |
| Calories | flame icon + active kcal |
| Temp °F | weather icon + temperature |
| Temp °C | weather icon + temperature |
| Battery | battery icon + charge % (bolt when charging) |
| Bluetooth | BT rune — visible when disconnected, blank when connected |
| Heart rate | heart icon + BPM ("72bpm" or "--") |

---

## Platforms

| Platform | Watch | Screen | Notes |
|---|---|---|---|
| Aplite | Pebble Classic, Steel | 144×168 B&W | High-contrast, invert option, no health |
| Basalt | Pebble Time | 144×168 color | Heart rate supported |
| Chalk | Pebble Time Round | 180×180 color | Round rendering |
| Diorite | Pebble 2 SE | 144×168 B&W | High-contrast, invert option, heart rate supported |
| Emery | Pebble Time 2 | 200×228 color | Large overlay default, heart rate supported |
| Flint | Pebble 2 | 144×168 B&W | High-contrast, invert option, heart rate supported |
| Gabbro | Pebble Round 2 | 260×260 color | Round rendering, large overlay default |

---

## History & credits

**December 2015 — Original design (Sterling Ely)**  
The concept was designed by Sterling Ely for the Pebble Time Round. The core idea: a radial bar graph where filled wedge segments encode time, battery, and steps — readable as pure geometry even without a digital readout.

**December 9, 2015 — Prototype implementation (MathewReiss)**  
GitHub: [MathewReiss/radium](https://github.com/MathewReiss/radium)

**December 9, 2016 — v1.0 release (MicroByte)**  
[apps.rebble.io](https://apps.rebble.io/en_US/application/584b212dce45dc907d00008f)

**March 2026 — Radium 2 (Sterling Ely & Claude)**  
Full rebuild for all modern Pebble platforms. Adds full color customization, 40 presets, 24h mode, 4 configurable info lines, live weather, health metrics, and improved cross-platform layout.  
GitHub: [SterlingEly/Radium2](https://github.com/SterlingEly/Radium2)  
Appstore: [apps.rebble.io](https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926)

---

## Store

- **Rebble Appstore:** [apps.rebble.io](https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926)
- **Pebble Appstore:** [apps.repebble.com](https://apps.repebble.com/en_US/application/69a6531826cc4f0009c65926)

---

## Building

Built with the Pebble SDK via CloudPebble. No external dependencies.

```
pebble build
pebble install --emulator basalt
```

Source files:
- `src/c/main.c` — all drawing, event handling, settings persistence, health & weather data
- `src/pkjs/config.js` — config page HTML/JS (built as a data URL)
- `src/pkjs/index.js` — PebbleKit JS: platform detection, settings relay, weather fetch
- `package.json` — message keys, target platforms, version

Note: `resources/fonts/` contains Roboto font files from an earlier iteration. They are not used by the current build and can be safely deleted.

---

## License

MIT

# Radium 2

**A radium dial watchface for Pebble — glowing tick marks, clean geometry, and a hidden clock.**

---

## The idea

Old radium dial watches are beautiful objects. The hour and minute markings glow independently of the hands, giving you a sense of time without needing to parse a needle position. Radium 2 brings that idea to Pebble: the right half of the screen fills with minute ticks, the left half with hour blocks, a thin outer ring tracks battery and steps — and in the center, optionally, a clean digital readout.

The face works on two levels. With the overlay on, it reads like a normal watchface: time, date, day. Shake it, and the overlay disappears to reveal the full radial display — a pure starburst of glowing geometry that tells time in a completely different way. Shake again to bring it back.

---

## Features

- **Radial time display** — right half: 60 minute ticks in groups of 5. Left half: 12 hour blocks (or 24 in 24h mode, each block split in two)
- **Overlay** — day, time, and date in the center circle; always-on, always-off, or shake-to-toggle
- **Art mode** — set overlay to Always Off and the tick ring fills the whole face with no center hole
- **Outer ring** — battery level on the right, step count on the left, both filling from 6 o'clock toward 12; hideable
- **24h support** — each hour slot splits into two segments with a gap between them
- **Full color customization** — 12 independently configurable color slots across text, lit elements, unlit elements, and base colors
- **40 presets** in five rows (Dark, Dark+, Light, Color, Special) — one tap to apply all 12 colors at once
- **B&W support** — Aplite, Diorite, and Flint get a clean high-contrast layout with optional invert toggle
- **All platforms** — Aplite, Basalt, Chalk, Diorite, Emery, Flint, Gabbro

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
- **Outer ring left:** steps toward daily goal, filling up from 6 toward 12
- **Center:** day name, time, date — hidden in art mode

---

## Overlay modes

| Mode | Behavior |
|---|---|
| Always On | Overlay always visible |
| Shake | Starts visible; shake toggles. Resets to visible on reboot |
| Always Off | Pure art mode — full starburst, no center hole |

---

## Platforms

| Platform | Watch | Screen | Notes |
|---|---|---|---|
| Aplite | Pebble Classic, Steel | 144×168 B&W | High-contrast, invert option |
| Basalt | Pebble Time | 144×168 color | |
| Chalk | Pebble Time Round | 180×180 color | Round rendering |
| Diorite | Pebble 2 | 144×168 B&W | High-contrast, invert option |
| Emery | Pebble Time 2 | 200×228 color | Larger overlay circle |
| Flint | Pebble 2 Duo | 144×168 B&W | High-contrast, invert option |
| Gabbro | Pebble Round 2 | 260×260 color | Round rendering |

---

## History & credits

**December 2015 — Original design (Sterling Ely)**
The concept was designed by Sterling Ely for the Pebble Time Round. The core idea: a radial bar graph where filled wedge segments encode time, battery, and steps — readable as pure geometry even without a digital readout. The name "Radium" being a pun on "radial" was a happy accident.

**December 9, 2015 — Prototype implementation (MathewReiss)**
Mathew Reiss built the first implementation based on the design.
GitHub: [MathewReiss/radium](https://github.com/MathewReiss/radium)

**December 9, 2016 — v1.0 release (MicroByte)**
MicroByte completed and published the first full release on the Pebble App Store.
Appstore: [apps.repebble.com](https://apps.repebble.com/en_US/application/584b212dce45dc907d00008f) · [apps.rebble.io](https://apps.rebble.io/en_US/application/584b212dce45dc907d00008f)

**March 2026 — Radium 2 (Sterling Ely & Claude)**
A full rebuild for all modern Pebble platforms, forked from the MathewReiss prototype. Sterling Ely led design and direction; Claude (Anthropic) handled technical implementation. Adds full color preset system, per-element color customization, 24h mode, and improved cross-platform layout.
GitHub: [SterlingEly/Radium2](https://github.com/SterlingEly/Radium2)
Appstore: [apps.repebble.com](https://apps.repebble.com/en_US/application/69a6531826cc4f0009c65926) · [apps.rebble.io](https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926)

---

## Store

- **Pebble Appstore:** [apps.repebble.com](https://apps.repebble.com/en_US/application/69a6531826cc4f0009c65926)
- **Rebble Appstore:** [apps.rebble.io](https://apps.rebble.io/en_US/application/69a6531826cc4f0009c65926)

*Submitted via the Rebble Developer Portal — appears in both stores automatically.*

---

## Building

Built with the Pebble SDK (CloudPebble or local SDK). No external dependencies.

```
pebble build
pebble install --emulator basalt
```

Source files:
- `src/main.c` — all drawing, event handling, settings persistence
- `src/pkjs/config.js` — config page HTML/JS (built as a data URL)
- `src/pkjs/index.js` — PebbleKit JS: platform detection, settings relay
- `appinfo.json` — message keys, target platforms, version

Note: `resources/fonts/` contains Roboto font files left over from an earlier iteration. They are not referenced by the current build (all text uses Pebble system fonts) and can be safely deleted.

---

## License

MIT

# Changelog — Radium 2

All notable changes to this project are documented here.
Versions follow the `versionLabel` in `appinfo.json`.

---

## [2.1] — 2026-03-08

### New features
- **12-color model** (color watches): every visual element now has an independent color slot.
  - *Text (2):* `TimeTextColor`, `DateTextColor`
  - *Lit (4):* `LitHourColor`, `LitMinuteColor`, `LitBatteryColor`, `LitStepsColor`
  - *Unlit (4):* `DimHourColor`, `DimMinuteColor`, `DimBatteryColor`, `DimStepsColor`
  - *Base (2):* `BackgroundColor`, `OverlayBgColor`
- **Independent dim colors**: unlit tick tracks and ring tracks can be styled separately per element (hours vs. minutes, battery vs. steps).
- **Overlay background color** (`OverlayBgColor`): the center overlay circle can now differ from the canvas background, enabling colored-overlay effects.
- **Show/hide outer ring** (`ShowRing` toggle): when hidden, tick wedges/arcs expand to fill the screen edge — pure starburst art mode without losing overlay.
- **40 presets** across five labeled rows (Dark, Dark+, Light, Color, Special), up from 24. New presets include Horizon, Reactor, Venom, Blossom, Solar, Aurora, Neon, Hearth, Boreal, GoldEye, Viper, Inferno, Cosmos, Triadic, Rainbow.
- **Split/quad parent swatches** in the config UI: two-child parent swatches (LitTicks, LitRing, DimTicks, DimRing, TextAll, BaseAll) show a diagonal split; four-child parents (LitAll, DimAll) show a quadrant view — making multi-color presets legible at a glance.
- **Cascade color picker**: tapping a parent swatch (e.g. LitAll, DimTicks) opens the picker and sets all child slots at once.

### Changes
- `SETTINGS_KEY` bumped from `1` to `2` — old persisted settings are not migrated (defaults applied on first run after update).
- `app_message_open` inbox buffer increased from 512 to 768 bytes to accommodate additional message keys.
- Hour tick width changed from 10° to 9° (gap 5° → 6°) for visual consistency with minute ticks; pitch unchanged at 15°.
- 24h mode divider changed from a 2° cut to a 3° cut (even thirds of the 9° tick block).
- Default overlay mode remains **Shake**; overlay starts visible on every reboot.
- Overlay circle now drawn on both rect and round platforms (previously guarded by `!is_round`, silently ignoring `OverlayBgColor` on Chalk/Emery).
- Rect ring gap tuned to 5 px (from 3 px) for cleaner visual separation at the 12 and 6 o'clock positions.
- Round ring arc sweep remains 174° (3°–177° / 183°–357°).
- GoldenEye preset updated: dim hours `#aa5500→#550000`, lit ticks/battery/steps `#aaffaa→#00ff00`.
- Gabbro temporarily removed from `targetPlatforms` due to a store submission issue; draw code remains intact.

### Bug fixes
- Fixed platform detection inversion that caused B&W-platform color logic to run on color watches and vice versa.
- Fixed settings not persisting across app reloads (`localStorage` now used correctly in `index.js`).
- Fixed overlay circle being skipped on round platforms, causing `OverlayBgColor` to have no effect on Chalk/Emery.

---

## [2.0] — 2025

Original v2.0 implementation. Established the core architecture as a full rebuild by Sterling Ely:
- Radial bar graph concept restored from the original Radium watchface (~2016).
- Wedge-based tick drawing for rect platforms; `graphics_fill_radial` arcs for round.
- Outer ring: battery (right half) + steps (left half), 3° gaps at 12 and 6 o'clock.
- Overlay: day name / time / date block, shake-to-toggle.
- 9 color slots, 24 presets (Dark, Light, Color rows).
- Settings persistence via `SETTINGS_KEY 1`.
- Platforms: aplite, basalt, chalk, diorite, emery, flint. (Gabbro added/removed during submission.)

---

## [1.0] — December 2016 (MicroByte)

First complete, publicly released version of Radium on the Pebble App Store. MicroByte developed this version starting from the earlier Matthew Reiss prototype. Store listing dates from December 9, 2016. No GitHub repository is known to exist for this version; the Radium 2 repo branches from the earlier Reiss code rather than this build.

---

## [0.x] — December 2015 (Matthew Reiss)

Partial prototype implementation by Matthew Reiss, built from Sterling Ely's original design. Development ended with a last commit dated December 9, 2015. This is the version the Radium 2 repository was forked from.

---

## [concept] — ~2015 (Sterling Ely)

Original watchface concept designed by Sterling Ely for the Pebble Time Round — a radial bar graph where filled wedge/arc segments represent time, battery, and steps. Designed to be readable as pure geometry with or without a digital overlay. Design files predate any implementation; exact date pending recovery of original assets.

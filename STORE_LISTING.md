# Radium 2 — Store Listing

## Name
Radium 2

## Short Description
Radial bar graph watchface. Hours and minutes as arcs, battery and steps on the outer ring.

## Full Description

A radial bar graph watchface.
Time and energy flowing around your wrist — hours and minutes as arcs, with battery and steps on the outer ring.

The left half of the face fills with hour blocks, one per hour, sweeping clockwise from the bottom. The right half fills with minute ticks: 60 individual marks in groups of five, lighting up as each minute passes. The outer ring tracks step count toward your daily goal on the left side and battery level on the right.

In the center, an optional overlay shows the time and up to four configurable info lines — mix and match day, date, weather, steps, distance, active calories, and battery % however you like. Shake to dismiss it and see the full radial display: a pure starburst of glowing geometry. Shake again to bring it back. (Or set the overlay to always-on or always-off in settings.)

Radium 2 is a complete rebuild of the original Radium watchface, designed for Pebble Time Round in 2015 and first published in 2016. Rebuilt for all Pebble platforms with 40 color presets and full per-element color customization.

---
FEATURES
- Radial bar graph display — hours on the left, minutes on the right
- Four configurable info lines: day, date, weather, steps, distance, calories, battery %, or none
- Live weather from Open-Meteo — no account or API key required
- Shake to toggle overlay on/off (or set always-on / always-off)
- 1-minute overlay mode: art mode by default, one shake shows info for 60 seconds
- Art mode: set overlay to Always Off for a pure full-face starburst
- 24h mode: each hour slot splits into two half-segments
- Outer ring: step count (left half) and battery level (right half)
- Optional outer ring: hide it and tick art extends to the screen edge
- Two overlay sizes: Small and Large (Large default on Pebble Time 2 and Round 2)
- 40 color presets in five rows: Dark, Dark+, Light, Color, Special
- Full per-element color customization: 17 independent color slots
- B&W invert mode for Pebble Classic, Pebble 2, and Pebble 2 Duo

PLATFORMS:
Pebble Classic/Steel · Pebble Time/Steel · Pebble Time Round · Pebble 2 · Pebble Time 2 · Pebble 2 Duo · Pebble Round 2

---
2015: Original Radium design by Sterling Ely
2016: Prototype implementation by MathewReiss; completed by MicroByte
2026: Radium 2 rebuilt by Sterling Ely & Claude (Anthropic)

Feedback is appreciated: please hit me up at @sterlingely on Twitter/X with any bugs or feature requests!

---

## Release Notes

### v2.2
- The text above & below the time are now 4 configurable info lines: Each can show day, date, weather, steps, distance, active calories, battery %, or nothing!
- Introducing 1-minute overlay mode: The watchface defaults to "art mode" while at rest, and a shake brings up the detailed info display for 60 seconds!
- Leading-tick highlights: The current hour and minute ticks are independently colorable, bringing the total customizable color slots to 17!
- New large overlay sizes for Pebble Time 2 & Round 2: Big info display or more room for the radial starburst? The choice is yours!
- 40 redesigned color presets: 40% prettier!
- ...all that and a couple of bug fixes!

### v2.1
- 40 color presets across five rows (Dark, Dark+, Light, Color, Special) — up from 24
- 12 independent color slots: every element (hours, minutes, battery, steps, text, overlay, background) fully customizable
- Independent unlit/dim colors per element
- Overlay background color now independently configurable
- Cascade color picker: tap any parent swatch to set a whole group at once
- Bug fixes: platform detection, settings persistence, overlay on round platforms

### v2.0
- Complete rebuild from the original Radium codebase
- Radial bar graph restored: wedge ticks on rect, arc ticks on round
- Outer ring: battery right, steps left
- Overlay with day / time / date; shake-to-toggle
- 24 color presets (Dark, Light, Color), 9 color slots
- Supports all Pebble platforms including Pebble Round 2

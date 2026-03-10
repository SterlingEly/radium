# Radium 2 — Store Listing

## Name
Radium 2

## Short Description
Radial bar graph watchface. Hours and minutes as arcs, battery and steps on the outer ring.

## Full Description

A watchface built around a single idea: display time as a radial bar graph.

The right half of the face fills with minute ticks — 60 individual marks in groups of five, lighting up as each minute passes. The left half fills with hour blocks, one per hour, sweeping around from the bottom. Together they make the time readable at a glance without needing to parse a hand position.

The outer ring tracks two more things quietly in the background: battery level on the right half, step count toward your daily goal on the left. Both fill from 6 o'clock toward 12.

In the center, an overlay shows the exact time, day, and date. Shake to dismiss it and see the full radial display — a pure starburst of glowing geometry. Shake again to bring it back. Or set the overlay to always-on or always-off in settings.

**Radium 2** is a complete rebuild of the original Radium watchface, designed in December 2015 and first published in December 2016. Rebuilt for all modern Pebble platforms with 40 color presets and full per-element color customization.

---

**Features**
- Radial bar graph display — minutes on the right, hours on the left
- Shake to toggle overlay on/off (or set always-on / always-off)
- Art mode: set overlay to Always Off for a pure full-face starburst
- 24h mode: each hour slot splits into two half-segments
- Outer ring: battery (right half) and step count (left half), both filling from 6 o'clock
- Optional outer ring: hide it and tick art extends to the screen edge
- 40 color presets in five rows: Dark, Dark+, Light, Color, Special
- Full per-element color customization: 12 independent color slots
- B&W invert mode for Pebble Classic, Pebble 2, and Pebble 2 Duo

**Platforms:** Pebble Classic/Steel · Pebble Time/Steel · Pebble Time Round · Pebble 2 · Pebble Time 2 · Pebble 2 Duo · Pebble Round 2

---

## Release Notes

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

---

*Original Radium design by Sterling Ely (2015). Prototype implementation by MathewReiss; completed by MicroByte. Radium 2 rebuilt by Sterling Ely & Claude (Anthropic).*

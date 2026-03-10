#include <pebble.h>

// ============================================================
// CONSTANTS
// ============================================================
#define SETTINGS_KEY      2    // bumped from 1: new struct, old settings incompatible
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2    // px gap between outer ring and tick radials
#define RING_THICK        6    // outer ring thickness in px

// OverlayMode values
#define OVERLAY_ON     0
#define OVERLAY_OFF    1
#define OVERLAY_SHAKE  2

// ============================================================
// SETTINGS  (v2.1 — full 12-color + ring toggle)
// ============================================================
// v2.1 color model (color watches) — all 12 slots fully independent:
//   Text  (2): TimeTextColor, DateTextColor
//   Lit   (4): LitHourColor, LitMinuteColor, LitBatteryColor, LitStepsColor
//   Unlit (4): DimHourColor, DimMinuteColor, DimBatteryColor, DimStepsColor
//   Base  (2): BackgroundColor, OverlayBgColor
// Config page cascades: top-level swatch sets all children; mid-level sets its group.
typedef struct {
  // Text (2)
  GColor TimeTextColor;
  GColor DateTextColor;
  // Lit elements (4)
  GColor LitHourColor;
  GColor LitMinuteColor;
  GColor LitBatteryColor;
  GColor LitStepsColor;
  // Unlit elements — NEW in v2.1: per-element dim colors (4)
  GColor DimHourColor;     // empty hour tick tracks
  GColor DimMinuteColor;   // empty minute tick tracks
  GColor DimBatteryColor;  // empty battery ring track
  GColor DimStepsColor;    // empty steps ring track
  // Base (2)
  GColor BackgroundColor;
  GColor OverlayBgColor;   // center overlay bg (usually = BackgroundColor)
  // Non-color settings
  int    StepGoal;
  int    OverlayMode;      // OVERLAY_ON / OVERLAY_OFF / OVERLAY_SHAKE
  bool   InvertBW;         // B&W only: swap black/white at draw time
  bool   ShowRing;         // show/hide outer battery+steps ring
} RadiumSettings;

static RadiumSettings s_settings;

static void prv_default_settings(void) {
  s_settings.BackgroundColor = GColorBlack;
  s_settings.OverlayBgColor  = GColorBlack;
#if defined(PBL_COLOR)
  s_settings.TimeTextColor   = GColorWhite;
  s_settings.DateTextColor   = GColorWhite;
  s_settings.LitHourColor    = GColorMintGreen;
  s_settings.LitMinuteColor  = GColorMintGreen;
  s_settings.LitBatteryColor = GColorMintGreen;
  s_settings.LitStepsColor   = GColorMintGreen;
  s_settings.DimHourColor    = GColorDarkGray;
  s_settings.DimMinuteColor  = GColorDarkGray;
  s_settings.DimBatteryColor = GColorDarkGray;
  s_settings.DimStepsColor   = GColorDarkGray;
#else
  s_settings.TimeTextColor   = GColorWhite;
  s_settings.DateTextColor   = GColorWhite;
  s_settings.LitHourColor    = GColorWhite;
  s_settings.LitMinuteColor  = GColorWhite;
  s_settings.LitBatteryColor = GColorWhite;
  s_settings.LitStepsColor   = GColorWhite;
  s_settings.DimHourColor    = GColorDarkGray;
  s_settings.DimMinuteColor  = GColorDarkGray;
  s_settings.DimBatteryColor = GColorDarkGray;
  s_settings.DimStepsColor   = GColorDarkGray;
#endif
  s_settings.StepGoal    = DEFAULT_STEP_GOAL;
  s_settings.OverlayMode = OVERLAY_SHAKE;
  s_settings.InvertBW    = false;
  s_settings.ShowRing    = true;
}

static void prv_save_settings(void) {
  persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

static void prv_load_settings(void) {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

// ============================================================
// STATE
// ============================================================
static Window *s_window;
static Layer  *s_canvas_layer;

static int  s_hour    = 0;
static int  s_minute  = 0;
static int  s_battery = 100;
static int  s_steps   = 0;
static bool s_show_overlay = true;

static char s_time_buffer[8];
static char s_day_buffer[12];
static char s_date_buffer[10];

static GPoint    s_tri_pts[3];
static GPathInfo s_tri_info = { .num_points = 3, .points = s_tri_pts };
static GPath    *s_tri_path = NULL;

// ============================================================
// HELPERS
// ============================================================
static const char *get_day_name(int wday) {
  static const char *days[] = {
    "SUNDAY","MONDAY","TUESDAY","WEDNESDAY","THURSDAY","FRIDAY","SATURDAY"
  };
  return (wday >= 0 && wday < 7) ? days[wday] : "";
}

static const char *get_month_abbr(int mon) {
  static const char *months[] = {
    "JAN","FEB","MAR","APR","MAY","JUN",
    "JUL","AUG","SEP","OCT","NOV","DEC"
  };
  return (mon >= 0 && mon < 12) ? months[mon] : "";
}

static bool prv_overlay_visible(void) {
  return (s_settings.OverlayMode != OVERLAY_OFF) && s_show_overlay;
}

static void draw_wedge(GContext *ctx, int cx, int cy, int radius,
                       int32_t a1, int32_t a2) {
  s_tri_pts[0] = GPoint(cx, cy);
  s_tri_pts[1] = GPoint(cx + radius * sin_lookup(a1) / TRIG_MAX_RATIO,
                        cy - radius * cos_lookup(a1) / TRIG_MAX_RATIO);
  s_tri_pts[2] = GPoint(cx + radius * sin_lookup(a2) / TRIG_MAX_RATIO,
                        cy - radius * cos_lookup(a2) / TRIG_MAX_RATIO);
  gpath_draw_filled(ctx, s_tri_path);
}

// ============================================================
// DRAW
// ============================================================
static void draw_layer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);
  int w  = bounds.size.w;
  int h  = bounds.size.h;
  int cx = w / 2;
  int cy = h / 2;

#if defined(PBL_ROUND)
  const bool is_round = true;
#else
  const bool is_round = false;
#endif

  // ----------------------------------------------------------
  // EFFECTIVE COLORS
  // B&W: InvertBW swaps black/white. All color vars resolved once here.
  // ----------------------------------------------------------
#if defined(PBL_BW)
  GColor bw_lit    = s_settings.InvertBW ? GColorBlack     : GColorWhite;
  GColor bw_dim    = s_settings.InvertBW ? GColorLightGray : GColorDarkGray;
  GColor col_bg    = s_settings.InvertBW ? GColorWhite     : GColorBlack;
  GColor col_fg    = s_settings.InvertBW ? GColorBlack     : GColorWhite;
  GColor col_dfg   = col_fg;
  GColor col_min   = bw_lit;
  GColor col_hour  = bw_lit;
  GColor col_batt  = bw_lit;
  GColor col_step  = bw_lit;
  GColor col_dmin  = bw_dim;
  GColor col_dhour = bw_dim;
  GColor col_dbatt = bw_dim;
  GColor col_dstep = bw_dim;
  GColor col_obg   = col_bg;
#else
  GColor col_bg    = s_settings.BackgroundColor;
  GColor col_fg    = s_settings.TimeTextColor;
  GColor col_dfg   = s_settings.DateTextColor;
  GColor col_min   = s_settings.LitMinuteColor;
  GColor col_hour  = s_settings.LitHourColor;
  GColor col_batt  = s_settings.LitBatteryColor;
  GColor col_step  = s_settings.LitStepsColor;
  GColor col_dmin  = s_settings.DimMinuteColor;
  GColor col_dhour = s_settings.DimHourColor;
  GColor col_dbatt = s_settings.DimBatteryColor;
  GColor col_dstep = s_settings.DimStepsColor;
  GColor col_obg   = s_settings.OverlayBgColor;
#endif

  // ----------------------------------------------------------
  // LAYOUT
  // When ring is hidden: ticks expand to screen edge (pure starburst).
  // When ring is shown: inset by ring + gap.
  // ----------------------------------------------------------
  bool show_ring = s_settings.ShowRing;
  int inset = show_ring ? (RING_THICK + RING_GAP) : 0;
  GRect tick_rect  = GRect(inset, inset, w - 2*inset, h - 2*inset);
  int inner_short  = (tick_rect.size.w < tick_rect.size.h)
                     ? tick_rect.size.w : tick_rect.size.h;

  int tick_thick;
  if (s_settings.OverlayMode == OVERLAY_OFF) {
    tick_thick = inner_short;
  } else {
#if defined(PBL_ROUND)
    tick_thick = (h > 180) ? inner_short * 36 / 164
                           : inner_short * 18 / 164;
#else
    tick_thick = inner_short * 19 / 164;
#endif
  }

  int radius = ((w > h) ? w : h) - RING_THICK - 1;

  graphics_context_set_stroke_width(ctx, 0);

  // ----------------------------------------------------------
  // BACKGROUND
  // ----------------------------------------------------------
  graphics_context_set_fill_color(ctx, col_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // ----------------------------------------------------------
  // TICK MARKS
  // ----------------------------------------------------------
  if (!is_round) {
    // RECT path

    int filled_groups = s_minute / 5;
    int partial_min   = s_minute % 5;

    // -- Minutes empty --
    // Minute groups: 9 deg block + 6 deg gap = 15 deg pitch, centering the group 5/6 gap on 90 deg.
    graphics_context_set_fill_color(ctx, col_dmin);
    graphics_context_set_stroke_color(ctx, col_dmin);
    for (int g = filled_groups + (partial_min > 0 ? 1 : 0); g < 12; g++) {
      int a = 3 + 15*g;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
    }
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, col_bg);
    graphics_context_set_stroke_color(ctx, col_bg);
    for (int g = filled_groups + (partial_min > 0 ? 1 : 0); g < 12; g++) {
      int base = 3 + 15*g;
      for (int i = 0; i < 4; i++) {
        int gap = base + 2*i + 1;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(gap), DEG_TO_TRIGANGLE(gap + 1));
      }
    }
#endif

    // -- Minutes filled --
    graphics_context_set_fill_color(ctx, col_min);
    graphics_context_set_stroke_color(ctx, col_min);
    for (int g = 0; g < filled_groups; g++) {
      int a = 3 + 15*g;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
    }
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, col_bg);
    graphics_context_set_stroke_color(ctx, col_bg);
    for (int g = 0; g < filled_groups; g++) {
      int base = 3 + 15*g;
      for (int i = 0; i < 4; i++) {
        int gap = base + 2*i + 1;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(gap), DEG_TO_TRIGANGLE(gap + 1));
      }
    }
#endif

    // -- Minutes partial group --
    if (partial_min > 0) {
      int a = 3 + 15*filled_groups;
      graphics_context_set_fill_color(ctx, col_dmin);
      graphics_context_set_stroke_color(ctx, col_dmin);
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
#if defined(PBL_COLOR)
      graphics_context_set_fill_color(ctx, col_bg);
      graphics_context_set_stroke_color(ctx, col_bg);
      for (int i = 0; i < 4; i++) {
        int gap = a + 2*i + 1;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(gap), DEG_TO_TRIGANGLE(gap + 1));
      }
#endif
      graphics_context_set_fill_color(ctx, col_min);
      graphics_context_set_stroke_color(ctx, col_min);
      for (int i = 0; i < partial_min; i++) {
        int ta = a + 2*i;
#if defined(PBL_BW)
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 2));
#else
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 1));
#endif
      }
    }

    // -- Hours --
    bool is_24h = clock_is_24h_style();
    int filled_slots = is_24h ? (s_hour / 2) : ((s_hour % 12) ?: 12);
    int filled_half  = s_hour % 2;

    // Empty slot bases
    // Hour ticks are 9 deg wide (gap 6 deg) — same 15 deg pitch as before but redistributed
    // so the inter-slot gap at 9 o'clock is centered exactly on 270 deg, and both edge
    // gaps at 6 and 12 o'clock are equal (3 deg each). Pitch unchanged; visually near-identical.
    graphics_context_set_fill_color(ctx, col_dhour);
    graphics_context_set_stroke_color(ctx, col_dhour);
    for (int h2 = 0; h2 < 12; h2++) {
      int a = 183 + 15*h2;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
    }
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, col_bg);
    graphics_context_set_stroke_color(ctx, col_bg);
    for (int h2 = 0; h2 < 12; h2++) {
      int a = 183 + 15*h2;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 9), DEG_TO_TRIGANGLE(a + 10));
    }
    if (is_24h) {
      // 24h divider: 3 deg cut (perfect thirds: a to a+3, gap a+3 to a+6, a+6 to a+9)
      for (int h2 = 0; h2 < 12; h2++) {
        int a = 183 + 15*h2 + 3;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
      }
    }
#endif

    // Filled hours
    graphics_context_set_fill_color(ctx, col_hour);
    graphics_context_set_stroke_color(ctx, col_hour);
    if (!is_24h) {
      for (int h2 = 0; h2 < filled_slots; h2++) {
        int a = 183 + 15*h2;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      }
    } else {
      for (int h2 = 0; h2 < filled_slots; h2++) {
        int a = 183 + 15*h2;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
      }
      if (filled_slots < 12) {
        int a = 183 + 15*filled_slots;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        if (filled_half == 1) {
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
      }
    }
#if defined(PBL_COLOR)
    // Re-cut separators over filled blocks
    graphics_context_set_fill_color(ctx, col_bg);
    graphics_context_set_stroke_color(ctx, col_bg);
    for (int h2 = 0; h2 <= filled_slots && h2 < 12; h2++) {
      int a = 183 + 15*h2;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 9), DEG_TO_TRIGANGLE(a + 10));
    }
    if (is_24h) {
      for (int h2 = 0; h2 <= filled_slots && h2 < 12; h2++) {
        int a = 183 + 15*h2 + 3;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
      }
    }
#endif

  } else {
    // ROUND path
    graphics_context_set_fill_color(ctx, col_dmin);
    for (int i = 0; i < 60; i++) {
      int a = 3 + 2*i + 5*(i/5);
      graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                           DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
    }
    graphics_context_set_fill_color(ctx, col_min);
    for (int i = 0; i < s_minute; i++) {
      int a = 3 + 2*i + 5*(i/5);
      graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                           DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
    }
    {
      bool is_24h = clock_is_24h_style();
      int filled_slots = is_24h ? (s_hour / 2) : ((s_hour % 12) ?: 12);
      int filled_half  = s_hour % 2;

      graphics_context_set_fill_color(ctx, col_dhour);
      for (int h2 = 0; h2 < 12; h2++) {
        int a = 183 + 15*h2;
        graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                             DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      }
      if (is_24h) {
        // 24h divider: 3 deg cut (perfect thirds of 9 deg tick)
        graphics_context_set_fill_color(ctx, col_bg);
        for (int h2 = 0; h2 < 12; h2++) {
          int a = 183 + 15*h2 + 3;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        }
      }
      graphics_context_set_fill_color(ctx, col_hour);
      if (!is_24h) {
        for (int h2 = 0; h2 < filled_slots; h2++) {
          int a = 183 + 15*h2;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
        }
      } else {
        for (int h2 = 0; h2 < filled_slots; h2++) {
          int a = 183 + 15*h2;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
        if (filled_slots < 12) {
          int a = 183 + 15*filled_slots;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          if (filled_half == 1) {
            graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                                 DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
          }
        }
        // Re-cut 24h dividers over filled slots
        graphics_context_set_fill_color(ctx, col_bg);
        for (int h2 = 0; h2 <= filled_slots && h2 < 12; h2++) {
          int a = 183 + 15*h2 + 3;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        }
      }
    }
  }

  // ----------------------------------------------------------
  // CENTER OVERLAY CIRCLE (rect only)
  // ----------------------------------------------------------
  if (prv_overlay_visible()) {
    int overlay_r = (w >= 200) ? 64 : 58;
    graphics_context_set_fill_color(ctx, col_obg);
    graphics_fill_circle(ctx, GPoint(cx, cy), overlay_r);
  }

  // ----------------------------------------------------------
  // INNER GAP STRIP (rect only) — separates ticks from outer ring
  // Skipped when ring is hidden (inset is 0, no ring to separate from)
  // ----------------------------------------------------------
#if !defined(PBL_ROUND)
  if (show_ring) {
    int strip = RING_THICK + RING_GAP;
    graphics_context_set_fill_color(ctx, col_bg);
    graphics_fill_rect(ctx, GRect(0,         0,         w,     strip), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0,         h - strip, w,     strip), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(0,         0,         strip, h    ), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(w - strip, 0,         strip, h    ), 0, GCornerNone);
  }
#endif

  // ----------------------------------------------------------
  // OUTER RING: battery (right) + steps (left)
  // Skipped entirely when ShowRing is false.
  // ----------------------------------------------------------
  if (show_ring) {
    int step_pct = (s_settings.StepGoal > 0)
      ? (s_steps * 100) / s_settings.StepGoal : 0;
    if (step_pct > 100) step_pct = 100;

    if (is_round) {
      // Empty tracks
      graphics_context_set_fill_color(ctx, col_dbatt);
      graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                           DEG_TO_TRIGANGLE(3),   DEG_TO_TRIGANGLE(177));
      graphics_context_set_fill_color(ctx, col_dstep);
      graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                           DEG_TO_TRIGANGLE(183), DEG_TO_TRIGANGLE(357));
      // Battery fill
      if (s_battery > 0) {
        graphics_context_set_fill_color(ctx, col_batt);
        graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                             DEG_TO_TRIGANGLE(177 - 174 * s_battery / 100),
                             DEG_TO_TRIGANGLE(177));
      }
      // Steps fill
      if (step_pct > 0) {
        graphics_context_set_fill_color(ctx, col_step);
        graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                             DEG_TO_TRIGANGLE(183),
                             DEG_TO_TRIGANGLE(183 + 174 * step_pct / 100));
      }
    } else {
      // Rect ring
      int t      = RING_THICK;
      int gap    = 5;
      int half_w = cx - gap;
      int total  = half_w + h + half_w;

      // Clear edge strips
      graphics_context_set_fill_color(ctx, col_bg);
      graphics_fill_rect(ctx, GRect(0,   0,   w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   h-t, w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   0,   t, h), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(w-t, 0,   t, h), 0, GCornerNone);

      // Battery empty track (right half)
      graphics_context_set_fill_color(ctx, col_dbatt);
      graphics_fill_rect(ctx, GRect(cx+gap, 0,   half_w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(w-t,    0,   t,      h), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(cx+gap, h-t, half_w, t), 0, GCornerNone);

      // Battery fill
      {
        int filled = total * s_battery / 100;
        graphics_context_set_fill_color(ctx, col_batt);
        if (filled > 0) {
          int seg = (filled < half_w) ? filled : half_w;
          graphics_fill_rect(ctx, GRect(cx+gap+half_w-seg, h-t, seg, t), 0, GCornerNone);
          filled -= seg;
        }
        if (filled > 0) {
          int seg = (filled < h) ? filled : h;
          graphics_fill_rect(ctx, GRect(w-t, h-seg, t, seg), 0, GCornerNone);
          filled -= seg;
        }
        if (filled > 0) {
          int seg = (filled < half_w) ? filled : half_w;
          graphics_fill_rect(ctx, GRect(cx+gap+half_w-seg, 0, seg, t), 0, GCornerNone);
        }
      }

      // Steps empty track (left half)
      graphics_context_set_fill_color(ctx, col_dstep);
      graphics_fill_rect(ctx, GRect(0,   0,   half_w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   0,   t,      h), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   h-t, half_w, t), 0, GCornerNone);

      // Steps fill
      if (step_pct > 0) {
        int filled = total * step_pct / 100;
        graphics_context_set_fill_color(ctx, col_step);
        if (filled > 0) {
          int seg = (filled < half_w) ? filled : half_w;
          graphics_fill_rect(ctx, GRect(cx-gap-seg, h-t, seg, t), 0, GCornerNone);
          filled -= seg;
        }
        if (filled > 0) {
          int seg = (filled < h) ? filled : h;
          graphics_fill_rect(ctx, GRect(0, h-seg, t, seg), 0, GCornerNone);
          filled -= seg;
        }
        if (filled > 0) {
          int seg = (filled < half_w) ? filled : half_w;
          graphics_fill_rect(ctx, GRect(0, 0, seg, t), 0, GCornerNone);
        }
      }
    }
  }

  // ----------------------------------------------------------
  // TEXT OVERLAY: DAY / TIME / DATE
  // ----------------------------------------------------------
  if (prv_overlay_visible()) {
    int time_h  = 40;
    int small_h = 18;
    int spacing = 3;
    int block_h = small_h + spacing + time_h + spacing + small_h;
    int top_y   = cy - block_h / 2 - 3;  // nudge up: LECO font has more bottom padding than top

    graphics_context_set_text_color(ctx, col_dfg);
    graphics_draw_text(ctx, s_day_buffer,
      fonts_get_system_font(FONT_KEY_GOTHIC_18),
      GRect(0, top_y, w, small_h + 2),
      GTextOverflowModeFill, GTextAlignmentCenter, NULL);

    graphics_context_set_text_color(ctx, col_fg);
    graphics_draw_text(ctx, s_time_buffer,
      fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS),
      GRect(0, top_y + small_h + spacing, w, time_h + 4),
      GTextOverflowModeFill, GTextAlignmentCenter, NULL);

    graphics_context_set_text_color(ctx, col_dfg);
    graphics_draw_text(ctx, s_date_buffer,
      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
      GRect(0, top_y + small_h + spacing + time_h + spacing, w, small_h + 2),
      GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

// ============================================================
// EVENT HANDLERS
// ============================================================
static void tick_handler(struct tm *t, TimeUnits units_changed) {
  s_hour   = t->tm_hour;
  s_minute = t->tm_min;
  int disp_hour = clock_is_24h_style() ? t->tm_hour : ((t->tm_hour % 12) ?: 12);
  snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d", disp_hour, t->tm_min);
  snprintf(s_day_buffer,  sizeof(s_day_buffer),  "%s", get_day_name(t->tm_wday));
  snprintf(s_date_buffer, sizeof(s_date_buffer), "%s %02d",
           get_month_abbr(t->tm_mon), t->tm_mday);
  layer_mark_dirty(s_canvas_layer);
}

static void battery_handler(BatteryChargeState state) {
  s_battery = state.charge_percent;
  layer_mark_dirty(s_canvas_layer);
}

#if defined(PBL_HEALTH)
static void update_steps(void) {
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(
    HealthMetricStepCount, time_start_of_today(), time(NULL));
  s_steps = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(HealthMetricStepCount) : 0;
  layer_mark_dirty(s_canvas_layer);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) update_steps();
}
#endif

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t;
  // Text colors
  t = dict_find(iter, MESSAGE_KEY_TimeTextColor);
  if (t) s_settings.TimeTextColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DateTextColor);
  if (t) s_settings.DateTextColor    = GColorFromHEX(t->value->int32);
  // Lit colors
  t = dict_find(iter, MESSAGE_KEY_LitHourColor);
  if (t) s_settings.LitHourColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitMinuteColor);
  if (t) s_settings.LitMinuteColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitBatteryColor);
  if (t) s_settings.LitBatteryColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitStepsColor);
  if (t) s_settings.LitStepsColor    = GColorFromHEX(t->value->int32);
  // Dim colors (new in v2.1)
  t = dict_find(iter, MESSAGE_KEY_DimHourColor);
  if (t) s_settings.DimHourColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimMinuteColor);
  if (t) s_settings.DimMinuteColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimBatteryColor);
  if (t) s_settings.DimBatteryColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimStepsColor);
  if (t) s_settings.DimStepsColor    = GColorFromHEX(t->value->int32);
  // Base colors
  t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (t) s_settings.BackgroundColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_OverlayBgColor);
  if (t) s_settings.OverlayBgColor   = GColorFromHEX(t->value->int32);
  // Non-color settings
  t = dict_find(iter, MESSAGE_KEY_StepGoal);
  if (t) s_settings.StepGoal = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_OverlayMode);
  if (t) {
    s_settings.OverlayMode = (int)t->value->int32;
    s_show_overlay = (s_settings.OverlayMode != OVERLAY_OFF);
  }
  t = dict_find(iter, MESSAGE_KEY_InvertBW);
  if (t) s_settings.InvertBW = (t->value->int32 == 1);
  t = dict_find(iter, MESSAGE_KEY_ShowRing);
  if (t) s_settings.ShowRing = (t->value->int32 == 1);
  prv_save_settings();
  layer_mark_dirty(s_canvas_layer);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  if (s_settings.OverlayMode != OVERLAY_SHAKE) return;
  s_show_overlay = !s_show_overlay;
  layer_mark_dirty(s_canvas_layer);
}

// ============================================================
// WINDOW / APP LIFECYCLE
// ============================================================
static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  s_canvas_layer = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_canvas_layer, draw_layer);
  layer_add_child(root, s_canvas_layer);
  s_tri_path = gpath_create(&s_tri_info);
  accel_tap_service_subscribe(accel_tap_handler);
}

static void window_unload(Window *window) {
  gpath_destroy(s_tri_path);
  s_tri_path = NULL;
  layer_destroy(s_canvas_layer);
}

static void init(void) {
  prv_load_settings();
  s_show_overlay = (s_settings.OverlayMode != OVERLAY_OFF);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_set_background_color(s_window, s_settings.BackgroundColor);
  window_stack_push(s_window, true);

  time_t now = time(NULL);
  tick_handler(localtime(&now), MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
  update_steps();
#endif
  app_message_register_inbox_received(inbox_received);
  app_message_open(768, 64);  // bumped: more keys now
}

static void deinit(void) {
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
#if defined(PBL_HEALTH)
  health_service_events_unsubscribe();
#endif
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

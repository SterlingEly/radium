#include <pebble.h>

// ============================================================
// CONSTANTS
// ============================================================
#define SETTINGS_KEY      3    // bumped from 2: new struct fields (tip colors)
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2    // px gap between outer ring and tick radials
#define RING_THICK        6    // outer ring thickness in px

// OverlayMode values
#define OVERLAY_ON     0
#define OVERLAY_OFF    1
#define OVERLAY_SHAKE  2

// ============================================================
// SETTINGS  (v2.2 — added LitHourTipColor, LitMinuteTipColor)
// ============================================================
typedef struct {
  // Text (2)
  GColor TimeTextColor;
  GColor DateTextColor;
  // Lit elements (4)
  GColor LitHourColor;
  GColor LitMinuteColor;
  GColor LitBatteryColor;
  GColor LitStepsColor;
  // Unlit elements (4)
  GColor DimHourColor;
  GColor DimMinuteColor;
  GColor DimBatteryColor;
  GColor DimStepsColor;
  // Base (2)
  GColor BackgroundColor;
  GColor OverlayBgColor;
  // Non-color settings
  int    StepGoal;
  int    OverlayMode;      // OVERLAY_ON / OVERLAY_OFF / OVERLAY_SHAKE
  bool   InvertBW;         // B&W only: swap black/white at draw time
  bool   ShowRing;         // show/hide outer battery+steps ring
  // Tip colors (2) — NEW in v2.2: leading edge tick on hour and minute bars
  GColor LitHourTipColor;
  GColor LitMinuteTipColor;
} RadiumSettings;

static RadiumSettings s_settings;

static void prv_default_settings(void) {
  s_settings.BackgroundColor   = GColorBlack;
  s_settings.OverlayBgColor    = GColorBlack;
#if defined(PBL_COLOR)
  s_settings.TimeTextColor     = GColorWhite;
  s_settings.DateTextColor     = GColorWhite;
  s_settings.LitHourColor      = GColorMintGreen;
  s_settings.LitMinuteColor    = GColorMintGreen;
  s_settings.LitBatteryColor   = GColorMintGreen;
  s_settings.LitStepsColor     = GColorMintGreen;
  s_settings.DimHourColor      = GColorDarkGray;
  s_settings.DimMinuteColor    = GColorDarkGray;
  s_settings.DimBatteryColor   = GColorDarkGray;
  s_settings.DimStepsColor     = GColorDarkGray;
  s_settings.LitHourTipColor   = GColorWhite;
  s_settings.LitMinuteTipColor = GColorWhite;
#else
  s_settings.TimeTextColor     = GColorWhite;
  s_settings.DateTextColor     = GColorWhite;
  s_settings.LitHourColor      = GColorWhite;
  s_settings.LitMinuteColor    = GColorWhite;
  s_settings.LitBatteryColor   = GColorWhite;
  s_settings.LitStepsColor     = GColorWhite;
  s_settings.DimHourColor      = GColorDarkGray;
  s_settings.DimMinuteColor    = GColorDarkGray;
  s_settings.DimBatteryColor   = GColorDarkGray;
  s_settings.DimStepsColor     = GColorDarkGray;
  s_settings.LitHourTipColor   = GColorWhite;
  s_settings.LitMinuteTipColor = GColorWhite;
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
  GColor col_hour_tip = bw_lit;
  GColor col_min_tip  = bw_lit;
#else
  GColor col_bg       = s_settings.BackgroundColor;
  GColor col_fg       = s_settings.TimeTextColor;
  GColor col_dfg      = s_settings.DateTextColor;
  GColor col_min      = s_settings.LitMinuteColor;
  GColor col_hour     = s_settings.LitHourColor;
  GColor col_batt     = s_settings.LitBatteryColor;
  GColor col_step     = s_settings.LitStepsColor;
  GColor col_dmin     = s_settings.DimMinuteColor;
  GColor col_dhour    = s_settings.DimHourColor;
  GColor col_dbatt    = s_settings.DimBatteryColor;
  GColor col_dstep    = s_settings.DimStepsColor;
  GColor col_obg      = s_settings.OverlayBgColor;
  GColor col_hour_tip = s_settings.LitHourTipColor;
  GColor col_min_tip  = s_settings.LitMinuteTipColor;
#endif

  // ----------------------------------------------------------
  // LAYOUT
  // ----------------------------------------------------------
  bool show_ring = s_settings.ShowRing;
  int inset = show_ring ? (RING_THICK + RING_GAP) : 0;
  GRect tick_rect  = GRect(inset, inset, w - 2*inset, h - 2*inset);
  int inner_short  = (tick_rect.size.w < tick_rect.size.h)
                     ? tick_rect.size.w : tick_rect.size.h;

  // tick_thick: same proportion for all platforms.
  // On round, radials fill from the screen edge inward — the overlay circle
  // covers the center. No inner radius calculation needed.
  int tick_thick;
  if (s_settings.OverlayMode == OVERLAY_OFF) {
    tick_thick = inner_short;
  } else {
    tick_thick = inner_short * 19 / 164;
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
    // ==== RECT PATH ====

    int filled_groups = s_minute / 5;
    int partial_min   = s_minute % 5;

    bool has_min_tip   = (s_minute > 0);
    int  min_tip_group = (partial_min > 0) ? filled_groups   : filled_groups - 1;
    int  min_tip_tick  = (partial_min > 0) ? partial_min - 1 : 4;

    // -- Minutes: empty tracks --
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

    // -- Minutes: filled groups --
    for (int g = 0; g < filled_groups; g++) {
      int a = 3 + 15*g;
#if defined(PBL_BW)
      graphics_context_set_fill_color(ctx, col_min);
      graphics_context_set_stroke_color(ctx, col_min);
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
#else
      for (int i = 0; i < 5; i++) {
        int ta = a + 2*i;
        bool is_tip = has_min_tip && (g == min_tip_group) && (i == min_tip_tick);
        GColor c = is_tip ? col_min_tip : col_min;
        graphics_context_set_fill_color(ctx, c);
        graphics_context_set_stroke_color(ctx, c);
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 1));
      }
#endif
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

    // -- Minutes: partial group --
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
      for (int i = 0; i < partial_min; i++) {
        int ta = a + 2*i;
        bool is_tip = has_min_tip && (i == min_tip_tick);
        GColor c = is_tip ? col_min_tip : col_min;
        graphics_context_set_fill_color(ctx, c);
        graphics_context_set_stroke_color(ctx, c);
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 1));
      }
#else
      graphics_context_set_fill_color(ctx, col_min);
      graphics_context_set_stroke_color(ctx, col_min);
      for (int i = 0; i < partial_min; i++) {
        int ta = a + 2*i;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 2));
      }
#endif
    }

    // -- Hours --
    bool is_24h = clock_is_24h_style();
    int filled_slots = is_24h ? (s_hour / 2) : ((s_hour % 12) ?: 12);
    int filled_half  = s_hour % 2;

    // 24h tip model — one slot = two hours:
    //   hour 0:              nothing lit, no tip
    //   odd hour (h%2==1):   slot h/2 first half just entered — tip = first half only
    //   even hour (h%2==0):  slot h/2-1 second half just completed —
    //                        first half draws normal, second half = tip
    // 12h: tip = last filled slot, full wedge.
    bool has_hour_tip;
    int  hour_tip_slot;
    if (!is_24h) {
      has_hour_tip  = (filled_slots > 0);
      hour_tip_slot = filled_slots - 1;
    } else {
      if (s_hour == 0) {
        has_hour_tip  = false;
        hour_tip_slot = 0;
      } else if (filled_half == 1) {
        has_hour_tip  = true;
        hour_tip_slot = filled_slots;
      } else {
        has_hour_tip  = (filled_slots > 0);
        hour_tip_slot = filled_slots - 1;
      }
    }

    // Empty hour tracks
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
      for (int h2 = 0; h2 < 12; h2++) {
        int a = 183 + 15*h2 + 3;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
      }
    }
#endif

    // Filled hours
    if (!is_24h) {
      graphics_context_set_fill_color(ctx, col_hour);
      graphics_context_set_stroke_color(ctx, col_hour);
      int draw_to = has_hour_tip ? hour_tip_slot : filled_slots;
      for (int h2 = 0; h2 < draw_to; h2++) {
        int a = 183 + 15*h2;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      }
#if defined(PBL_COLOR)
      if (has_hour_tip) {
        int a = 183 + 15*hour_tip_slot;
        graphics_context_set_fill_color(ctx, col_hour_tip);
        graphics_context_set_stroke_color(ctx, col_hour_tip);
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      }
#endif
    } else {
      // 24h rect
      graphics_context_set_fill_color(ctx, col_hour);
      graphics_context_set_stroke_color(ctx, col_hour);
      int complete_draw_to = has_hour_tip ? hour_tip_slot : filled_slots;
      for (int h2 = 0; h2 < complete_draw_to; h2++) {
        int a = 183 + 15*h2;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
      }
#if defined(PBL_COLOR)
      if (has_hour_tip) {
        int a = 183 + 15*hour_tip_slot;
        if (filled_half == 1) {
          // Odd hour: first half = tip only
          graphics_context_set_fill_color(ctx, col_hour_tip);
          graphics_context_set_stroke_color(ctx, col_hour_tip);
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        } else {
          // Even hour > 0: first half = normal, second half = tip
          graphics_context_set_fill_color(ctx, col_hour);
          graphics_context_set_stroke_color(ctx, col_hour);
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
          graphics_context_set_fill_color(ctx, col_hour_tip);
          graphics_context_set_stroke_color(ctx, col_hour_tip);
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
      }
#endif
    }

#if defined(PBL_COLOR)
    // Re-cut separators over all filled blocks
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
    // ==== ROUND PATH ====
    // Full radials from screen edge inward by tick_thick.
    // The overlay circle covers the center — no inner radius tricks needed.

    // -- Minutes --
    graphics_context_set_fill_color(ctx, col_dmin);
    for (int i = 0; i < 60; i++) {
      int a = 3 + 2*i + 5*(i/5);
      graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                           DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
    }
    if (s_minute > 0) {
      graphics_context_set_fill_color(ctx, col_min);
      for (int i = 0; i < s_minute - 1; i++) {
        int a = 3 + 2*i + 5*(i/5);
        graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                             DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
      }
#if defined(PBL_COLOR)
      graphics_context_set_fill_color(ctx, col_min_tip);
#endif
      {
        int i = s_minute - 1;
        int a = 3 + 2*i + 5*(i/5);
        graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                             DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
      }
    }

    // -- Hours (round) --
    {
      bool is_24h = clock_is_24h_style();
      int filled_slots = is_24h ? (s_hour / 2) : ((s_hour % 12) ?: 12);
      int filled_half  = s_hour % 2;

      bool has_hour_tip;
      int  hour_tip_slot;
      if (!is_24h) {
        has_hour_tip  = (filled_slots > 0);
        hour_tip_slot = filled_slots - 1;
      } else {
        if (s_hour == 0) {
          has_hour_tip  = false;
          hour_tip_slot = 0;
        } else if (filled_half == 1) {
          has_hour_tip  = true;
          hour_tip_slot = filled_slots;
        } else {
          has_hour_tip  = (filled_slots > 0);
          hour_tip_slot = filled_slots - 1;
        }
      }

      // Empty tracks
      graphics_context_set_fill_color(ctx, col_dhour);
      for (int h2 = 0; h2 < 12; h2++) {
        int a = 183 + 15*h2;
        graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                             DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      }
      if (is_24h) {
        graphics_context_set_fill_color(ctx, col_bg);
        for (int h2 = 0; h2 < 12; h2++) {
          int a = 183 + 15*h2 + 3;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        }
      }

      if (!is_24h) {
        graphics_context_set_fill_color(ctx, col_hour);
        int draw_to = has_hour_tip ? hour_tip_slot : filled_slots;
        for (int h2 = 0; h2 < draw_to; h2++) {
          int a = 183 + 15*h2;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
        }
        if (has_hour_tip) {
#if defined(PBL_COLOR)
          graphics_context_set_fill_color(ctx, col_hour_tip);
#endif
          int a = 183 + 15*hour_tip_slot;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
        }
      } else {
        // 24h round
        graphics_context_set_fill_color(ctx, col_hour);
        int complete_draw_to = has_hour_tip ? hour_tip_slot : filled_slots;
        for (int h2 = 0; h2 < complete_draw_to; h2++) {
          int a = 183 + 15*h2;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
#if defined(PBL_COLOR)
        if (has_hour_tip) {
          int a = 183 + 15*hour_tip_slot;
          if (filled_half == 1) {
            // Odd hour: first half = tip only
            graphics_context_set_fill_color(ctx, col_hour_tip);
            graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                                 DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          } else {
            // Even hour > 0: first half = normal, second half = tip
            graphics_context_set_fill_color(ctx, col_hour);
            graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                                 DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
            graphics_context_set_fill_color(ctx, col_hour_tip);
            graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                                 DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
          }
        }
#else
        // B&W round 24h: no tip distinction
        if (filled_slots < 12) {
          int a = 183 + 15*filled_slots;
          graphics_context_set_fill_color(ctx, col_hour);
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          if (filled_half == 1) {
            graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                                 DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
          }
        }
#endif
        // Re-cut 24h dividers over all lit slots
        graphics_context_set_fill_color(ctx, col_bg);
        for (int h2 = 0; h2 <= filled_slots && h2 < 12; h2++) {
          int a2 = 183 + 15*h2 + 3;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a2), DEG_TO_TRIGANGLE(a2 + 3));
        }
      }
    }
  }

  // ----------------------------------------------------------
  // CENTER OVERLAY CIRCLE — 58px radius, all platforms.
  // Covers tick centers on rect and inner radials on round.
  // Per-platform sizing and large overlay option are future work.
  // ----------------------------------------------------------
  if (prv_overlay_visible()) {
    graphics_context_set_fill_color(ctx, col_obg);
    graphics_fill_circle(ctx, GPoint(cx, cy), 58);
  }

  // ----------------------------------------------------------
  // INNER GAP STRIP (rect only) — separates ticks from outer ring
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
  // ----------------------------------------------------------
  if (show_ring) {
    int step_pct = (s_settings.StepGoal > 0)
      ? (s_steps * 100) / s_settings.StepGoal : 0;
    if (step_pct > 100) step_pct = 100;

    if (is_round) {
      graphics_context_set_fill_color(ctx, col_dbatt);
      graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                           DEG_TO_TRIGANGLE(3),   DEG_TO_TRIGANGLE(177));
      graphics_context_set_fill_color(ctx, col_dstep);
      graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                           DEG_TO_TRIGANGLE(183), DEG_TO_TRIGANGLE(357));
      if (s_battery > 0) {
        graphics_context_set_fill_color(ctx, col_batt);
        graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                             DEG_TO_TRIGANGLE(177 - 174 * s_battery / 100),
                             DEG_TO_TRIGANGLE(177));
      }
      if (step_pct > 0) {
        graphics_context_set_fill_color(ctx, col_step);
        graphics_fill_radial(ctx, bounds, GOvalScaleModeFitCircle, RING_THICK,
                             DEG_TO_TRIGANGLE(183),
                             DEG_TO_TRIGANGLE(183 + 174 * step_pct / 100));
      }
    } else {
      int t      = RING_THICK;
      int gap    = 5;
      int half_w = cx - gap;
      int total  = half_w + h + half_w;

      graphics_context_set_fill_color(ctx, col_bg);
      graphics_fill_rect(ctx, GRect(0,   0,   w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   h-t, w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   0,   t, h), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(w-t, 0,   t, h), 0, GCornerNone);

      graphics_context_set_fill_color(ctx, col_dbatt);
      graphics_fill_rect(ctx, GRect(cx+gap, 0,   half_w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(w-t,    0,   t,      h), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(cx+gap, h-t, half_w, t), 0, GCornerNone);

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

      graphics_context_set_fill_color(ctx, col_dstep);
      graphics_fill_rect(ctx, GRect(0,   0,   half_w, t), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   0,   t,      h), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(0,   h-t, half_w, t), 0, GCornerNone);

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
    int top_y   = cy - block_h / 2 - 3;

    graphics_context_set_text_color(ctx, col_dfg);
    graphics_draw_text(ctx, s_day_buffer,
      fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD),
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
  t = dict_find(iter, MESSAGE_KEY_TimeTextColor);
  if (t) s_settings.TimeTextColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DateTextColor);
  if (t) s_settings.DateTextColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitHourColor);
  if (t) s_settings.LitHourColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitMinuteColor);
  if (t) s_settings.LitMinuteColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitBatteryColor);
  if (t) s_settings.LitBatteryColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitStepsColor);
  if (t) s_settings.LitStepsColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimHourColor);
  if (t) s_settings.DimHourColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimMinuteColor);
  if (t) s_settings.DimMinuteColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimBatteryColor);
  if (t) s_settings.DimBatteryColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimStepsColor);
  if (t) s_settings.DimStepsColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (t) s_settings.BackgroundColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_OverlayBgColor);
  if (t) s_settings.OverlayBgColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitHourTipColor);
  if (t) s_settings.LitHourTipColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitMinuteTipColor);
  if (t) s_settings.LitMinuteTipColor = GColorFromHEX(t->value->int32);
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
  app_message_open(768, 64);
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

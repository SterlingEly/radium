#include <pebble.h>
#include <limits.h>

// ============================================================
// CONSTANTS
// ============================================================
#define SETTINGS_KEY      5
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2
#define RING_THICK        6

#define OVERLAY_OFF    1
#define OVERLAY_SHAKE  2

#define OVERLAY_SMALL  0
#define OVERLAY_LARGE  1

// Modular overlay field IDs
#define FIELD_NONE      0
#define FIELD_DAY_LONG  1  // "SATURDAY"
#define FIELD_DATE      2  // "MAR 21"
#define FIELD_DAY_DATE  3  // "SAT MAR 21"
#define FIELD_STEPS     4  // steps icon + count
#define FIELD_TEMP_F    5  // weather icon + temp F
#define FIELD_TEMP_C    6  // weather icon + temp C
#define FIELD_BATTERY   7  // battery icon + %

static const char *s_short_days[] = {
  "SUN","MON","TUE","WED","THU","FRI","SAT"
};

// ============================================================
// SETTINGS
// ============================================================
typedef struct {
  GColor TimeTextColor;
  GColor DateTextColor;
  GColor LitHourColor;
  GColor LitMinuteColor;
  GColor LitBatteryColor;
  GColor LitStepsColor;
  GColor DimHourColor;
  GColor DimMinuteColor;
  GColor DimBatteryColor;
  GColor DimStepsColor;
  GColor BackgroundColor;
  GColor OverlayBgColor;
  int    StepGoal;
  int    OverlayMode;
  bool   InvertBW;
  bool   ShowRing;
  GColor LitHourTipColor;
  GColor LitMinuteTipColor;
  int    TopOuterField;
  int    TopInnerField;
  int    BottomInnerField;
  int    BottomOuterField;
  GColor InfoLine1Color;  // top outer
  GColor InfoLine2Color;  // top inner
  GColor InfoLine3Color;  // bottom inner
  GColor InfoLine4Color;  // bottom outer
  int    OverlaySize;     // 0=small, 1=large (emery/gabbro)
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
  s_settings.InfoLine1Color    = GColorLightGray;
  s_settings.InfoLine2Color    = GColorLightGray;
  s_settings.InfoLine3Color    = GColorLightGray;
  s_settings.InfoLine4Color    = GColorLightGray;
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
  s_settings.InfoLine1Color    = GColorWhite;
  s_settings.InfoLine2Color    = GColorWhite;
  s_settings.InfoLine3Color    = GColorWhite;
  s_settings.InfoLine4Color    = GColorWhite;
#endif
  s_settings.StepGoal         = DEFAULT_STEP_GOAL;
  s_settings.OverlayMode      = OVERLAY_SHAKE;
  s_settings.InvertBW         = false;
  s_settings.ShowRing         = true;
  // DEBUG: all 4 slots populated
  s_settings.TopOuterField    = FIELD_TEMP_F;
  s_settings.TopInnerField    = FIELD_DAY_LONG;
  s_settings.BottomInnerField = FIELD_DATE;
  s_settings.BottomOuterField = FIELD_STEPS;
  // Large overlay default on emery/gabbro, small elsewhere
#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_CHALK)
  s_settings.OverlaySize = OVERLAY_LARGE;
#else
  s_settings.OverlaySize = OVERLAY_SMALL;
#endif
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

// Weather -- both units stored, INT_MIN = not yet received
static int  s_weather_temp_f = INT_MIN;
static int  s_weather_temp_c = INT_MIN;
static int  s_weather_code   = 0;

static char s_time_buffer[8];
static char s_day_buffer[12];
static char s_date_buffer[10];
static char s_day_date_buffer[14];
static char s_steps_buffer[12];  // "99,999" + null
static char s_battery_buffer[6]; // e.g. "72%"
static char s_temp_f_buffer[8];  // e.g. "72F"
static char s_temp_c_buffer[8];  // e.g. "22C"

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

// draw_wedge: filled triangle from center to arc
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
// WEATHER CODE -> ICON TYPE
// 0=sun, 1=partly cloudy, 2=cloud, 3=rain, 4=snow, 5=storm
// ============================================================
static int weather_icon_for_code(int code) {
  if (code == 0)                             return 0;
  if (code >= 1  && code <= 3)              return 1;
  if (code >= 45 && code <= 48)             return 2;
  if ((code >= 51 && code <= 67) ||
      (code >= 80 && code <= 82))           return 3;
  if ((code >= 71 && code <= 77) ||
      (code >= 85 && code <= 86))           return 4;
  if (code >= 95 && code <= 99)             return 5;
  return 2;
}

// ============================================================
// ICON DRAWING
// Small icons: 11x11px (GOTHIC_18_BOLD cap = 11px)
// Large icons: 15x15px (GOTHIC_24_BOLD cap = 15px)
// ICON_W / ICON_TEXT_GAP are set per draw_field call based on size.
// ============================================================
#define SMALL_FONT_PAD   8
#define LARGE_FONT_PAD   10
#define SMALL_ICON_W     11
#define LARGE_ICON_W     15
#define ICON_TEXT_GAP    2

static void draw_footprint(GContext *ctx, int fx, int fy, GColor col, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (!large) {
    graphics_fill_rect(ctx, GRect(fx, fy, 4, 5), 2, GCornersAll);
    graphics_fill_rect(ctx, GRect(fx+1, fy+4, 2, 4), 1, GCornersAll);
  } else {
    // Scaled up ~35%: 5x7 toe + 3x5 heel
    graphics_fill_rect(ctx, GRect(fx, fy, 5, 7), 2, GCornersAll);
    graphics_fill_rect(ctx, GRect(fx+1, fy+6, 3, 5), 1, GCornersAll);
  }
}

static void draw_steps_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  if (!large) {
    draw_footprint(ctx, ox+5, oy+0, col, false);
    draw_footprint(ctx, ox+0, oy+3, col, false);
  } else {
    draw_footprint(ctx, ox+7, oy+0, col, true);
    draw_footprint(ctx, ox+0, oy+4, col, true);
  }
}

static void draw_battery_icon(GContext *ctx, int ox, int oy, GColor col, int pct, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_rect(ctx, GRect(ox, oy+2, 9, 7));
    graphics_context_set_fill_color(ctx, col);
    graphics_fill_rect(ctx, GRect(ox+9, oy+4, 2, 3), 0, GCornerNone);
    int fill_w = (7 * pct) / 100;
    if (fill_w < 1 && pct > 0) fill_w = 1;
    if (fill_w > 0) graphics_fill_rect(ctx, GRect(ox+1, oy+3, fill_w, 5), 0, GCornerNone);
  } else {
    graphics_draw_rect(ctx, GRect(ox, oy+2, 12, 10));
    graphics_context_set_fill_color(ctx, col);
    graphics_fill_rect(ctx, GRect(ox+12, oy+5, 3, 4), 0, GCornerNone);
    int fill_w = (10 * pct) / 100;
    if (fill_w < 1 && pct > 0) fill_w = 1;
    if (fill_w > 0) graphics_fill_rect(ctx, GRect(ox+1, oy+3, fill_w, 8), 0, GCornerNone);
  }
}

static void draw_sun_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  int sz = large ? 15 : 11;
  int r  = large ? 4  : 3;
  int icx = ox + sz/2, icy = oy + sz/2;
  graphics_draw_circle(ctx, GPoint(icx, icy), r);
  graphics_draw_pixel(ctx, GPoint(icx, oy));      graphics_draw_pixel(ctx, GPoint(icx, oy+1));
  graphics_draw_pixel(ctx, GPoint(icx, oy+sz-1)); graphics_draw_pixel(ctx, GPoint(icx, oy+sz-2));
  graphics_draw_pixel(ctx, GPoint(ox, icy));       graphics_draw_pixel(ctx, GPoint(ox+1, icy));
  graphics_draw_pixel(ctx, GPoint(ox+sz-1, icy));  graphics_draw_pixel(ctx, GPoint(ox+sz-2, icy));
}

static void draw_cloud_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (!large) {
    graphics_fill_rect(ctx, GRect(ox+2, oy+2, 4, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+3, oy+1, 3, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+6, oy+2, 3, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+3, 9, 4), 0, GCornerNone);
  } else {
    graphics_fill_rect(ctx, GRect(ox+3, oy+3, 5, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+4, oy+1, 4, 3), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+8, oy+2, 4, 3), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+4, 13, 6), 0, GCornerNone);
  }
}

static void draw_partly_cloudy_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_circle(ctx, GPoint(ox+7, oy+3), 2);
    graphics_draw_pixel(ctx, GPoint(ox+7, oy));
    graphics_draw_pixel(ctx, GPoint(ox+10, oy+3));
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+6));
    graphics_context_set_fill_color(ctx, col);
    graphics_fill_rect(ctx, GRect(ox+2, oy+4, 3, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+5, oy+4, 2, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+5, 7, 4), 0, GCornerNone);
  } else {
    graphics_draw_circle(ctx, GPoint(ox+10, oy+4), 3);
    graphics_draw_pixel(ctx, GPoint(ox+10, oy));
    graphics_draw_pixel(ctx, GPoint(ox+14, oy+4));
    graphics_draw_pixel(ctx, GPoint(ox+10, oy+8));
    graphics_context_set_fill_color(ctx, col);
    graphics_fill_rect(ctx, GRect(ox+2, oy+6, 4, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+6, oy+6, 3, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+7, 10, 6), 0, GCornerNone);
  }
}

static void draw_rain_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  draw_cloud_icon(ctx, ox, oy-1, col, large);
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_pixel(ctx, GPoint(ox+2, oy+7)); graphics_draw_pixel(ctx, GPoint(ox+2, oy+9));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+8)); graphics_draw_pixel(ctx, GPoint(ox+5, oy+10));
    graphics_draw_pixel(ctx, GPoint(ox+8, oy+7)); graphics_draw_pixel(ctx, GPoint(ox+8, oy+9));
  } else {
    graphics_draw_pixel(ctx, GPoint(ox+2, oy+10)); graphics_draw_pixel(ctx, GPoint(ox+2, oy+12));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+11)); graphics_draw_pixel(ctx, GPoint(ox+6, oy+13));
    graphics_draw_pixel(ctx, GPoint(ox+10, oy+10)); graphics_draw_pixel(ctx, GPoint(ox+10, oy+12));
  }
}

static void draw_snow_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  int sz = large ? 15 : 11;
  int icx = ox + sz/2, icy = oy + sz/2;
  graphics_draw_line(ctx, GPoint(ox+1, icy),    GPoint(ox+sz-2, icy));
  graphics_draw_line(ctx, GPoint(icx, oy+1),    GPoint(icx, oy+sz-2));
  graphics_draw_line(ctx, GPoint(ox+2, oy+2),   GPoint(ox+sz-3, oy+sz-3));
  graphics_draw_line(ctx, GPoint(ox+sz-3, oy+2),GPoint(ox+2, oy+sz-3));
}

static void draw_storm_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  draw_cloud_icon(ctx, ox, oy-1, col, large);
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+6)); graphics_draw_pixel(ctx, GPoint(ox+5, oy+7));
    graphics_draw_pixel(ctx, GPoint(ox+4, oy+7)); graphics_draw_pixel(ctx, GPoint(ox+4, oy+8));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+8)); graphics_draw_pixel(ctx, GPoint(ox+6, oy+9));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+9)); graphics_draw_pixel(ctx, GPoint(ox+5, oy+10));
  } else {
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+8));  graphics_draw_pixel(ctx, GPoint(ox+7, oy+9));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+9));  graphics_draw_pixel(ctx, GPoint(ox+6, oy+10));
    graphics_draw_pixel(ctx, GPoint(ox+8, oy+10)); graphics_draw_pixel(ctx, GPoint(ox+8, oy+11));
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+11)); graphics_draw_pixel(ctx, GPoint(ox+7, oy+12));
  }
}

static void draw_weather_icon(GContext *ctx, int ox, int oy, GColor col, int icon_type, bool large) {
  switch (icon_type) {
    case 0: draw_sun_icon(ctx, ox, oy, col, large);           break;
    case 1: draw_partly_cloudy_icon(ctx, ox, oy, col, large); break;
    case 2: draw_cloud_icon(ctx, ox, oy, col, large);         break;
    case 3: draw_rain_icon(ctx, ox, oy, col, large);          break;
    case 4: draw_snow_icon(ctx, ox, oy, col, large);          break;
    case 5: draw_storm_icon(ctx, ox, oy, col, large);         break;
    default: draw_cloud_icon(ctx, ox, oy, col, large);        break;
  }
}

// ============================================================
// OVERLAY FIELD DRAWING
//
// All icon+text fields use dynamic centering:
//   text width measured, unit centered on cx.
// font, icon_w, font_pad vary by overlay size.
// ============================================================
static void draw_field(GContext *ctx, int field, int y, int w, int cx,
                       GColor col, GColor bg, bool large) {
  if (field == FIELD_NONE) return;

  GFont font     = large
    ? fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD)
    : fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  int   font_h   = large ? 17 : 13;
  int   font_pad = large ? LARGE_FONT_PAD : SMALL_FONT_PAD;
  int   icon_w   = large ? LARGE_ICON_W  : SMALL_ICON_W;
  int   iy       = y + font_pad;

  graphics_context_set_text_color(ctx, col);

  if (field == FIELD_DAY_LONG) {
    graphics_draw_text(ctx, s_day_buffer, font,
      GRect(0, y, w, font_h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);

  } else if (field == FIELD_DATE) {
    graphics_draw_text(ctx, s_date_buffer, font,
      GRect(0, y, w, font_h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);

  } else if (field == FIELD_DAY_DATE) {
    graphics_draw_text(ctx, s_day_date_buffer, font,
      GRect(0, y, w, font_h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);

  } else if (field == FIELD_STEPS) {
    GSize text_size = graphics_text_layout_get_content_size(
      s_steps_buffer, font, GRect(0, 0, 200, 20),
      GTextOverflowModeFill, GTextAlignmentLeft);
    int unit_w = icon_w + ICON_TEXT_GAP + text_size.w;
    int icon_x = cx - unit_w / 2;
    int text_x = icon_x + icon_w + ICON_TEXT_GAP;
    draw_steps_icon(ctx, icon_x, iy, col, large);
    graphics_draw_text(ctx, s_steps_buffer, font,
      GRect(text_x, y, w - text_x, font_h), GTextOverflowModeFill, GTextAlignmentLeft, NULL);

  } else if (field == FIELD_BATTERY) {
    GSize text_size = graphics_text_layout_get_content_size(
      s_battery_buffer, font, GRect(0, 0, 200, 20),
      GTextOverflowModeFill, GTextAlignmentLeft);
    int unit_w = icon_w + ICON_TEXT_GAP + text_size.w;
    int icon_x = cx - unit_w / 2;
    int text_x = icon_x + icon_w + ICON_TEXT_GAP;
    draw_battery_icon(ctx, icon_x, iy, col, s_battery, large);
    graphics_draw_text(ctx, s_battery_buffer, font,
      GRect(text_x, y, w - text_x, font_h), GTextOverflowModeFill, GTextAlignmentLeft, NULL);

  } else if (field == FIELD_TEMP_F || field == FIELD_TEMP_C) {
    bool is_f  = (field == FIELD_TEMP_F);
    bool ready = is_f ? (s_weather_temp_f != INT_MIN) : (s_weather_temp_c != INT_MIN);
    if (!ready) {
      graphics_draw_text(ctx, "--", font,
        GRect(0, y, w, font_h), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    } else {
      const char *temp_str = is_f ? s_temp_f_buffer : s_temp_c_buffer;
      GSize text_size = graphics_text_layout_get_content_size(
        temp_str, font, GRect(0, 0, 200, 20),
        GTextOverflowModeFill, GTextAlignmentLeft);
      int unit_w = icon_w + ICON_TEXT_GAP + text_size.w;
      int icon_x = cx - unit_w / 2;
      int text_x = icon_x + icon_w + ICON_TEXT_GAP;
      draw_weather_icon(ctx, icon_x, iy - 1, col, weather_icon_for_code(s_weather_code), large);
      graphics_draw_text(ctx, temp_str, font,
        GRect(text_x, y, w - text_x, font_h), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
    }
  }
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

  bool large = (s_settings.OverlaySize == OVERLAY_LARGE);

  // ----------------------------------------------------------
  // EFFECTIVE COLORS
  // ----------------------------------------------------------
#if defined(PBL_BW)
  GColor bw_lit    = s_settings.InvertBW ? GColorBlack     : GColorWhite;
  GColor bw_dim    = s_settings.InvertBW ? GColorLightGray : GColorDarkGray;
  GColor col_bg    = s_settings.InvertBW ? GColorWhite     : GColorBlack;
  GColor col_fg    = s_settings.InvertBW ? GColorBlack     : GColorWhite;
  GColor col_min   = bw_lit;
  GColor col_hour  = bw_lit;
  GColor col_batt  = bw_lit;
  GColor col_step  = bw_lit;
  GColor col_dmin  = bw_dim;
  GColor col_dhour = bw_dim;
  GColor col_dbatt = bw_dim;
  GColor col_dstep = bw_dim;
  GColor col_obg   = col_bg;
  GColor col_l1    = col_fg;
  GColor col_l2    = col_fg;
  GColor col_l3    = col_fg;
  GColor col_l4    = col_fg;
  (void)col_fg;
#else
  GColor col_bg       = s_settings.BackgroundColor;
  GColor col_fg       = s_settings.TimeTextColor;
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
  GColor col_l1       = s_settings.InfoLine1Color;
  GColor col_l2       = s_settings.InfoLine2Color;
  GColor col_l3       = s_settings.InfoLine3Color;
  GColor col_l4       = s_settings.InfoLine4Color;
#endif

  // ----------------------------------------------------------
  // LAYOUT
  // ----------------------------------------------------------
  bool show_ring  = s_settings.ShowRing;
  int  inset      = show_ring ? (RING_THICK + RING_GAP) : 0;
  GRect tick_rect = GRect(inset, inset, w - 2*inset, h - 2*inset);
  int inner_short = (tick_rect.size.w < tick_rect.size.h)
                    ? tick_rect.size.w : tick_rect.size.h;

  int tick_thick = inner_short;
#if !defined(PBL_ROUND)
  if (s_settings.OverlayMode != OVERLAY_OFF) {
    tick_thick = inner_short * 19 / 164;
  }
#endif

  int radius = ((w > h) ? w : h) - RING_THICK - 1;

  graphics_context_set_stroke_width(ctx, 0);

  // ----------------------------------------------------------
  // BACKGROUND
  // ----------------------------------------------------------
  graphics_context_set_fill_color(ctx, col_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // ----------------------------------------------------------
  // TICK MARKS -- Painter's algorithm
  // ----------------------------------------------------------
  if (!is_round) {
    int filled_groups = s_minute / 5;
    int partial_min   = s_minute % 5;

#if defined(PBL_COLOR)
    int tip_deg = 0;
    if (s_minute > 0) {
      tip_deg = (partial_min > 0)
        ? 3 + 15*filled_groups + 2*(partial_min - 1)
        : 3 + 15*(filled_groups - 1) + 2*4;
    }
#endif

    int first_empty = filled_groups + (partial_min > 0 ? 1 : 0);
    graphics_context_set_fill_color(ctx, col_dmin);
    for (int g = first_empty; g < 12; g++) {
      int a = 3 + 15*g;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
    }
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, col_bg);
    for (int g = first_empty; g < 12; g++) {
      int a = 3 + 15*g;
      for (int i = 0; i < 4; i++) {
        int gap = a + 2*i + 1;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(gap), DEG_TO_TRIGANGLE(gap + 1));
      }
    }
#endif

    graphics_context_set_fill_color(ctx, col_min);
    for (int g = 0; g < filled_groups; g++) {
      int a = 3 + 15*g;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
    }
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, col_bg);
    for (int g = 0; g < filled_groups; g++) {
      int a = 3 + 15*g;
      for (int i = 0; i < 4; i++) {
        int gap = a + 2*i + 1;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(gap), DEG_TO_TRIGANGLE(gap + 1));
      }
    }
#endif

    if (partial_min > 0) {
      int a = 3 + 15*filled_groups;
      graphics_context_set_fill_color(ctx, col_dmin);
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
#if defined(PBL_COLOR)
      graphics_context_set_fill_color(ctx, col_bg);
      for (int i = 0; i < 4; i++) {
        int gap = a + 2*i + 1;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(gap), DEG_TO_TRIGANGLE(gap + 1));
      }
      graphics_context_set_fill_color(ctx, col_min);
      for (int i = 0; i < partial_min; i++) {
        int ta = a + 2*i;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 1));
      }
#else
      graphics_context_set_fill_color(ctx, col_min);
      for (int i = 0; i < partial_min; i++) {
        int ta = a + 2*i;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(ta), DEG_TO_TRIGANGLE(ta + 2));
      }
#endif
    }

#if defined(PBL_COLOR)
    if (s_minute > 0) {
      graphics_context_set_fill_color(ctx, col_min_tip);
      draw_wedge(ctx, cx, cy, radius,
                 DEG_TO_TRIGANGLE(tip_deg), DEG_TO_TRIGANGLE(tip_deg + 1));
    }
#endif

    bool is_24h       = clock_is_24h_style();
    int  filled_slots = is_24h ? (s_hour / 2) : ((s_hour % 12) ?: 12);
    int  filled_half  = s_hour % 2;

#if defined(PBL_COLOR)
    int hour_tip_slot;
    if (!is_24h) {
      hour_tip_slot = (filled_slots > 0) ? (filled_slots - 1) : 0;
    } else {
      hour_tip_slot = (filled_half == 1) ? filled_slots : (filled_slots > 0 ? filled_slots - 1 : 0);
    }
#endif

    graphics_context_set_fill_color(ctx, col_dhour);
    for (int h2 = 0; h2 < 12; h2++) {
      int a = 183 + 15*h2;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
    }
#if defined(PBL_COLOR)
    graphics_context_set_fill_color(ctx, col_bg);
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

    if (!is_24h) {
      graphics_context_set_fill_color(ctx, col_hour);
      for (int h2 = 0; h2 < filled_slots; h2++) {
        int a = 183 + 15*h2;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      }
    } else {
      graphics_context_set_fill_color(ctx, col_hour);
      for (int h2 = 0; h2 < filled_slots; h2++) {
        int a = 183 + 15*h2;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
      }
      if (filled_half == 1 && filled_slots < 12) {
        int a = 183 + 15*filled_slots;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
      }
    }

#if defined(PBL_COLOR)
    if (s_hour > 0 || is_24h) {
      if (!is_24h && filled_slots > 0) {
        graphics_context_set_fill_color(ctx, col_hour_tip);
        int a = 183 + 15*hour_tip_slot;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
      } else if (is_24h) {
        int a = 183 + 15*hour_tip_slot;
        if (filled_half == 1) {
          graphics_context_set_fill_color(ctx, col_hour_tip);
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        } else if (filled_slots > 0) {
          graphics_context_set_fill_color(ctx, col_hour);
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
          graphics_context_set_fill_color(ctx, col_hour_tip);
          draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
      }
    }
    graphics_context_set_fill_color(ctx, col_bg);
    for (int h2 = 0; h2 < filled_slots && h2 < 12; h2++) {
      int a = 183 + 15*h2;
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a + 9), DEG_TO_TRIGANGLE(a + 10));
    }
    if (filled_slots > 0) {
      int tip_a = 183 + 15*(filled_slots - 1);
      draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(tip_a + 9), DEG_TO_TRIGANGLE(tip_a + 10));
    }
    if (is_24h) {
      int cut_to = (filled_half == 1) ? filled_slots + 1 : filled_slots;
      for (int h2 = 0; h2 < cut_to && h2 < 12; h2++) {
        int a = 183 + 15*h2 + 3;
        draw_wedge(ctx, cx, cy, radius, DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
      }
    }
#endif

  } else {
    // ROUND
    graphics_context_set_fill_color(ctx, col_dmin);
    for (int i = 0; i < 60; i++) {
      int a = 3 + 2*i + 5*(i/5);
      graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                           DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
    }
    if (s_minute > 0) {
      graphics_context_set_fill_color(ctx, col_min);
      for (int i = 0; i < s_minute; i++) {
        int a = 3 + 2*i + 5*(i/5);
        graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                             DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
      }
#if defined(PBL_COLOR)
      graphics_context_set_fill_color(ctx, col_min_tip);
      {
        int i = s_minute - 1;
        int a = 3 + 2*i + 5*(i/5);
        graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                             DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 1));
      }
#endif
    }

    {
      bool is_24h       = clock_is_24h_style();
      int  filled_slots = is_24h ? (s_hour / 2) : ((s_hour % 12) ?: 12);
      int  filled_half  = s_hour % 2;
#if defined(PBL_COLOR)
      int hour_tip_slot = (filled_half == 1) ? filled_slots : (filled_slots > 0 ? filled_slots - 1 : 0);
#endif

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
        for (int h2 = 0; h2 < filled_slots; h2++) {
          int a = 183 + 15*h2;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
        }
      } else {
        graphics_context_set_fill_color(ctx, col_hour);
        for (int h2 = 0; h2 < filled_slots; h2++) {
          int a = 183 + 15*h2;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
        if (filled_half == 1 && filled_slots < 12) {
          int a = 183 + 15*filled_slots;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        }
        graphics_context_set_fill_color(ctx, col_bg);
        int cut_to = (filled_half == 1) ? filled_slots + 1 : filled_slots;
        for (int h2 = 0; h2 < cut_to && h2 < 12; h2++) {
          int a = 183 + 15*h2 + 3;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        }
      }

#if defined(PBL_COLOR)
      if (!is_24h) {
        if (filled_slots > 0) {
          graphics_context_set_fill_color(ctx, col_hour_tip);
          int a = 183 + 15*hour_tip_slot;
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 9));
        }
      } else {
        int a = 183 + 15*hour_tip_slot;
        if (filled_half == 1) {
          graphics_context_set_fill_color(ctx, col_hour_tip);
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
        } else if (filled_slots > 0) {
          graphics_context_set_fill_color(ctx, col_hour);
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a), DEG_TO_TRIGANGLE(a + 3));
          graphics_context_set_fill_color(ctx, col_hour_tip);
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
      }
#endif
    }
  }

  // ----------------------------------------------------------
  // CENTER OVERLAY CIRCLE
  // Small: 58px radius. Large: 80px radius.
  // ----------------------------------------------------------
  int overlay_r = large ? 80 : 58;
  if (prv_overlay_visible()) {
    graphics_context_set_fill_color(ctx, col_obg);
    graphics_fill_circle(ctx, GPoint(cx, cy), overlay_r);
  }

  // ----------------------------------------------------------
  // INNER GAP STRIP (rect only)
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
  // OUTER RING
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
  // TEXT / FIELD OVERLAY
  // Small: LECO_36/GOTHIC_18, circle 58, cap 11, gap 6, stride 17
  // Large: LECO_42/GOTHIC_24, circle 80, cap 15, gap 8, stride 23
  // ----------------------------------------------------------
  if (prv_overlay_visible()) {
    int time_h, cap_h, gap, stride;
    GFont time_font;
    if (large) {
      time_font = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
      time_h = 52;
      cap_h  = 15;
      gap    = 8;
    } else {
      time_font = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
      time_h = 40;
      cap_h  = 11;
      gap    = 6;
    }
    stride = cap_h + gap;

    int time_y    = cy - time_h / 2 - 2;
    int top_inner = s_settings.TopInnerField;
    int top_outer = s_settings.TopOuterField;
    int bot_inner = s_settings.BottomInnerField;
    int bot_outer = s_settings.BottomOuterField;
    int top_count = (top_inner ? 1 : 0) + (top_outer ? 1 : 0);
    int bot_count = (bot_inner ? 1 : 0) + (bot_outer ? 1 : 0);
    int font_pad  = large ? LARGE_FONT_PAD : SMALL_FONT_PAD;

    graphics_context_set_text_color(ctx, col_fg);
    graphics_draw_text(ctx, s_time_buffer, time_font,
      GRect(0, time_y, w, time_h + 4),
      GTextOverflowModeFill, GTextAlignmentCenter, NULL);

    if (top_count == 1) {
      int field = top_inner ? top_inner : top_outer;
      GColor col = top_inner ? col_l2 : col_l1;
      draw_field(ctx, field, time_y - (cap_h + gap + 2), w, cx, col, col_obg, large);
    } else if (top_count == 2) {
      int inner_y = time_y - font_pad - gap + 1;
      int outer_y = inner_y - stride;
      draw_field(ctx, top_inner, inner_y, w, cx, col_l2, col_obg, large);
      draw_field(ctx, top_outer, outer_y, w, cx, col_l1, col_obg, large);
    }

    if (bot_count == 1) {
      int field = bot_inner ? bot_inner : bot_outer;
      GColor col = bot_inner ? col_l3 : col_l4;
      draw_field(ctx, field, time_y + time_h + 2, w, cx, col, col_obg, large);
    } else if (bot_count == 2) {
      int inner_y = time_y + time_h + gap - font_pad - 3;
      int outer_y = inner_y + stride;
      draw_field(ctx, bot_inner, inner_y, w, cx, col_l3, col_obg, large);
      draw_field(ctx, bot_outer, outer_y, w, cx, col_l4, col_obg, large);
    }
  }
}

// ============================================================
// EVENT HANDLERS
// ============================================================
static void update_steps_buffer(void) {
  if (s_steps >= 1000) {
    snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%d,%03d", s_steps/1000, s_steps%1000);
  } else {
    snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%d", s_steps);
  }
}

static void tick_handler(struct tm *t, TimeUnits units_changed) {
  s_hour   = t->tm_hour;
  s_minute = t->tm_min;

  int disp_hour = clock_is_24h_style() ? t->tm_hour : ((t->tm_hour % 12) ?: 12);
  snprintf(s_time_buffer,     sizeof(s_time_buffer),     "%02d:%02d", disp_hour, t->tm_min);
  snprintf(s_day_buffer,      sizeof(s_day_buffer),      "%s", get_day_name(t->tm_wday));
  snprintf(s_date_buffer,     sizeof(s_date_buffer),     "%s %02d", get_month_abbr(t->tm_mon), t->tm_mday);
  snprintf(s_day_date_buffer, sizeof(s_day_date_buffer), "%s %s %02d",
           s_short_days[t->tm_wday], get_month_abbr(t->tm_mon), t->tm_mday);
  update_steps_buffer();
  layer_mark_dirty(s_canvas_layer);
}

static void battery_handler(BatteryChargeState state) {
  s_battery = state.charge_percent;
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", s_battery);
  layer_mark_dirty(s_canvas_layer);
}

#if defined(PBL_HEALTH)
static void update_steps(void) {
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(
    HealthMetricStepCount, time_start_of_today(), time(NULL));
  s_steps = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(HealthMetricStepCount) : 0;
  update_steps_buffer();
  layer_mark_dirty(s_canvas_layer);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) update_steps();
}
#endif

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t;
  t = dict_find(iter, MESSAGE_KEY_TimeTextColor);
  if (t) s_settings.TimeTextColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DateTextColor);
  if (t) s_settings.DateTextColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitHourColor);
  if (t) s_settings.LitHourColor      = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitMinuteColor);
  if (t) s_settings.LitMinuteColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitBatteryColor);
  if (t) s_settings.LitBatteryColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitStepsColor);
  if (t) s_settings.LitStepsColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimHourColor);
  if (t) s_settings.DimHourColor      = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimMinuteColor);
  if (t) s_settings.DimMinuteColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimBatteryColor);
  if (t) s_settings.DimBatteryColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimStepsColor);
  if (t) s_settings.DimStepsColor     = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (t) s_settings.BackgroundColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_OverlayBgColor);
  if (t) s_settings.OverlayBgColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitHourTipColor);
  if (t) s_settings.LitHourTipColor   = GColorFromHEX(t->value->int32);
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
  if (t) s_settings.InvertBW  = (t->value->int32 == 1);
  t = dict_find(iter, MESSAGE_KEY_ShowRing);
  if (t) s_settings.ShowRing  = (t->value->int32 == 1);
  t = dict_find(iter, MESSAGE_KEY_TopOuterField);
  if (t) s_settings.TopOuterField    = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_TopInnerField);
  if (t) s_settings.TopInnerField    = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_BottomInnerField);
  if (t) s_settings.BottomInnerField = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_BottomOuterField);
  if (t) s_settings.BottomOuterField = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_InfoLine1Color);
  if (t) s_settings.InfoLine1Color   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_InfoLine2Color);
  if (t) s_settings.InfoLine2Color   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_InfoLine3Color);
  if (t) s_settings.InfoLine3Color   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_InfoLine4Color);
  if (t) s_settings.InfoLine4Color   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_OverlaySize);
  if (t) s_settings.OverlaySize      = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_WeatherTempF);
  if (t) {
    s_weather_temp_f = (int)t->value->int32;
    snprintf(s_temp_f_buffer, sizeof(s_temp_f_buffer), "%dF", s_weather_temp_f);
  }
  t = dict_find(iter, MESSAGE_KEY_WeatherTempC);
  if (t) {
    s_weather_temp_c = (int)t->value->int32;
    snprintf(s_temp_c_buffer, sizeof(s_temp_c_buffer), "%dC", s_weather_temp_c);
  }
  t = dict_find(iter, MESSAGE_KEY_WeatherCode);
  if (t) s_weather_code = (int)t->value->int32;

  prv_save_settings();
  layer_mark_dirty(s_canvas_layer);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  if (s_settings.OverlayMode != OVERLAY_SHAKE) return;
  s_show_overlay = !s_show_overlay;
  layer_mark_dirty(s_canvas_layer);
}

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
  s_show_overlay   = (s_settings.OverlayMode != OVERLAY_OFF);
  s_weather_temp_f = INT_MIN;
  s_weather_temp_c = INT_MIN;
  s_weather_code   = 0;
  snprintf(s_steps_buffer,   sizeof(s_steps_buffer),   "0");
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "0%%");
  snprintf(s_temp_f_buffer,  sizeof(s_temp_f_buffer),  "--");
  snprintf(s_temp_c_buffer,  sizeof(s_temp_c_buffer),  "--");

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

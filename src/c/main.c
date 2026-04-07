#include <pebble.h>
#include <limits.h>

// ============================================================
// CONSTANTS
// ============================================================
#define SETTINGS_KEY      7          // increment when struct layout changes
#define DEFAULT_STEP_GOAL 10000
#define RING_GAP          2          // px gap between outer ring and tick radials
#define RING_THICK        6          // outer ring thickness (px)

// Overlay visibility modes
#define OVERLAY_ALWAYS_ON   0
#define OVERLAY_OFF         1
#define OVERLAY_SHAKE       2
#define OVERLAY_AUTO        3        // shake to show, auto-hides after OVERLAY_AUTO_HIDE_MS

#define OVERLAY_AUTO_HIDE_MS  60000  // auto-hide timeout for OVERLAY_AUTO mode (ms)

// Overlay sizes -- large is default on emery/gabbro (high-res screens)
#define OVERLAY_SMALL  0
#define OVERLAY_LARGE  1

// Info line field IDs -- what each of the 4 lines can display
#define FIELD_NONE      0
#define FIELD_DAY_LONG  1  // "SATURDAY"
#define FIELD_DATE      2  // "MAR 21"
#define FIELD_DAY_DATE  3  // "SAT MAR 21"
#define FIELD_STEPS     4  // footprint icon + step count
#define FIELD_TEMP_F    5  // weather icon + temperature in F
#define FIELD_TEMP_C    6  // weather icon + temperature in C
#define FIELD_BATTERY   7  // battery icon + charge %
#define FIELD_DISTANCE  8  // footprint icon + walked distance (mi or km)
#define FIELD_CALORIES  9  // flame icon + active kcal burned
#define FIELD_BT        10 // bluetooth rune -- visible when disconnected
#define FIELD_HEART_RATE 11 // heart icon + BPM (requires HR sensor)

// Weather icon types -- see weather_icon_for_code() for WMO code mapping
// 0=sun  1=partly-cloudy  2=cloud  3=rain  4=snow  5=storm

static const char *s_short_days[] = {
  "SUN","MON","TUE","WED","THU","FRI","SAT"
};

// ============================================================
// SETTINGS  (persisted at SETTINGS_KEY)
// ============================================================
typedef struct {
  // Colors -- background & overlay circle
  GColor BackgroundColor;  // watchface background
  GColor OverlayColor;     // center overlay circle fill

  // Colors -- time display (LECO digits in center overlay)
  GColor TimeColor;

  // Colors -- lit tick marks & outer ring arcs
  GColor LitHourColor;
  GColor LitMinuteColor;
  GColor LitBatteryColor;
  GColor LitStepsColor;

  // Colors -- unlit (dim) tick marks & ring arcs
  GColor DimHourColor;
  GColor DimMinuteColor;
  GColor DimBatteryColor;
  GColor DimStepsColor;

  // Colors -- leading-tick highlights (current hour/minute tick)
  GColor HourTipColor;    // independently colorable; cascades from LitHourColor
  GColor MinuteTipColor;  // independently colorable; cascades from LitMinuteColor

  // Colors -- info lines (1=top-outer, 2=top-inner, 3=bot-inner, 4=bot-outer)
  GColor Line1Color;
  GColor Line2Color;
  GColor Line3Color;
  GColor Line4Color;

  // Info line field assignments (FIELD_* values above)
  int Line1Field;  // top outer
  int Line2Field;  // top inner
  int Line3Field;  // bottom inner
  int Line4Field;  // bottom outer

  // Health, display, and behavior
  int  StepGoal;
  int  OverlayMode;  // OVERLAY_ALWAYS_ON / OVERLAY_OFF / OVERLAY_SHAKE / OVERLAY_AUTO
  int  OverlaySize;  // OVERLAY_SMALL / OVERLAY_LARGE
  bool InvertBW;     // B&W platforms only: swap black/white
  bool ShowRing;     // show/hide battery+steps outer ring
} RadiumSettings;

static RadiumSettings s_settings;

static void prv_default_settings(void) {
  s_settings.BackgroundColor = GColorBlack;
  s_settings.OverlayColor    = GColorBlack;

#if defined(PBL_COLOR)
  s_settings.TimeColor         = GColorWhite;
  s_settings.LitHourColor      = GColorGreen;
  s_settings.LitMinuteColor    = GColorGreen;
  s_settings.LitBatteryColor   = GColorGreen;
  s_settings.LitStepsColor     = GColorGreen;
  s_settings.DimHourColor      = GColorDarkGreen;
  s_settings.DimMinuteColor    = GColorDarkGreen;
  s_settings.DimBatteryColor   = GColorDarkGreen;
  s_settings.DimStepsColor     = GColorDarkGreen;
  s_settings.HourTipColor      = GColorMintGreen;
  s_settings.MinuteTipColor    = GColorMintGreen;
  s_settings.Line1Color        = GColorGreen;
  s_settings.Line2Color        = GColorMintGreen;
  s_settings.Line3Color        = GColorMintGreen;
  s_settings.Line4Color        = GColorGreen;
#else
  s_settings.TimeColor         = GColorWhite;
  s_settings.LitHourColor      = GColorWhite;
  s_settings.LitMinuteColor    = GColorWhite;
  s_settings.LitBatteryColor   = GColorWhite;
  s_settings.LitStepsColor     = GColorWhite;
  s_settings.DimHourColor      = GColorDarkGray;
  s_settings.DimMinuteColor    = GColorDarkGray;
  s_settings.DimBatteryColor   = GColorDarkGray;
  s_settings.DimStepsColor     = GColorDarkGray;
  s_settings.HourTipColor      = GColorWhite;
  s_settings.MinuteTipColor    = GColorWhite;
  s_settings.Line1Color        = GColorWhite;
  s_settings.Line2Color        = GColorWhite;
  s_settings.Line3Color        = GColorWhite;
  s_settings.Line4Color        = GColorWhite;
#endif

  s_settings.StepGoal    = DEFAULT_STEP_GOAL;
  s_settings.OverlayMode = OVERLAY_SHAKE;
  s_settings.InvertBW    = false;
#if defined(PBL_PLATFORM_APLITE)
  s_settings.ShowRing    = false;
#else
  s_settings.ShowRing    = true;
#endif

  s_settings.Line1Field  = FIELD_NONE;
  s_settings.Line2Field  = FIELD_DAY_LONG;
  s_settings.Line3Field  = FIELD_DATE;
  s_settings.Line4Field  = FIELD_NONE;

#if defined(PBL_PLATFORM_EMERY) || defined(PBL_PLATFORM_GABBRO)
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

static int  s_hour       = 0;
static int  s_minute     = 0;
static int  s_battery    = 100;
static int  s_steps      = 0;
static int  s_distance_m = 0;
static int  s_calories   = 0;
static bool s_is_charging  = false; // true when watch is on charger
static bool s_bt_connected = true;  // false when phone is disconnected
static int  s_heart_rate   = 0;    // current BPM; 0 = unavailable
static bool s_show_overlay = true;

static int  s_weather_temp_f = INT_MIN;
static int  s_weather_temp_c = INT_MIN;
static int  s_weather_code   = 0;

static char s_time_buffer[8];
static char s_day_buffer[12];
static char s_date_buffer[10];
static char s_day_date_buffer[14];
static char s_steps_buffer[12];
static char s_battery_buffer[6];
static char s_temp_f_buffer[8];
static char s_temp_c_buffer[8];
static char s_distance_buffer[10];
static char s_calories_buffer[8];
static char s_heart_rate_buffer[8]; // "72bpm" or "--"

static AppTimer *s_overlay_timer = NULL;

static GPoint    s_tri_pts[3];
static GPathInfo s_tri_info = { .num_points = 3, .points = s_tri_pts };
static GPath    *s_tri_path = NULL;

static void prv_overlay_auto_hide(void *context);

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
// WEATHER CODE -> ICON TYPE
// ============================================================
static int weather_icon_for_code(int code) {
  if (code == 0)                              return 0;
  if (code >= 1  && code <= 3)               return 1;
  if (code >= 45 && code <= 48)              return 2;
  if ((code >= 51 && code <= 67) ||
      (code >= 80 && code <= 82))            return 3;
  if ((code >= 71 && code <= 77) ||
      (code >= 85 && code <= 86))            return 4;
  if (code >= 95 && code <= 99)              return 5;
  return 2;
}

// ============================================================
// ICON DRAWING
// ============================================================
#define SMALL_FONT_PAD  8
#define LARGE_FONT_PAD  10
#define SMALL_ICON_W    11
#define LARGE_ICON_W    14
#define ICON_TEXT_GAP   2

static void draw_footprint(GContext *ctx, int fx, int fy, GColor col, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (!large) {
    graphics_fill_rect(ctx, GRect(fx,   fy,   4, 5), 2, GCornersAll);
    graphics_fill_rect(ctx, GRect(fx+1, fy+4, 2, 4), 1, GCornersAll);
  } else {
    graphics_fill_rect(ctx, GRect(fx,   fy,   5, 7), 2, GCornersAll);
    graphics_fill_rect(ctx, GRect(fx+1, fy+6, 3, 4), 1, GCornersAll);
  }
}

static void draw_steps_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  if (!large) {
    draw_footprint(ctx, ox+5, oy-1, col, false);
    draw_footprint(ctx, ox+0, oy+2, col, false);
  } else {
    draw_footprint(ctx, ox+7, oy,   col, true);
    draw_footprint(ctx, ox+0, oy+3, col, true);
  }
}

static void draw_battery_icon(GContext *ctx, int ox, int oy, GColor col, int pct, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (s_is_charging) {
    // Charging: show a lightning bolt instead of the battery outline.
    // Diagonal Z-shape, upper-right to lower-left, with a wide notch at center.
    if (!large) {
      // Upper blade: 4px wide, angling left over 4 rows (rows 0-3)
      graphics_fill_rect(ctx, GRect(ox+5, oy+0, 4, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+4, oy+1, 4, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+3, oy+2, 4, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+2, oy+3, 4, 1), 0, GCornerNone);
      // Wide middle notch (rows 4-5)
      graphics_fill_rect(ctx, GRect(ox+1, oy+4, 7, 2), 0, GCornerNone);
      // Lower blade: 4px wide, continuing left over 4 rows (rows 6-9)
      graphics_fill_rect(ctx, GRect(ox+3, oy+6, 4, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+2, oy+7, 4, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+1, oy+8, 4, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+0, oy+9, 4, 1), 0, GCornerNone);
    } else {
      // Upper blade (rows 0-4)
      graphics_fill_rect(ctx, GRect(ox+7, oy+0,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+6, oy+1,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+5, oy+2,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+4, oy+3,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+3, oy+4,  5, 1), 0, GCornerNone);
      // Wide notch (rows 5-6)
      graphics_fill_rect(ctx, GRect(ox+1, oy+5, 11, 2), 0, GCornerNone);
      // Lower blade (rows 7-11)
      graphics_fill_rect(ctx, GRect(ox+5, oy+7,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+4, oy+8,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+3, oy+9,  5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+2, oy+10, 5, 1), 0, GCornerNone);
      graphics_fill_rect(ctx, GRect(ox+1, oy+11, 5, 1), 0, GCornerNone);
    }
  } else {
    // Normal: battery outline with charge level fill.
    graphics_context_set_stroke_color(ctx, col);
    graphics_context_set_stroke_width(ctx, 1);
    if (!large) {
      graphics_draw_rect(ctx, GRect(ox, oy+2, 9, 7));
      graphics_fill_rect(ctx, GRect(ox+9, oy+4, 2, 3), 0, GCornerNone);
      int fill_w = (7 * pct) / 100;
      if (fill_w < 1 && pct > 0) fill_w = 1;
      if (fill_w > 0) graphics_fill_rect(ctx, GRect(ox+1, oy+3, fill_w, 5), 0, GCornerNone);
    } else {
      graphics_draw_rect(ctx, GRect(ox, oy+1, 12, 10));
      graphics_fill_rect(ctx, GRect(ox+12, oy+4, 2, 4), 0, GCornerNone);
      int fill_w = (10 * pct) / 100;
      if (fill_w < 1 && pct > 0) fill_w = 1;
      if (fill_w > 0) graphics_fill_rect(ctx, GRect(ox+1, oy+2, fill_w, 8), 0, GCornerNone);
    }
  }
}

// Bluetooth rune icon.
// Shown in overlay when phone connection is lost.
// Classic BT rune: vertical spine, two right-pointing chevrons, left serifs top/bottom.
static void draw_bt_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    // 11px: spine at col 3 (ox+3), chevron peak at col 6 (ox+6)
    graphics_draw_line(ctx, GPoint(ox+3, oy+0),  GPoint(ox+3, oy+10)); // spine
    graphics_draw_line(ctx, GPoint(ox+1, oy+0),  GPoint(ox+2, oy+0));  // top serif
    graphics_draw_line(ctx, GPoint(ox+1, oy+10), GPoint(ox+2, oy+10)); // bottom serif
    // Upper chevron
    graphics_draw_pixel(ctx, GPoint(ox+4, oy+1));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+2));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+3)); // peak
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+4));
    graphics_draw_pixel(ctx, GPoint(ox+4, oy+5));
    // Lower chevron
    graphics_draw_pixel(ctx, GPoint(ox+4, oy+6));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+7));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+8)); // peak
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+9));
    graphics_draw_pixel(ctx, GPoint(ox+4, oy+10));
  } else {
    // 14px: spine at col 4 (ox+4), chevron peak at col 8 (ox+8)
    graphics_draw_line(ctx, GPoint(ox+4, oy+0),  GPoint(ox+4, oy+13)); // spine
    graphics_draw_line(ctx, GPoint(ox+1, oy+0),  GPoint(ox+3, oy+0));  // top serif
    graphics_draw_line(ctx, GPoint(ox+1, oy+13), GPoint(ox+3, oy+13)); // bottom serif
    // Upper chevron
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+1));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+2));
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+3));
    graphics_draw_pixel(ctx, GPoint(ox+8, oy+4)); // peak
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+5));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+6));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+7));
    // Lower chevron
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+8));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+9));
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+10));
    graphics_draw_pixel(ctx, GPoint(ox+8, oy+11)); // peak
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+12));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+13));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+14));
  }
}

// Heart icon for heart rate field.
// Two bumps at top, tapering to a single-pixel point at bottom.
static void draw_heart_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (!large) {
    // 11px slot: 1px top pad
    int y = oy + 1;
    graphics_fill_rect(ctx, GRect(ox+1, y+0, 3, 2), 1, GCornersTop); // left bump
    graphics_fill_rect(ctx, GRect(ox+6, y+0, 3, 2), 1, GCornersTop); // right bump
    graphics_fill_rect(ctx, GRect(ox+0, y+1, 10, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, y+3, 8,  1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+2, y+4, 6,  1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+3, y+5, 4,  1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+4, y+6, 2,  1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+5, y+7, 1,  1), 0, GCornerNone); // tip
  } else {
    // 14px slot: 1px top pad
    int y = oy + 1;
    graphics_fill_rect(ctx, GRect(ox+1, y+0,  4, 2), 1, GCornersTop); // left bump
    graphics_fill_rect(ctx, GRect(ox+8, y+0,  4, 2), 1, GCornersTop); // right bump
    graphics_fill_rect(ctx, GRect(ox+0, y+1, 13, 3), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, y+4, 11, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+2, y+5,  9, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+3, y+6,  7, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+4, y+7,  5, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+5, y+8,  3, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+6, y+9,  1, 1), 0, GCornerNone); // tip
  }
}

// Stylized flame: wide rounded base tapering to a single-pixel tip.
static void draw_calories_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (!large) {
    graphics_fill_rect(ctx, GRect(ox+2, oy+7, 7, 3), 2, GCornersBottom);
    graphics_fill_rect(ctx, GRect(ox+3, oy+4, 5, 4), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+4, oy+1, 3, 4), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+5, oy+0, 1, 2), 0, GCornerNone);
  } else {
    graphics_fill_rect(ctx, GRect(ox+2, oy+9,  10, 5), 2, GCornersBottom);
    graphics_fill_rect(ctx, GRect(ox+3, oy+5,  8,  5), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+5, oy+2,  4,  4), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+6, oy+0,  2,  3), 0, GCornerNone);
  }
}

static void draw_sun_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  int sz  = large ? 14 : 11;
  int icx = ox + sz/2;
  int icy = oy + sz/2;
  graphics_draw_circle(ctx, GPoint(icx, icy), 3);
  graphics_draw_pixel(ctx, GPoint(icx,      oy));       graphics_draw_pixel(ctx, GPoint(icx,      oy+1));
  graphics_draw_pixel(ctx, GPoint(icx,      oy+sz-1));  graphics_draw_pixel(ctx, GPoint(icx,      oy+sz-2));
  graphics_draw_pixel(ctx, GPoint(ox,       icy));       graphics_draw_pixel(ctx, GPoint(ox+1,     icy));
  graphics_draw_pixel(ctx, GPoint(ox+sz-1,  icy));       graphics_draw_pixel(ctx, GPoint(ox+sz-2,  icy));
}

static void draw_cloud_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_fill_color(ctx, col);
  if (!large) {
    graphics_fill_rect(ctx, GRect(ox+2, oy+2, 4, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+3, oy+1, 3, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+6, oy+2, 3, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+3, 9, 4), 0, GCornerNone);
  } else {
    graphics_fill_rect(ctx, GRect(ox+3, oy+2,  5,  1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+4, oy+0,  4,  3), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+8, oy+1,  4,  3), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+3, 13,  6), 0, GCornerNone);
  }
}

static void draw_partly_cloudy_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_circle(ctx, GPoint(ox+7, oy+3), 2);
    graphics_draw_pixel(ctx, GPoint(ox+7,  oy));
    graphics_draw_pixel(ctx, GPoint(ox+10, oy+3));
    graphics_draw_pixel(ctx, GPoint(ox+7,  oy+6));
    graphics_context_set_fill_color(ctx, col);
    graphics_fill_rect(ctx, GRect(ox+2, oy+4, 3, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+5, oy+4, 2, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+5, 7, 4), 0, GCornerNone);
  } else {
    graphics_draw_circle(ctx, GPoint(ox+10, oy+3), 3);
    graphics_draw_pixel(ctx, GPoint(ox+10, oy));
    graphics_draw_pixel(ctx, GPoint(ox+14, oy+3));
    graphics_draw_pixel(ctx, GPoint(ox+10, oy+7));
    graphics_context_set_fill_color(ctx, col);
    graphics_fill_rect(ctx, GRect(ox+2, oy+5,  4, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+6, oy+5,  3, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(ox+1, oy+6, 10, 6), 0, GCornerNone);
  }
}

static void draw_rain_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  draw_cloud_icon(ctx, ox, oy, col, large);
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_pixel(ctx, GPoint(ox+2, oy+8));   graphics_draw_pixel(ctx, GPoint(ox+2, oy+10));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+9));   graphics_draw_pixel(ctx, GPoint(ox+5, oy+11));
    graphics_draw_pixel(ctx, GPoint(ox+8, oy+8));   graphics_draw_pixel(ctx, GPoint(ox+8, oy+10));
  } else {
    graphics_draw_pixel(ctx, GPoint(ox+2,  oy+9));  graphics_draw_pixel(ctx, GPoint(ox+2,  oy+11));
    graphics_draw_pixel(ctx, GPoint(ox+6,  oy+10)); graphics_draw_pixel(ctx, GPoint(ox+6,  oy+12));
    graphics_draw_pixel(ctx, GPoint(ox+10, oy+9));  graphics_draw_pixel(ctx, GPoint(ox+10, oy+11));
  }
}

static void draw_snow_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  int sz  = large ? 14 : 11;
  int icx = ox + sz/2;
  int icy = oy + sz/2;
  graphics_draw_line(ctx, GPoint(ox+1,    icy),      GPoint(ox+sz-2, icy));
  graphics_draw_line(ctx, GPoint(icx,     oy+1),     GPoint(icx,     oy+sz-2));
  graphics_draw_line(ctx, GPoint(ox+2,    oy+2),     GPoint(ox+sz-3, oy+sz-3));
  graphics_draw_line(ctx, GPoint(ox+sz-3, oy+2),     GPoint(ox+2,    oy+sz-3));
}

static void draw_storm_icon(GContext *ctx, int ox, int oy, GColor col, bool large) {
  draw_cloud_icon(ctx, ox, oy, col, large);
  graphics_context_set_stroke_color(ctx, col);
  graphics_context_set_stroke_width(ctx, 1);
  if (!large) {
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+7));  graphics_draw_pixel(ctx, GPoint(ox+5, oy+8));
    graphics_draw_pixel(ctx, GPoint(ox+4, oy+8));  graphics_draw_pixel(ctx, GPoint(ox+4, oy+9));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+9));  graphics_draw_pixel(ctx, GPoint(ox+6, oy+10));
    graphics_draw_pixel(ctx, GPoint(ox+5, oy+10)); graphics_draw_pixel(ctx, GPoint(ox+5, oy+11));
  } else {
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+9));  graphics_draw_pixel(ctx, GPoint(ox+7, oy+10));
    graphics_draw_pixel(ctx, GPoint(ox+6, oy+10)); graphics_draw_pixel(ctx, GPoint(ox+6, oy+11));
    graphics_draw_pixel(ctx, GPoint(ox+8, oy+11)); graphics_draw_pixel(ctx, GPoint(ox+8, oy+12));
    graphics_draw_pixel(ctx, GPoint(ox+7, oy+12)); graphics_draw_pixel(ctx, GPoint(ox+7, oy+13));
  }
}

static void draw_weather_icon(GContext *ctx, int ox, int oy, GColor col, int type, bool large) {
  switch (type) {
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
// INFO LINE DRAWING
// ============================================================
static void draw_info_line(GContext *ctx, int field, int y, int w, int cx,
                           GColor col, bool large) {
  if (field == FIELD_NONE) return;

  GFont font   = large ? fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD)
                       : fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  int font_h   = large ? 14 : 11;
  int font_pad = large ? LARGE_FONT_PAD : SMALL_FONT_PAD;
  int icon_w   = large ? LARGE_ICON_W  : SMALL_ICON_W;
  int iy       = y + font_pad;

  #define DRAW_ICON_TEXT(draw_icon_call, text_buf) do { \
    GSize sz = graphics_text_layout_get_content_size( \
      (text_buf), font, GRect(0, 0, 200, 20), \
      GTextOverflowModeFill, GTextAlignmentLeft); \
    int unit_w = icon_w + ICON_TEXT_GAP + sz.w; \
    int icon_x = cx - unit_w / 2; \
    int text_x = icon_x + icon_w + ICON_TEXT_GAP; \
    draw_icon_call; \
    graphics_draw_text(ctx, (text_buf), font, \
      GRect(text_x, y, w - text_x, font_h + 2), \
      GTextOverflowModeFill, GTextAlignmentLeft, NULL); \
  } while(0)

  graphics_context_set_text_color(ctx, col);

  if (field == FIELD_DAY_LONG) {
    graphics_draw_text(ctx, s_day_buffer, font,
      GRect(0, y, w, font_h + 2), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  } else if (field == FIELD_DATE) {
    graphics_draw_text(ctx, s_date_buffer, font,
      GRect(0, y, w, font_h + 2), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  } else if (field == FIELD_DAY_DATE) {
    graphics_draw_text(ctx, s_day_date_buffer, font,
      GRect(0, y, w, font_h + 2), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  } else if (field == FIELD_STEPS) {
    DRAW_ICON_TEXT(draw_steps_icon(ctx, icon_x, iy, col, large), s_steps_buffer);
  } else if (field == FIELD_DISTANCE) {
    DRAW_ICON_TEXT(draw_steps_icon(ctx, icon_x, iy, col, large), s_distance_buffer);
  } else if (field == FIELD_CALORIES) {
    DRAW_ICON_TEXT(draw_calories_icon(ctx, icon_x, iy, col, large), s_calories_buffer);
  } else if (field == FIELD_BATTERY) {
    DRAW_ICON_TEXT(draw_battery_icon(ctx, icon_x, iy, col, s_battery, large), s_battery_buffer);
  } else if (field == FIELD_BT) {
    // Blank when connected; show centered BT rune when disconnected.
    if (!s_bt_connected) {
      int bx = cx - (large ? LARGE_ICON_W : SMALL_ICON_W) / 2;
      draw_bt_icon(ctx, bx, iy, col, large);
    }
  } else if (field == FIELD_HEART_RATE) {
    DRAW_ICON_TEXT(draw_heart_icon(ctx, icon_x, iy, col, large), s_heart_rate_buffer);
  } else if (field == FIELD_TEMP_F || field == FIELD_TEMP_C) {
    bool is_f  = (field == FIELD_TEMP_F);
    bool ready = is_f ? (s_weather_temp_f != INT_MIN) : (s_weather_temp_c != INT_MIN);
    if (!ready) {
      graphics_draw_text(ctx, "--", font,
        GRect(0, y, w, font_h + 2), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    } else {
      const char *str = is_f ? s_temp_f_buffer : s_temp_c_buffer;
      DRAW_ICON_TEXT(
        draw_weather_icon(ctx, icon_x, iy + (large ? 0 : -1), col,
                          weather_icon_for_code(s_weather_code), large),
        str);
    }
  }

  #undef DRAW_ICON_TEXT
}

// ============================================================
// MAIN DRAW
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

#if defined(PBL_BW)
  GColor bw_lit    = s_settings.InvertBW ? GColorBlack     : GColorWhite;
  GColor bw_dim    = s_settings.InvertBW ? GColorLightGray : GColorDarkGray;
  GColor col_bg    = s_settings.InvertBW ? GColorWhite     : GColorBlack;
  GColor col_time  = s_settings.InvertBW ? GColorBlack     : GColorWhite;
  GColor col_hour  = bw_lit;
  GColor col_min   = bw_lit;
  GColor col_batt  = bw_lit;
  GColor col_step  = bw_lit;
  GColor col_dhour = bw_dim;
  GColor col_dmin  = bw_dim;
  GColor col_dbatt = bw_dim;
  GColor col_dstep = bw_dim;
  GColor col_obg   = col_bg;
  GColor col_l1    = col_time;
  GColor col_l2    = col_time;
  GColor col_l3    = col_time;
  GColor col_l4    = col_time;
#else
  GColor col_bg       = s_settings.BackgroundColor;
  GColor col_obg      = s_settings.OverlayColor;
  GColor col_time     = s_settings.TimeColor;
  GColor col_hour     = s_settings.LitHourColor;
  GColor col_min      = s_settings.LitMinuteColor;
  GColor col_batt     = s_settings.LitBatteryColor;
  GColor col_step     = s_settings.LitStepsColor;
  GColor col_dhour    = s_settings.DimHourColor;
  GColor col_dmin     = s_settings.DimMinuteColor;
  GColor col_dbatt    = s_settings.DimBatteryColor;
  GColor col_dstep    = s_settings.DimStepsColor;
  GColor col_hour_tip = s_settings.HourTipColor;
  GColor col_min_tip  = s_settings.MinuteTipColor;
  GColor col_l1       = s_settings.Line1Color;
  GColor col_l2       = s_settings.Line2Color;
  GColor col_l3       = s_settings.Line3Color;
  GColor col_l4       = s_settings.Line4Color;
#endif

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

  graphics_context_set_fill_color(ctx, col_bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  if (!is_round) {
    // ---- RECT ----
    int filled_groups = s_minute / 5;
    int partial_min   = s_minute % 5;
    int first_empty   = filled_groups + (partial_min > 0 ? 1 : 0);

#if defined(PBL_COLOR)
    int tip_deg = 0;
    if (s_minute > 0) {
      tip_deg = (partial_min > 0)
        ? 3 + 15*filled_groups + 2*(partial_min - 1)
        : 3 + 15*(filled_groups - 1) + 2*4;
    }
#endif

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
      hour_tip_slot = (filled_half == 1) ? filled_slots
                    : (filled_slots > 0 ? filled_slots - 1 : 0);
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
    // ---- ROUND ----
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
      int hour_tip_slot;
      if (!is_24h) {
        hour_tip_slot = (filled_slots > 0) ? (filled_slots - 1) : 0;
      } else {
        hour_tip_slot = (filled_half == 1) ? filled_slots
                      : (filled_slots > 0 ? filled_slots - 1 : 0);
      }
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
                               DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
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
                               DEG_TO_TRIGANGLE(a),     DEG_TO_TRIGANGLE(a + 3));
          graphics_context_set_fill_color(ctx, col_hour_tip);
          graphics_fill_radial(ctx, tick_rect, GOvalScaleModeFitCircle, tick_thick,
                               DEG_TO_TRIGANGLE(a + 6), DEG_TO_TRIGANGLE(a + 9));
        }
      }
#endif
    }
  }

  int overlay_r = large ? 70 : 58;
  if (prv_overlay_visible()) {
    graphics_context_set_fill_color(ctx, col_obg);
    graphics_fill_circle(ctx, GPoint(cx, cy), overlay_r);
  }

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
          graphics_fill_rect(ctx, GRect(cx+gap, h-t, seg, t), 0, GCornerNone);
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

  if (prv_overlay_visible()) {
    int time_h, cap_h, line_gap, single_offset;
    GFont time_font;

    if (large) {
      time_font     = fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS);
      time_h        = 52;
      cap_h         = 14;
      line_gap      = 7;
      single_offset = 16;
    } else {
      time_font     = fonts_get_system_font(FONT_KEY_LECO_36_BOLD_NUMBERS);
      time_h        = 40;
      cap_h         = 11;
      line_gap      = 6;
      single_offset = 12;
    }
    int stride   = cap_h + line_gap;
    int font_pad = large ? LARGE_FONT_PAD : SMALL_FONT_PAD;
    int time_y   = large ? (cy - time_h/2 - 1) : (cy - time_h/2 - 3);

    int f1 = s_settings.Line1Field;
    int f2 = s_settings.Line2Field;
    int f3 = s_settings.Line3Field;
    int f4 = s_settings.Line4Field;
    int top_count = (f1 ? 1 : 0) + (f2 ? 1 : 0);
    int bot_count = (f3 ? 1 : 0) + (f4 ? 1 : 0);

    graphics_context_set_text_color(ctx, col_time);
    graphics_draw_text(ctx, s_time_buffer, time_font,
      GRect(0, time_y, w, time_h + 4),
      GTextOverflowModeFill, GTextAlignmentCenter, NULL);

    if (top_count == 1) {
      int field  = f2 ? f2 : f1;
      GColor col = f2 ? col_l2 : col_l1;
      draw_info_line(ctx, field, time_y - single_offset - cap_h + 5, w, cx, col, large);
    } else if (top_count == 2) {
      int nudge   = large ? -2 : 0;
      int inner_y = time_y - font_pad - line_gap + 1 + nudge;
      int outer_y = inner_y - stride;
      draw_info_line(ctx, f2, inner_y, w, cx, col_l2, large);
      draw_info_line(ctx, f1, outer_y, w, cx, col_l1, large);
    }

    if (bot_count == 1) {
      int field  = f3 ? f3 : f4;
      GColor col = f3 ? col_l3 : col_l4;
      int nudge  = large ? -8 : -1;
      draw_info_line(ctx, field, time_y + time_h + single_offset - cap_h + nudge, w, cx, col, large);
    } else if (bot_count == 2) {
      int nudge   = large ? -7 : 0;
      int inner_y = time_y + time_h + line_gap - font_pad - 3 + nudge;
      int outer_y = inner_y + stride;
      draw_info_line(ctx, f3, inner_y, w, cx, col_l3, large);
      draw_info_line(ctx, f4, outer_y, w, cx, col_l4, large);
    }
  }
}

// ============================================================
// EVENT HANDLERS
// ============================================================
static void update_steps_buffer(void) {
  if (s_steps >= 1000) {
    snprintf(s_steps_buffer, sizeof(s_steps_buffer), "%d,%03d",
             s_steps / 1000, s_steps % 1000);
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
  snprintf(s_date_buffer,     sizeof(s_date_buffer),     "%s %02d",
           get_month_abbr(t->tm_mon), t->tm_mday);
  snprintf(s_day_date_buffer, sizeof(s_day_date_buffer), "%s %s %02d",
           s_short_days[t->tm_wday], get_month_abbr(t->tm_mon), t->tm_mday);
  update_steps_buffer();
  layer_mark_dirty(s_canvas_layer);
}

static void battery_handler(BatteryChargeState state) {
  s_battery     = state.charge_percent;
  s_is_charging = state.is_charging;
  snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", s_battery);
  layer_mark_dirty(s_canvas_layer);
}

#if defined(PBL_HEALTH)
static void update_health_data(void) {
  time_t start = time_start_of_today();
  time_t now   = time(NULL);
  HealthServiceAccessibilityMask mask;

  mask = health_service_metric_accessible(HealthMetricStepCount, start, now);
  s_steps = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(HealthMetricStepCount) : 0;
  update_steps_buffer();

  mask = health_service_metric_accessible(HealthMetricWalkedDistanceMeters, start, now);
  s_distance_m = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(HealthMetricWalkedDistanceMeters) : 0;
  if (strcmp(i18n_get_system_locale(), "en_US") == 0) {
    int miles_x10 = (s_distance_m * 10) / 1609;
    snprintf(s_distance_buffer, sizeof(s_distance_buffer),
             "%d.%dmi", miles_x10 / 10, miles_x10 % 10);
  } else {
    int km_x10 = (s_distance_m * 10) / 1000;
    snprintf(s_distance_buffer, sizeof(s_distance_buffer),
             "%d.%dkm", km_x10 / 10, km_x10 % 10);
  }

  mask = health_service_metric_accessible(HealthMetricActiveKCalories, start, now);
  s_calories = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_sum_today(HealthMetricActiveKCalories) : 0;
  snprintf(s_calories_buffer, sizeof(s_calories_buffer), "%dcal", s_calories);

  mask = health_service_metric_accessible(HealthMetricHeartRateBPM, start, now);
  s_heart_rate = (mask & HealthServiceAccessibilityMaskAvailable)
    ? (int)health_service_peek_current_value(HealthMetricHeartRateBPM) : 0;
  if (s_heart_rate > 0) {
    snprintf(s_heart_rate_buffer, sizeof(s_heart_rate_buffer), "%dbpm", s_heart_rate);
  } else {
    snprintf(s_heart_rate_buffer, sizeof(s_heart_rate_buffer), "--");
  }

  layer_mark_dirty(s_canvas_layer);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) update_health_data();
}
#endif

static void bt_handler(bool connected) {
  s_bt_connected = connected;
  if (!connected) {
    vibes_double_pulse();
  }
  layer_mark_dirty(s_canvas_layer);
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t;

  t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (t) s_settings.BackgroundColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_OverlayColor);
  if (t) s_settings.OverlayColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_TimeColor);
  if (t) s_settings.TimeColor       = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitHourColor);
  if (t) s_settings.LitHourColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitMinuteColor);
  if (t) s_settings.LitMinuteColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitBatteryColor);
  if (t) s_settings.LitBatteryColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_LitStepsColor);
  if (t) s_settings.LitStepsColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimHourColor);
  if (t) s_settings.DimHourColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimMinuteColor);
  if (t) s_settings.DimMinuteColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimBatteryColor);
  if (t) s_settings.DimBatteryColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_DimStepsColor);
  if (t) s_settings.DimStepsColor   = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_HourTipColor);
  if (t) s_settings.HourTipColor    = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_MinuteTipColor);
  if (t) s_settings.MinuteTipColor  = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_Line1Color);
  if (t) s_settings.Line1Color      = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_Line2Color);
  if (t) s_settings.Line2Color      = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_Line3Color);
  if (t) s_settings.Line3Color      = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_Line4Color);
  if (t) s_settings.Line4Color      = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_Line1Field);
  if (t) s_settings.Line1Field      = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_Line2Field);
  if (t) s_settings.Line2Field      = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_Line3Field);
  if (t) s_settings.Line3Field      = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_Line4Field);
  if (t) s_settings.Line4Field      = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_StepGoal);
  if (t) s_settings.StepGoal        = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_OverlayMode);
  if (t) {
    s_settings.OverlayMode = (int)t->value->int32;
    if (s_overlay_timer) { app_timer_cancel(s_overlay_timer); s_overlay_timer = NULL; }
    if (s_settings.OverlayMode == OVERLAY_AUTO) {
      s_show_overlay  = true;
      s_overlay_timer = app_timer_register(OVERLAY_AUTO_HIDE_MS, prv_overlay_auto_hide, NULL);
    } else {
      s_show_overlay = (s_settings.OverlayMode != OVERLAY_OFF);
    }
  }
  t = dict_find(iter, MESSAGE_KEY_OverlaySize);
  if (t) s_settings.OverlaySize = (int)t->value->int32;
  t = dict_find(iter, MESSAGE_KEY_InvertBW);
  if (t) s_settings.InvertBW    = (t->value->int32 == 1);
  t = dict_find(iter, MESSAGE_KEY_ShowRing);
  if (t) s_settings.ShowRing    = (t->value->int32 == 1);
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

static void prv_overlay_auto_hide(void *context) {
  s_overlay_timer = NULL;
  s_show_overlay  = false;
  layer_mark_dirty(s_canvas_layer);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
  if (s_settings.OverlayMode == OVERLAY_SHAKE) {
    s_show_overlay = !s_show_overlay;
    layer_mark_dirty(s_canvas_layer);
  } else if (s_settings.OverlayMode == OVERLAY_AUTO) {
    if (s_overlay_timer) { app_timer_cancel(s_overlay_timer); }
    s_show_overlay  = true;
    s_overlay_timer = app_timer_register(OVERLAY_AUTO_HIDE_MS, prv_overlay_auto_hide, NULL);
    layer_mark_dirty(s_canvas_layer);
  }
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

  s_weather_temp_f = INT_MIN;
  s_weather_temp_c = INT_MIN;
  s_weather_code   = 0;

  snprintf(s_steps_buffer,      sizeof(s_steps_buffer),      "0");
  snprintf(s_battery_buffer,    sizeof(s_battery_buffer),    "0%%");
  snprintf(s_temp_f_buffer,     sizeof(s_temp_f_buffer),     "--");
  snprintf(s_temp_c_buffer,     sizeof(s_temp_c_buffer),     "--");
  snprintf(s_distance_buffer,   sizeof(s_distance_buffer),   "--");
  snprintf(s_calories_buffer,   sizeof(s_calories_buffer),   "--");
  snprintf(s_heart_rate_buffer, sizeof(s_heart_rate_buffer), "--");

  if (s_settings.OverlayMode == OVERLAY_AUTO) {
    s_show_overlay  = true;
    s_overlay_timer = app_timer_register(OVERLAY_AUTO_HIDE_MS, prv_overlay_auto_hide, NULL);
  } else {
    s_show_overlay = (s_settings.OverlayMode != OVERLAY_OFF);
  }

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
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bt_handler
  });
  s_bt_connected = connection_service_peek_pebble_app_connection();
#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
  update_health_data();
#endif
  app_message_register_inbox_received(inbox_received);
  app_message_open(768, 64);
}

static void deinit(void) {
  if (s_overlay_timer) { app_timer_cancel(s_overlay_timer); s_overlay_timer = NULL; }
  accel_tap_service_unsubscribe();
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  connection_service_unsubscribe();
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

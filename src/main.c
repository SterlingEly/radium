#include <pebble.h>

#define SETTINGS_KEY  1
#define DEFAULT_STEP_GOAL 10000

typedef struct {
  GColor BackgroundColor;
  GColor TextColor;
  GColor BatteryColor;
  GColor StepsColor;
  GColor TickHourColor;
  GColor TickMinuteColor;
  GColor RingEmptyColor;
  int    StepGoal;
} RadiumSettings;

static RadiumSettings s_settings;

static void prv_default_settings(void) {
#if defined(PBL_COLOR)
  s_settings.BackgroundColor = GColorBlack;
  s_settings.TextColor       = GColorWhite;
  s_settings.BatteryColor    = GColorLightGray;
  s_settings.StepsColor      = GColorTiffanyBlue;
  s_settings.TickHourColor   = GColorWhite;
  s_settings.TickMinuteColor = GColorTiffanyBlue;
  s_settings.RingEmptyColor  = GColorDarkGray;
#else
  s_settings.BackgroundColor = GColorBlack;
  s_settings.TextColor       = GColorWhite;
  s_settings.BatteryColor    = GColorWhite;
  s_settings.StepsColor      = GColorWhite;
  s_settings.TickHourColor   = GColorWhite;
  s_settings.TickMinuteColor = GColorWhite;
  s_settings.RingEmptyColor  = GColorDarkGray;
#endif
  s_settings.StepGoal = DEFAULT_STEP_GOAL;
}

static void prv_save_settings(void) {
  persist_write_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

static void prv_load_settings(void) {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &s_settings, sizeof(s_settings));
}

static Window *s_window;
static Layer  *s_canvas_layer;

static int  s_hour    = 0;
static int  s_minute  = 0;
static int  s_battery = 100;
static int  s_steps   = 0;

static char s_time_buffer[16];
static char s_day_buffer[16];
static char s_date_buffer[16];

static const char *get_day_name(int wday) {
  switch (wday) {
    case 0: return "SUNDAY";
    case 1: return "MONDAY";
    case 2: return "TUESDAY";
    case 3: return "WEDNESDAY";
    case 4: return "THURSDAY";
    case 5: return "FRIDAY";
    case 6: return "SATURDAY";
    default: return "";
  }
}

static const char *get_month_abbr(int mon) {
  static const char *months[] = {
    "JAN","FEB","MAR","APR","MAY","JUN",
    "JUL","AUG","SEP","OCT","NOV","DEC"
  };
  return (mon >= 0 && mon < 12) ? months[mon] : "";
}

static void draw_layer(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(layer);
  int w = bounds.size.w;
  int h = bounds.size.h;
  int diameter = (w < h) ? w : h;
  int cx = w / 2;
  int cy = h / 2;
  int outer_inset = 2;
  int outer_thick = 6;
  int tick_margin = outer_inset + outer_thick + 2;
  int tick_thick  = 19;

  graphics_context_set_fill_color(ctx, s_settings.BackgroundColor);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  GRect outer_frame = GRect(
    cx - diameter/2 + outer_inset,
    cy - diameter/2 + outer_inset,
    diameter - 2*outer_inset,
    diameter - 2*outer_inset
  );

  graphics_context_set_fill_color(ctx, s_settings.RingEmptyColor);
  graphics_fill_radial(ctx, outer_frame, GOvalScaleModeFitCircle,
                       outer_thick,
                       DEG_TO_TRIGANGLE(3),
                       DEG_TO_TRIGANGLE(177));
  graphics_context_set_fill_color(ctx, s_settings.BatteryColor);
  if (s_battery > 0) {
    int32_t batt_end = DEG_TO_TRIGANGLE(3 + (174 * s_battery / 100));
    graphics_fill_radial(ctx, outer_frame, GOvalScaleModeFitCircle,
                         outer_thick, DEG_TO_TRIGANGLE(3), batt_end);
  }

  graphics_context_set_fill_color(ctx, s_settings.RingEmptyColor);
  graphics_fill_radial(ctx, outer_frame, GOvalScaleModeFitCircle,
                       outer_thick,
                       DEG_TO_TRIGANGLE(183),
                       DEG_TO_TRIGANGLE(357));
  int step_pct = (s_steps * 100) / s_settings.StepGoal;
  if (step_pct > 100) step_pct = 100;
  graphics_context_set_fill_color(ctx, s_settings.StepsColor);
  if (step_pct > 0) {
    int32_t steps_end = DEG_TO_TRIGANGLE(183 + (174 * step_pct / 100));
    graphics_fill_radial(ctx, outer_frame, GOvalScaleModeFitCircle,
                         outer_thick, DEG_TO_TRIGANGLE(183), steps_end);
  }

  GRect tick_frame = GRect(
    cx - diameter/2 + tick_margin,
    cy - diameter/2 + tick_margin,
    diameter - 2*tick_margin,
    diameter - 2*tick_margin
  );

  graphics_context_set_fill_color(ctx, s_settings.RingEmptyColor);
  for (int i = 0; i < 60; i++) {
    graphics_fill_radial(ctx, tick_frame, GOvalScaleModeFitCircle,
                         tick_thick,
                         DEG_TO_TRIGANGLE(3 + 2*i + 5*(i/5)),
                         DEG_TO_TRIGANGLE(3 + 2*i + 1 + 5*(i/5)));
  }
  graphics_context_set_fill_color(ctx, s_settings.TickMinuteColor);
  for (int i = 0; i < s_minute; i++) {
    graphics_fill_radial(ctx, tick_frame, GOvalScaleModeFitCircle,
                         tick_thick,
                         DEG_TO_TRIGANGLE(3 + 2*i + 5*(i/5)),
                         DEG_TO_TRIGANGLE(3 + 2*i + 1 + 5*(i/5)));
  }

  graphics_context_set_fill_color(ctx, s_settings.RingEmptyColor);
  for (int i = 0; i < 12; i++) {
    graphics_fill_radial(ctx, tick_frame, GOvalScaleModeFitCircle,
                         tick_thick,
                         DEG_TO_TRIGANGLE(180 + 3 + 15*i),
                         DEG_TO_TRIGANGLE(180 + 3 + 15*i + 9));
  }
  graphics_context_set_fill_color(ctx, s_settings.TickHourColor);
  for (int i = 0; i < s_hour; i++) {
    graphics_fill_radial(ctx, tick_frame, GOvalScaleModeFitCircle,
                         tick_thick,
                         DEG_TO_TRIGANGLE(180 + 3 + 15*i),
                         DEG_TO_TRIGANGLE(180 + 3 + 15*i + 9));
  }

  int text_y_base = (h - 33) / 2 - 4;
  graphics_context_set_text_color(ctx, s_settings.TextColor);
  graphics_draw_text(ctx, s_time_buffer,
    fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS),
    GRect(0, text_y_base, w, 36),
    GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_day_buffer,
    fonts_get_system_font(FONT_KEY_GOTHIC_14),
    GRect(0, text_y_base + 36, w, 16),
    GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  graphics_draw_text(ctx, s_date_buffer,
    fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD),
    GRect(0, text_y_base + 52, w, 16),
    GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void tick_handler(struct tm *t, TimeUnits units_changed) {
  s_hour   = t->tm_hour % 12;
  s_minute = t->tm_min;
  snprintf(s_time_buffer, sizeof(s_time_buffer), "%02d:%02d",
           clock_is_24h_style() ? t->tm_hour : t->tm_hour % 12,
           t->tm_min);
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
  HealthMetric metric = HealthMetricStepCount;
  HealthServiceAccessibilityMask mask =
    health_service_metric_accessible(metric, time_start_of_today(), time(NULL));
  if (mask & HealthServiceAccessibilityMaskAvailable) {
    s_steps = (int)health_service_sum_today(metric);
  } else {
    s_steps = 0;
  }
  layer_mark_dirty(s_canvas_layer);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventMovementUpdate) {
    update_steps();
  }
}
#endif

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *t;
#if defined(PBL_COLOR)
  t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (t) s_settings.BackgroundColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_TextColor);
  if (t) s_settings.TextColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_BatteryColor);
  if (t) s_settings.BatteryColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_StepsColor);
  if (t) s_settings.StepsColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_TickHourColor);
  if (t) s_settings.TickHourColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_TickMinuteColor);
  if (t) s_settings.TickMinuteColor = GColorFromHEX(t->value->int32);
  t = dict_find(iter, MESSAGE_KEY_RingEmptyColor);
  if (t) s_settings.RingEmptyColor = GColorFromHEX(t->value->int32);
#endif
  t = dict_find(iter, MESSAGE_KEY_StepGoal);
  if (t) s_settings.StepGoal = (int)t->value->int32;
  prv_save_settings();
  layer_mark_dirty(s_canvas_layer);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, draw_layer);
  layer_add_child(root, s_canvas_layer);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static void init(void) {
  prv_load_settings();
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_set_background_color(s_window, s_settings.BackgroundColor);
  window_stack_push(s_window, true);
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  tick_handler(t, MINUTE_UNIT);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
#if defined(PBL_HEALTH)
  health_service_events_subscribe(health_handler, NULL);
  update_steps();
#endif
  app_message_register_inbox_received(inbox_received);
  app_message_open(256, 64);
}

static void deinit(void) {
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

  } else if (field == FIELD_TEMP_F || field == FIELD_TEMP_C) {
    bool is_f  = (field == FIELD_TEMP_F);
    bool ready = is_f ? (s_weather_temp_f != INT_MIN) : (s_weather_temp_c != INT_MIN);
    if (!ready) {
      graphics_draw_text(ctx, "--", font,
        GRect(0, y, w, 13), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    } else {
      const char *temp_str = is_f ? s_temp_f_buffer : s_temp_c_buffer;
      // Weather text is short (max "100F" = 4 chars ~32px) + icon (11px) + gap (2px) = ~45px.
      // Use fixed centering: icon at cx-25, text at cx-12. Total unit ~45px centered on cx.
      int icon_x = cx - 25;
      int text_x = cx - 12;
      draw_weather_icon(ctx, icon_x, iy - 1, col, weather_icon_for_code(s_weather_code));
      graphics_draw_text(ctx, temp_str, font,
        GRect(text_x, y, w - text_x, 13), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
    }
  }
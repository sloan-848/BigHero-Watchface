#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *time_layer;
static GBitmap *s_baymax_logo;
static BitmapLayer *s_baymax_logo_layer;

static GFont *s_akashi_12;
static GFont *s_akashi_14;
static GFont *s_akashi_24;

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  int hour = tick_time->tm_hour;
  int minute = tick_time->tm_min;
  char time_buffer[8];
  if(!clock_is_24h_style()){
    if(hour > 12){
      hour -= 12;
    }
  }
  // Update the TextLayer
  snprintf(time_buffer, sizeof(time_buffer), "%i:%02i", hour, minute);
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", time_buffer);
  text_layer_set_text(time_layer, time_buffer);
}

static void bt_handler(bool connected){
  if (connected) {
    text_layer_set_text(text_layer, "Are you satisfied with your care?");
  } else {
    text_layer_set_text(text_layer, "I have been\nDISCONNECTED");
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Set up Akashi Fonts
  s_akashi_12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_AKASHI_12));
  s_akashi_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_AKASHI_14));
  s_akashi_24 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_AKASHI_24));

  // Set up background color
  #ifdef PBL_COLOR
    window_set_background_color(window, GColorRed);
  #else
    window_set_background_color(window, GColorWhite);
  #endif

  // Set up Time Layer
  char firstTime[8];
  clock_copy_time_string(firstTime, sizeof(firstTime));
  time_layer = text_layer_create((GRect) { .origin = { 0, 10}, .size = { bounds.size.w, 40 } });
  text_layer_set_text(time_layer, firstTime);
  text_layer_set_font(time_layer, s_akashi_24);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_background_color(time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  // Set up baymax png layer
  int logo_size = 90;
  s_baymax_logo = gbitmap_create_with_resource(RESOURCE_ID_BAYMAX_LOGO);
  s_baymax_logo_layer = bitmap_layer_create(GRect(0, (bounds.size.h - logo_size) / 2 - 3, bounds.size.w, logo_size));
  bitmap_layer_set_bitmap(s_baymax_logo_layer, s_baymax_logo);
  bitmap_layer_set_compositing_mode(s_baymax_logo_layer, GCompOpSet);

  // Set up Text Layer
  text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 44}, .size = { bounds.size.w, 40 } });
  text_layer_set_text(text_layer, "Are you satisfied with your care?");
  text_layer_set_font(text_layer, s_akashi_14);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  // Add baymax logo layer to window
  layer_add_child(window_layer, bitmap_layer_get_layer(s_baymax_logo_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
  gbitmap_destroy(s_baymax_logo);
  bitmap_layer_destroy(s_baymax_logo_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  //Subscribe to minute tick
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

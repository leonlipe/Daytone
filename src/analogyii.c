#include <pebble.h>

#define CONFIG_INVERTED 0
//#define CONFIG_CENTER_SECONDS_X 72;
//#define CONFIG_CENTER_SECONDS_Y 115;
#define CONFIG_RADIUS_SECS_CIRCLE 15
#define CONFIG_HAND_LENGTH_SEC 12
#define CONFIG_HAND_LENGTH_HOUR 36
#define CONFIG_HAND_LENGTH_MIN 53
#define THICKNESS 4
#define THICKNESS_HOUR 4
#define THICKNESS_SECONDS 2

typedef struct {
  int days;
  int hours;
  int minutes;
  int seconds;
} Time;

static Window *s_main_window;
static Layer *s_bg_layer,  *s_canvas_layer;
static TextLayer *s_weekday_layer, *s_day_in_month_layer, *s_month_layer, *s_12_layer, *s_9_layer, *s_6_layer, *s_digital_time_layer;
static Time s_last_time;
static char s_weekday_buffer[8], s_month_buffer[8], s_day_in_month_buffer[3];

static GPoint make_hand_point(int quantity, int intervals, int len, GPoint center) {
  return (GPoint) {
    .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.y,
  };
}

static void bg_update_proc(Layer *layer, GContext *ctx) {
 // GRect bounds = layer_get_bounds(layer);
 // GPoint center = grect_center_point(&bounds);


  GPoint center_seconds = (GPoint) {
    .x = (int16_t)72,
    .y = (int16_t)115,
  };

  // Aplite
  if (CONFIG_INVERTED){
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_fill_color(ctx, GColorBlack);
  }else{
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
  }
  // Seconds circle
  graphics_draw_circle(ctx,center_seconds,CONFIG_RADIUS_SECS_CIRCLE);
  // Marks  
  graphics_fill_rect(ctx, GRect(70, 0, 4,10), 1, GCornersAll);  
  // Day window
  graphics_fill_rect(ctx, GRect(100, 75, 25,22), 1, GCornersAll);    

}

static void draw_proc(Layer *layer, GContext *ctx) {
   // Aplite
  if (CONFIG_INVERTED){
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_fill_color(ctx, GColorBlack);
  }else{
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
  }

  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  GPoint center_seconds = (GPoint) {
    .x = (int16_t)72,
    .y = (int16_t)115,
  };

  Time now = s_last_time;

  GPoint second_hand_long = make_hand_point(now.seconds, 60, CONFIG_HAND_LENGTH_SEC, center_seconds);
  GPoint minute_hand_long = make_hand_point(now.minutes, 60, CONFIG_HAND_LENGTH_MIN, center);
  float minute_angle = TRIG_MAX_ANGLE * now.minutes / 60; //now.minutes
  float hour_angle = TRIG_MAX_ANGLE * now.hours / 12; //now.hours
  hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);
  GPoint hour_hand_long = (GPoint) {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)CONFIG_HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)CONFIG_HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.y,
  };

  for(int y = 0; y < THICKNESS; y++) {
    for(int x = 0; x < THICKNESS; x++) {
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
    }
  }
  for(int y = 0; y < THICKNESS_HOUR; y++) {
    for(int x = 0; x < THICKNESS_HOUR; x++) {
      graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
    }
  }

    for(int y = 0; y < THICKNESS_SECONDS; y++) {
      for(int x = 0; x < THICKNESS_SECONDS; x++) {       
        graphics_draw_line(ctx, GPoint(center_seconds.x + x, center_seconds.y+y ), GPoint(second_hand_long.x + x, second_hand_long.y+y ));
      }
    }


 if (CONFIG_INVERTED){
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorBlack);
  }else{
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_fill_color(ctx, GColorWhite);
  }
  
  graphics_draw_circle(ctx, GPoint(center.x + 1, center.y + 1), 4);
  graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 3);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 1);

  

}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  s_last_time.days = tick_time->tm_mday;
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.minutes = tick_time->tm_min;
  s_last_time.seconds = tick_time->tm_sec;
  snprintf(s_day_in_month_buffer, sizeof(s_day_in_month_buffer), "%d", s_last_time.days);
  strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
  strftime(s_month_buffer, sizeof(s_month_buffer), "%b", tick_time);  
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?"%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_digital_time_layer, s_buffer);
  layer_mark_dirty(s_canvas_layer);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  // Create Background Layer
  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  s_weekday_layer = text_layer_create(GRect(90, 59, 44, 40));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_weekday_layer, GColorWhite);
  text_layer_set_background_color(s_weekday_layer, GColorClear);

  s_day_in_month_layer = text_layer_create(GRect(90, 69, 44, 40));
  text_layer_set_text_alignment(s_day_in_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_day_in_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(s_day_in_month_layer, GColorBlack);
  text_layer_set_background_color(s_day_in_month_layer, GColorClear);

  s_month_layer = text_layer_create(GRect(90, 94, 44, 40));
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_month_layer, GColorWhite);
  text_layer_set_background_color(s_month_layer, GColorClear);

  s_digital_time_layer = text_layer_create(GRect(90, 110, 44, 40));
  text_layer_set_text_alignment(s_digital_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_digital_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_digital_time_layer, GColorWhite);
  text_layer_set_background_color(s_digital_time_layer, GColorClear);

  s_12_layer = text_layer_create(GRect(62, 5, 85, 28));
  text_layer_set_text_alignment(s_12_layer, GTextAlignmentCenter);
  text_layer_set_font(s_12_layer,fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(s_12_layer, GColorWhite);
  text_layer_set_background_color(s_12_layer, GColorClear);
  text_layer_set_text_alignment(s_12_layer, GTextAlignmentLeft);

  s_6_layer = text_layer_create(GRect(68, 135, 85, 28));
  text_layer_set_text_alignment(s_6_layer, GTextAlignmentCenter);
  text_layer_set_font(s_6_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(s_6_layer, GColorWhite);
  text_layer_set_background_color(s_6_layer, GColorClear);
  text_layer_set_text_alignment(s_6_layer, GTextAlignmentLeft);

  s_9_layer = text_layer_create(GRect(8, 68, 85, 28));
  text_layer_set_text_alignment(s_9_layer, GTextAlignmentCenter);
  text_layer_set_font(s_9_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(s_9_layer, GColorWhite);
  text_layer_set_background_color(s_9_layer, GColorClear);
  text_layer_set_text_alignment(s_9_layer, GTextAlignmentLeft);

  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, draw_proc);

  text_layer_set_text(s_digital_time_layer, "");  
  text_layer_set_text(s_weekday_layer, s_weekday_buffer);
  text_layer_set_text(s_day_in_month_layer, s_day_in_month_buffer);
  text_layer_set_text(s_month_layer, s_month_buffer);
  text_layer_set_text(s_12_layer, "12");
  text_layer_set_text(s_6_layer, "6");
  text_layer_set_text(s_9_layer, "9");


  layer_add_child(window_layer, text_layer_get_layer(s_digital_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_day_in_month_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_month_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_9_layer));  
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_6_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_12_layer));
  layer_add_child(window_layer, s_canvas_layer);
}


static void window_unload(Window *window) {
  layer_destroy(s_bg_layer);
  layer_destroy(s_canvas_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_day_in_month_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_12_layer);
  text_layer_destroy(s_9_layer);
  text_layer_destroy(s_6_layer);
  text_layer_destroy(s_digital_time_layer);
 

}



static void init() {
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  s_main_window = window_create();

  //Aplite
  if (CONFIG_INVERTED){
    window_set_background_color(s_main_window, GColorWhite);
  }else{
    window_set_background_color(s_main_window, GColorBlack);
  }
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  time_t t = time(NULL);
  struct tm *tm_now = localtime(&t);
  tick_handler(tm_now, SECOND_UNIT);

   
}

static void deinit() {
   window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
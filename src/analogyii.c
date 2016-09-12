#include <pebble.h>

#define CONFIG_INVERTED 0
#define CONFIG_CENTER_SECONDS_X 72
#define CONFIG_CENTER_SECONDS_Y 115
#define CONFIG_RADIUS_SECS_CIRCLE 18
#define CONFIG_HAND_LENGTH_SEC 15
#define CONFIG_HAND_LENGTH_HOUR 45
#define CONFIG_HAND_LENGTH_MIN 65
#define THICKNESS_MINUTES 4
#define THICKNESS_HOUR 4
#define THICKNESSMARKS 4
#define THICKNESS_SECONDS 2
#define DRAW_PATH_HAND 0
#define CONFIG_X_START_INFO_BOX 100

#define MESSAGE_KEY_GET_WEATHER 1
#define SECONDS_FOR_POLL 3600

#define WEATHER_TEMPERATURE_KEY 1
#define WEATHER_TEMPERATURE_FORE_KEY 2



// Message sizes
const uint32_t inbox_size = 64;
const uint32_t outbox_size = 256;

typedef struct {
  int days;
  int hours;
  int minutes;
  int seconds;
} Time;

static unsigned int last_time_weather;

static Window *s_main_window;
static Layer *s_bg_layer,  *s_canvas_layer, *s_forecast_layer;
static TextLayer *s_weekday_layer, *s_day_in_month_layer, *s_month_layer, *s_digital_time_layer, *s_temperature_layer;
static Layer *s_background_layer;
//static GBitmap *s_background_bitmap;

//static temp_vals[];

static Time s_last_time;
static char s_weekday_buffer[8], s_month_buffer[8], s_day_in_month_buffer[3];

static GPath *s_hour_hand_path_ptr = NULL, *s_minute_hand_path_ptr = NULL;
static const GPathInfo MINUTE_HAND_PATH = {
  .num_points = 4,
  .points = (GPoint []) {{-3, 0}, {-3, CONFIG_HAND_LENGTH_MIN*-1}, {3, CONFIG_HAND_LENGTH_MIN*-1}, {3, 0}}
};
static const GPathInfo HOUR_HAND_PATH = {
  .num_points = 4,
  .points = (GPoint []) {{-3, 0}, {-3, CONFIG_HAND_LENGTH_HOUR*-1}, {3, CONFIG_HAND_LENGTH_HOUR*-1}, {3, 0}}
};



static int32_t getMarkSize(int h){
  int32_t resultado = 75;
  switch(h){
    case 0  :
       resultado = 75;
       break; 
    case 1  :
       resultado = 95;
       break; 
    case 2  :
       resultado = 75;
       break;    
    case 3  :
       resultado = 65;
       break;    
    case 4  :
       resultado = 75;
       break;    
    case 5  :
       resultado = 90;
       break;    
    case 6  :
       resultado = 77;
       break;    
    case 7  :
       resultado = 90;
       break;    
    case 8  :
       resultado = 79;
       break;    
    case 9  :
       resultado = 69;
       break;    
    case 10  :
       resultado = 79 ;
       break;    
    case 11  :
       resultado = 95;
       break;       
} 
return resultado;
}


/*
 * Este procedimiento devuelve un punto relativo al centro de la pantalla, para poder dibujar
 * una manecilla.
 *
 */
static GPoint make_hand_point(int quantity, int intervals, int len, GPoint center) {
  return (GPoint) {
    .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * quantity / intervals) * (int32_t)len / TRIG_MAX_RATIO) + center.y,
  };
}

/*
  Este procedimiento dibuja el fondo de la watchface
 */
static void bg_update_proc(Layer *layer, GContext *ctx) {
  GPoint center_seconds = (GPoint) {
    .x = (int16_t)CONFIG_CENTER_SECONDS_X,
    .y = (int16_t)CONFIG_CENTER_SECONDS_Y,
  };
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
// Aplite


  for(int h = 0; h < 12; h++) { 



        GPoint point = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.y,
        };

        GPoint point02 = (GPoint) {
          //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
          //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
          .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * getMarkSize(h) / TRIG_MAX_RATIO) + center.x,
          //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
          .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * getMarkSize(h) / TRIG_MAX_RATIO) + center.y,
        };

        if (h == 0 || h == 3 || h ==6 || h == 9){
            #if defined(PBL_COLOR)
                graphics_context_set_stroke_color(ctx, GColorWhite);
                graphics_context_set_fill_color(ctx, GColorWhite);
            #elif defined(PBL_BW)
              if (CONFIG_INVERTED){
                graphics_context_set_stroke_color(ctx, GColorBlack);
                graphics_context_set_fill_color(ctx, GColorBlack);
              }else{
                graphics_context_set_stroke_color(ctx, GColorWhite);
                graphics_context_set_fill_color(ctx, GColorWhite);
              }
            #endif
            }else{
                #if defined(PBL_COLOR)
                    graphics_context_set_stroke_color(ctx, GColorYellow);
                    graphics_context_set_fill_color(ctx, GColorYellow);
                #elif defined(PBL_BW)
                  if (CONFIG_INVERTED){
                    graphics_context_set_stroke_color(ctx, GColorBlack);
                    graphics_context_set_fill_color(ctx, GColorBlack);
                  }else{
                    graphics_context_set_stroke_color(ctx, GColorWhite);
                    graphics_context_set_fill_color(ctx, GColorWhite);
                  }
                #endif
            }
              
            for(int y = 0; y < THICKNESSMARKS; y++) {
              for(int x = 0; x < THICKNESSMARKS; x++) {
                graphics_draw_line(ctx, GPoint(point02.x + x, point02.y + y), GPoint(point.x + x, point.y + y));
              }
            }
    
  }

    #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
  #elif defined(PBL_BW)
    if (CONFIG_INVERTED){
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    }
  #endif
  
  // Seconds circle
  graphics_draw_circle(ctx,center_seconds,CONFIG_RADIUS_SECS_CIRCLE);



 
  // Marks  
  //graphics_fill_rect(ctx, GRect(70, 0, 4,10), 1, GCornersAll);  
  // Day window
  // 
  //graphics_fill_rect(ctx, GRect(CONFIG_X_START_INFO_BOX+10, 75, 25,22), 1, GCornersAll);  


}


static void refresh_temp_fore_graph(char fore[20]){

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Val: %s", fore);

    //strtok(fore, "|");

  //while( token != NULL ) {
     // APP_LOG(APP_LOG_LEVEL_DEBUG, "Val: %d", (int)token);

  //  token = strtok(NULL, "|");
  //}
    

 //graphics_fill_rect(ctx, GRect(CONFIG_X_START_INFO_BOX+10, 75, 25,22), 1, GCornersAll);  

}

static void update_forecast_graph(Layer *layer, GContext *ctx) {
  // Custom drawing happens here
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int x = 0; x<5; x++){
    graphics_fill_rect(ctx, GRect((x+(x*4+1)), 10, 4,12), 0, GCornersAll);  
  }

}

/*
  Este procedimiento dibuja las manecillas del reloj y todo lo que cambie cada TICK TIME

 */
static void draw_proc(Layer *layer, GContext *ctx) {

    GRect bounds = layer_get_bounds(layer);
    GPoint center = grect_center_point(&bounds);
    GPoint center_seconds = (GPoint) {
      .x = (int16_t)72,
      .y = (int16_t)115,
    };

    Time now = s_last_time;

    GPoint second_hand_long = make_hand_point(now.seconds, 60, CONFIG_HAND_LENGTH_SEC, center_seconds);
    float minute_angle = TRIG_MAX_ANGLE * now.minutes / 60; //now.minutes
    float hour_angle = TRIG_MAX_ANGLE * now.hours / 12; //now.hours
    hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);

  if (DRAW_PATH_HAND){
      // Draw Hours Hand
    gpath_rotate_to(s_hour_hand_path_ptr,hour_angle);
    gpath_rotate_to(s_minute_hand_path_ptr,minute_angle);
      // Aplite
    if (CONFIG_INVERTED){
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_fill_color(ctx, GColorWhite);
    }
    gpath_draw_filled(ctx, s_hour_hand_path_ptr);
    gpath_draw_filled(ctx, s_minute_hand_path_ptr);

    if (CONFIG_INVERTED){
      graphics_context_set_fill_color(ctx, GColorWhite);
    }else{
      graphics_context_set_fill_color(ctx, GColorBlack);
    }
    gpath_draw_outline(ctx, s_hour_hand_path_ptr);  
    gpath_draw_outline(ctx, s_minute_hand_path_ptr);  

    // Aplite
    if (CONFIG_INVERTED){
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    }
    graphics_draw_line(ctx, GPoint(center_seconds.x , center_seconds.y ), GPoint(second_hand_long.x, second_hand_long.y ));


  }else{
     // Aplite
    if (CONFIG_INVERTED){
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    }

    GPoint minute_hand_long = make_hand_point(now.minutes, 60, CONFIG_HAND_LENGTH_MIN, center);    
    GPoint hour_hand_long = (GPoint) {
      .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)CONFIG_HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)CONFIG_HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.y,
    };


    for(int y = 0; y < THICKNESS_MINUTES; y++) {
      for(int x = 0; x < THICKNESS_MINUTES; x++) {
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
      }
    }
   
    #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorRed);
      graphics_context_set_fill_color(ctx, GColorRed);
    #elif defined(PBL_BW)
      if (CONFIG_INVERTED){
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorBlack);
      }else{
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_fill_color(ctx, GColorWhite);
      }
    #endif

    for(int y = 0; y < THICKNESS_HOUR; y++) {
      for(int x = 0; x < THICKNESS_HOUR; x++) {
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
      }
    }


     #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorVividCerulean);
      graphics_context_set_fill_color(ctx, GColorVividCerulean);
    #elif defined(PBL_BW)
      if (CONFIG_INVERTED){
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorBlack);
      }else{
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_fill_color(ctx, GColorWhite);
      }
    #endif

      for(int y = 0; y < THICKNESS_SECONDS; y++) {
        for(int x = 0; x < THICKNESS_SECONDS; x++) {       
          graphics_draw_line(ctx, GPoint(center_seconds.x + x, center_seconds.y+y ), GPoint(second_hand_long.x + x, second_hand_long.y+y ));
        }
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
   if (CONFIG_INVERTED){
    graphics_context_set_fill_color(ctx, GColorWhite);
  }else{
    graphics_context_set_fill_color(ctx, GColorBlack);
  }
  graphics_fill_circle(ctx, GPoint(center.x + 1, center.y + 1), 1);

  



}


static void get_weather(){
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Enter Get Weather");

  // Declare the dictionary's iterator
  DictionaryIterator *out_iter;

  // Prepare the outbox buffer for this message
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if(result == APP_MSG_OK) {
    // Add an item to ask for weather data
    int value = 0;
    dict_write_int(out_iter, MESSAGE_KEY_GET_WEATHER, &value, sizeof(int), true);

    // Send this message
    result = app_message_outbox_send();
    if(result != APP_MSG_OK) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }

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

  unsigned int now = mktime(tick_time);
  if (now > last_time_weather + SECONDS_FOR_POLL ){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Calling weather");
    last_time_weather = mktime(tick_time);
    get_weather();
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Time :%u", last_time_weather);

}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  // Create Background Layer
  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  s_weekday_layer = text_layer_create(GRect(25, 33, 44, 40));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_weekday_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_weekday_layer, GColorWhite);
  }
  text_layer_set_background_color(s_weekday_layer, GColorClear);

  s_day_in_month_layer = text_layer_create(GRect(50, 25, 44, 40));
  text_layer_set_text_alignment(s_day_in_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_day_in_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_day_in_month_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_day_in_month_layer, GColorWhite);
  }  
  text_layer_set_background_color(s_day_in_month_layer, GColorClear);

  s_month_layer = text_layer_create(GRect(75, 33, 44, 40));
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_month_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_month_layer, GColorWhite);
  } 
  text_layer_set_background_color(s_month_layer, GColorClear);

  s_digital_time_layer = text_layer_create(GRect(CONFIG_X_START_INFO_BOX, 110, 44, 40));
  text_layer_set_text_alignment(s_digital_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_digital_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_digital_time_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_digital_time_layer, GColorWhite);
  } 
  text_layer_set_background_color(s_digital_time_layer, GColorClear);

  // s_temperature_layer = text_layer_create(GRect(CONFIG_X_START_INFO_BOX, 44, 44, 40));
  // text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentCenter);
  // text_layer_set_font(s_temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  // if (CONFIG_INVERTED){
  //   text_layer_set_text_color(s_temperature_layer, GColorBlack);
  // }else{
  //   text_layer_set_text_color(s_temperature_layer, GColorWhite);
  // }  
  // text_layer_set_background_color(s_temperature_layer, GColorClear);


  // s_forecast_layer = layer_create(GRect(CONFIG_X_START_INFO_BOX+10, 24, 44, 40));
  // layer_set_update_proc(s_forecast_layer, update_forecast_graph);

// TODO: Change sizes
  s_background_layer = layer_create(GRect(0, 0, 144, 168));
 // bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
 // bitmap_layer_set_compositing_mode(s_background_layer, GCompOpSet);

  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, draw_proc);

  text_layer_set_text(s_digital_time_layer, "");  
  text_layer_set_text(s_weekday_layer, s_weekday_buffer);
  text_layer_set_text(s_day_in_month_layer, s_day_in_month_buffer);
  text_layer_set_text(s_month_layer, s_month_buffer);
 // text_layer_set_text(s_temperature_layer, "...");  

  layer_add_child(window_layer, s_background_layer);
  layer_add_child(window_layer, text_layer_get_layer(s_day_in_month_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_month_layer));
  //layer_add_child(window_layer, text_layer_get_layer(s_digital_time_layer));
  //layer_add_child(window_layer, text_layer_get_layer(s_temperature_layer));
  layer_add_child(window_layer, s_canvas_layer);
  //layer_add_child(window_layer, s_forecast_layer);


   s_hour_hand_path_ptr = gpath_create(&HOUR_HAND_PATH);
   s_minute_hand_path_ptr = gpath_create(&MINUTE_HAND_PATH);
  
  // Translate by (5, 5):
  gpath_move_to(s_hour_hand_path_ptr, GPoint(72, 84));
  gpath_move_to(s_minute_hand_path_ptr, GPoint(72, 84));
}


static void window_unload(Window *window) {
  layer_destroy(s_background_layer);
  layer_destroy(s_bg_layer);
  layer_destroy(s_canvas_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_day_in_month_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_digital_time_layer);
 // text_layer_destroy(s_temperature_layer);
  //layer_destroy(s_forecast_layer);
  gpath_destroy(s_hour_hand_path_ptr);
  gpath_destroy(s_minute_hand_path_ptr);
 // gbitmap_destroy(s_background_bitmap);

 

}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  // A new message has been successfully received
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Mensaje recibido");

  static char weather_temp_buff[4];
  static char weather_temp_fore_buff[20];
  Tuple *t = dict_read_first(iter);
  while(t != NULL) {
    switch(t->key) {
       case WEATHER_TEMPERATURE_KEY:
         APP_LOG(APP_LOG_LEVEL_DEBUG, "Temperatura: %d", (int)t->value->int32);
         snprintf(weather_temp_buff, sizeof(weather_temp_buff), "%dC", (int)t->value->int32);
       break;
        case WEATHER_TEMPERATURE_FORE_KEY:
         APP_LOG(APP_LOG_LEVEL_DEBUG, "Forecast: %s", t->value->cstring);
         snprintf(weather_temp_fore_buff, sizeof(weather_temp_fore_buff), "%s", t->value->cstring);
       break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
  t = dict_read_next(iter);    
  }
  text_layer_set_text(s_temperature_layer, weather_temp_buff);  
  refresh_temp_fore_graph(weather_temp_fore_buff);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  // A message was received, but had to be dropped
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. Reason: %d", (int)reason);
}

static void outbox_sent_callback(DictionaryIterator *iter, void *context) {
  // The message just sent has been successfully delivered

}

static void outbox_failed_callback(DictionaryIterator *iter,
                                      AppMessageResult reason, void *context) {
  // The message just sent failed to be delivered
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message send failed. Reason: %d", (int)reason);
}

static void init() {
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  s_main_window = window_create();
 // s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_BW_IMAGE);

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

  // Communications initialization
  // app_message_open(inbox_size, outbox_size);
  // app_message_register_inbox_received(inbox_received_callback);
  // app_message_register_inbox_dropped(inbox_dropped_callback);
  // app_message_register_outbox_sent(outbox_sent_callback);
  // app_message_register_outbox_failed(outbox_failed_callback);


   
}

static void deinit() {
   window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
#include <pebble.h>
#include "health.h"

#define CONFIG_INVERTED 0
//#define CONFIG_CENTER_SECONDS_X 72
//#define CONFIG_CENTER_SECONDS_Y 120
#define SECONDS_CENTER_OFFSET_X 30 // 30
#define SECONDS_CENTER_OFFSET_Y 36 // 36
#define CONFIG_HAND_LENGTH_SEC 22
#define CONFIG_RADIUS_SECS_CIRCLE 23
#define CONFIG_HAND_LENGTH_HOUR 45
#define CONFIG_HAND_LENGTH_MIN 65
#define THICKNESS_MINUTES 4
#define THICKNESS_HOUR 4
#define THICKNESSMARKS 5
#define THICKNESS_SECONDS 2

#define DRAW_PATH_HAND 1
#define CENTER_HANDS_OFFSET 0
#define CENTER_HANDS_RADIUS 4
#define CONFIG_X_START_INFO_BOX 100
#define PATH_HANDS_INVERSE_LEGTH 15

#define CONFIG_CENTER_INFOLEFT_X 42
#define CONFIG_CENTER_INFOLEFT_Y 48
#define CONFIG_RADIUS_INFOLEFT_CIRCLE 23
#define CONFIG_CENTER_INFORIGHT_X 102
#define CONFIG_CENTER_INFORIGHT_Y 48
#define CONFIG_RADIUS_INFORIGHT_CIRCLE 23

#define MESSAGE_KEY_GET_WEATHER 1
#define SECONDS_FOR_POLL 3600

#define WEATHER_TEMPERATURE_KEY 1
#define WEATHER_TEMPERATURE_FORE_KEY 2
#define SHOW_MINUTES_MARKS 1
#define SHOW_HOUR_MARKS 1
#define TIME_NUMERALS_FONT FONT_KEY_LECO_20_BOLD_NUMBERS
#define XPOS_12H 55
#define XPOS_01H 95
#define XPOS_02H 105
#define XPOS_03H 105
#define XPOS_04H 105
#define XPOS_05H 95
#define XPOS_06H 55
#define XPOS_07H 15
#define XPOS_08H 5
#define XPOS_09H 5
#define XPOS_10H 5
#define XPOS_11H 15

#define YPOS_12H 10
#define YPOS_01H 10
#define YPOS_02H 40
#define YPOS_03H 75
#define YPOS_04H 110
#define YPOS_05H 135
#define YPOS_06H 138
#define YPOS_07H 135
#define YPOS_08H 110
#define YPOS_09H 75
#define YPOS_10H 40
#define YPOS_11H 10

#define XLENHOURS 40
#define YLENHOURS 40

#define CIRCLE_SECONDS_AREA true

#define CONFIG_SECONDS_HAND_INVERSED false
#define CONFIG_INFOLEFT_HAND_INVERSED false
#define CONFIG_INFORIGHT_HAND_INVERSED false

#define DEBUG true


#define HOUR_MARKERS_COLOR GColorLightGray
#define MINUTE_MARKERS_COLOR GColorLightGray
#define HOUR_HAND_COLOR GColorLightGray
#define MINUTE_HAND_COLOR GColorWhite
#define SECONDS_HAND_COLOR  GColorVividCerulean
#define INFOLEFT_HAND_COLOR  GColorVividCerulean
#define INFORIGHT_HAND_COLOR GColorVividCerulean
#define INFOLEFT_CIRCLE_COLOR  GColorVividCerulean
#define INFORIGHT_CIRCLE_COLOR  GColorVividCerulean
#define CIRCLES_COLOR  GColorBlack

#define SUBSCRIBE_TO_BATTERY true
#define SUBSCRIBE_TO_HEALTH true
#define SHOWINFOCIRCLES true


static GFont s_connection_icons_14;

// Message sizes
const uint32_t inbox_size = 64;
const uint32_t outbox_size = 256;

typedef struct {
  int days;
  int hours;
  int minutes;
  int seconds;
  int wday;
  int month;
} Time;

static unsigned int last_time_weather;


 /**
  * Config vars
  */
 

static Window *s_main_window;
static Layer *s_bg_layer,  *s_canvas_layer, *s_seconds_layer, *s_battery_layer, *s_health_layer, *s_window_layer;
static TextLayer *s_12_hour_layer, *s_01_hour_layer, *s_02_hour_layer, *s_03_hour_layer, *s_04_hour_layer, *s_05_hour_layer, *s_06_hour_layer, *s_07_hour_layer, *s_08_hour_layer, *s_09_hour_layer, *s_10_hour_layer, *s_11_hour_layer;
static TextLayer *s_weekday_layer, *s_day_in_month_layer, *s_month_layer, *s_digital_time_layer, *s_temperature_layer, *s_connection_layer;
static Layer *s_background_layer;
//static GBitmap *s_background_bitmap;

//static temp_vals[];

static Time s_last_time;
static char s_weekday_buffer[8], s_month_buffer[8], s_day_in_month_buffer[3];
static int s_weekday_number;

static GPath *s_hour_hand_path_ptr = NULL, *s_minute_hand_path_ptr = NULL,*s_hour_hand_path_bold_ptr = NULL, *s_minute_hand_path_bold_ptr = NULL;

static const GPathInfo MINUTE_HAND_PATH = {
  .num_points = 4,
  .points = (GPoint []) { {-3, PATH_HANDS_INVERSE_LEGTH}, 
                          {-3, CONFIG_HAND_LENGTH_MIN*-1}, 
                          {3, CONFIG_HAND_LENGTH_MIN*-1}, 
                          {3, PATH_HANDS_INVERSE_LEGTH}}
};
static const GPathInfo HOUR_HAND_PATH = {
  .num_points = 4,
  .points = (GPoint []) {{-3, PATH_HANDS_INVERSE_LEGTH}, 
                          {-3, CONFIG_HAND_LENGTH_HOUR*-1}, 
                          {3, CONFIG_HAND_LENGTH_HOUR*-1}, 
                          {3, PATH_HANDS_INVERSE_LEGTH}}
};

static const GPathInfo MINUTE_HAND_PATH_BOLD = {
  .num_points = 4,
  .points = (GPoint []) {{-4, PATH_HANDS_INVERSE_LEGTH}, 
                          {-4, CONFIG_HAND_LENGTH_MIN*-1}, 
                          {4, CONFIG_HAND_LENGTH_MIN*-1}, 
                          {4, PATH_HANDS_INVERSE_LEGTH}}
};
static const GPathInfo HOUR_HAND_PATH_BOLD = {
  .num_points = 4,
  .points = (GPoint []) {{-4, PATH_HANDS_INVERSE_LEGTH}, 
                          {-4, CONFIG_HAND_LENGTH_HOUR*-1}, 
                          {4, CONFIG_HAND_LENGTH_HOUR*-1}, 
                          {4, PATH_HANDS_INVERSE_LEGTH}}
};



typedef struct {
  int charge_percent;
  bool is_charging;
  bool is_plugged;
} Battery;

typedef struct {
 bool enableSeconds;
 bool enableBattery;
 bool enableHealth; 
 bool enableInfoLeft; 
 bool enableInfoRight;
 bool enableMinutesMarks;
 bool enableHourMarks;
 bool enableConnection;
 int backgroundcolor;
 int hourHandsColor;
 int minuteHandsColor;
 int smallHandsColor;
 int hourMarkersColor;
 int minuteMarkersColor;
 int numbersColor;
 int batteryCircleColor;
 int healthCircleColor;
 int infoCirclesColor;
 int infoLeftBackColor;
 int infoRightBackColor;
 int secondsBackColor;
 int dayInMonthcolor;

} Configuration;

static Battery s_last_battery;
static Configuration config;

static void handle_battery(BatteryChargeState charge_state) {
  s_last_battery.charge_percent = charge_state.charge_percent;
  s_last_battery.is_charging = charge_state.is_charging;
  s_last_battery.is_plugged = charge_state.is_plugged;
  
  layer_mark_dirty(s_battery_layer);
}


int inverse_hand(int actual_time){
  int new_time = actual_time + 30;
  if (new_time > 60){
    new_time -= 60;
  }
  return new_time;
}  

int inverse_hand_hour(int actual_time){
  int new_time = actual_time + 6;
  if (new_time > 12){
    new_time -= 12;
  }
  return new_time;  
}

static int32_t getMarkSize(int h){
  int32_t resultado = 75;
  switch(h){
    case 0  :
       resultado = 80;
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

static int32_t getMarkSizeForMinutes(int m){
  int32_t resultado = 75;
 
  return resultado;
}

static void refreshAllLayers(){
  text_layer_set_text_color(s_12_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_01_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_02_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_03_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_04_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_05_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_06_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_07_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_08_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_09_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_10_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_11_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_text_color(s_day_in_month_layer, GColorFromHEX(config.dayInMonthcolor));
  text_layer_set_text_color(s_month_layer, GColorFromHEX(config.infoCirclesColor)); 
  text_layer_set_text_color(s_weekday_layer, GColorFromHEX(config.infoCirclesColor)); 
  window_set_background_color(s_main_window, GColorFromHEX(config.backgroundcolor));

  layer_mark_dirty(s_background_layer);
  layer_mark_dirty(s_battery_layer);
  layer_mark_dirty(s_bg_layer);
  layer_mark_dirty(s_health_layer);
  layer_mark_dirty(s_seconds_layer);
  layer_mark_dirty(s_canvas_layer);


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

/**
 * Funcion que maneja el estado de la conexion o desconexion del reloj
 * @param connected Si esta conectado o no
 */
static void app_connection_handler(bool connected) {  
  if(DEBUG)
    APP_LOG(APP_LOG_LEVEL_INFO, "Pebble app %sconnected", connected ? "" : "dis");
  if (connected){
    text_layer_set_text(s_connection_layer, "");    
  }else{
    text_layer_set_text(s_connection_layer, "ha");
  }
}

static void kit_connection_handler(bool connected) {
  if(DEBUG)
    APP_LOG(APP_LOG_LEVEL_INFO, "PebbleKit %sconnected", connected ? "" : "dis"); 
}

static void change_layers(bool value){
    layer_set_hidden(s_background_layer, value);
    layer_set_hidden(s_battery_layer, value);
    layer_set_hidden(s_health_layer, value);
    layer_set_hidden(s_bg_layer, value);
    layer_set_hidden(s_seconds_layer, value);
    layer_set_hidden(text_layer_get_layer(s_12_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_01_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_02_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_03_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_04_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_05_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_06_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_07_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_08_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_09_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_10_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_11_hour_layer), value);
    layer_set_hidden(text_layer_get_layer(s_month_layer), value);
    layer_set_hidden(text_layer_get_layer(s_day_in_month_layer), value);
    layer_set_hidden(text_layer_get_layer(s_weekday_layer), value);
    layer_set_hidden(text_layer_get_layer(s_connection_layer), value);

}

static void prv_unobstructed_will_change(GRect final_unobstructed_screen_area,
void *context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(s_window_layer);
  if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
    // Screen is about to become obstructed, hide the date
    change_layers(true);
  }
}

static void prv_unobstructed_did_change(void *context) {
  // Get the full size of the screen
  GRect full_bounds = layer_get_bounds(s_window_layer);
  // Get the total available screen real-estate
  GRect bounds = layer_get_unobstructed_bounds(s_window_layer);
  if (grect_equal(&full_bounds, &bounds)) {
    // Screen is no longer obstructed, show the date
    change_layers(false);
  }
}

/*
  Procedimiento que dibuja el circulo de Pebble Health
 */

static void health_layer_update(Layer *layer, GContext *ctx) {
 
// 42, 48  ---- 23 radius
// Dibujar el circulo que servirá para la bateria
  if (config.enableHealth && config.enableInfoRight){
    int health_steps_today = health_get_steps_today();
    graphics_context_set_fill_color(ctx, GColorFromHEX(config.healthCircleColor));
    int steps_goal_percent = 10;
    if (health_steps_today <= HEALTH_STEPS_GOAL){
       steps_goal_percent = health_steps_today * 10 / HEALTH_STEPS_GOAL;
     }

    
      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Health Val: %d",health_steps_today );

      graphics_fill_radial(ctx, GRect(83, 29, 39, 39), GOvalScaleModeFitCircle, 3, 0, DEG_TO_TRIGANGLE(36 * steps_goal_percent));
    }else{
      //TODO:Color
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
      graphics_fill_radial(ctx, GRect(83, 29, 39, 39), GOvalScaleModeFitCircle, 3, 0, DEG_TO_TRIGANGLE(360));
    }
    GRect full_bounds = layer_get_bounds(s_window_layer);
    GRect final_unobstructed_screen_area = layer_get_unobstructed_bounds(s_window_layer);
    if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
        change_layers(true);
    }else{
      change_layers(false);
    }

}

/* 
  Procedimiento que dibuja el circulo de la bateria
*/

static void battery_layer_update(Layer *layer, GContext *ctx) {
 
// 42, 48  ---- 23 radius
// Dibujar el circulo que servirá para la bateria
  if (config.enableBattery && config.enableInfoLeft){
  //if(s_last_battery.charge_percent >= 30){
  graphics_context_set_fill_color(ctx, GColorFromHEX(config.batteryCircleColor));
  // }else if(s_last_battery.charge_percent < 30 && s_last_battery.charge_percent >= 20){
  //   graphics_context_set_fill_color(ctx, GColorYellow);
  // }else{
  //    graphics_context_set_fill_color(ctx, GColorRed);

  // } 
  if (DEBUG)
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Battery Val: %i", 36 * s_last_battery.charge_percent);
  graphics_fill_radial(ctx, GRect(23, 29, 39, 39), GOvalScaleModeFitCircle, 3, 0, DEG_TO_TRIGANGLE(36 * (s_last_battery.charge_percent/10)));
  }else{
    graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
    graphics_fill_radial(ctx, GRect(83, 29, 39, 39), GOvalScaleModeFitCircle, 3, 0, DEG_TO_TRIGANGLE(360));
  }
//  graphics_fill_radial(ctx, GRect(24, 30, 37, 37), GOvalScaleModeFitCircle, 3, 0, DEG_TO_TRIGANGLE(36 * (s_last_battery.charge_percent/10)));
  GRect full_bounds = layer_get_bounds(s_window_layer);
  GRect final_unobstructed_screen_area = layer_get_unobstructed_bounds(s_window_layer);
  if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
      change_layers(true);
  }else{
    change_layers(false);
  }

}
/*
  Este procedimiento dibuja la parte de los segundos (background)
 */
static void bg_update_seconds_proc(Layer *layer, GContext *ctx) {  
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(layer);  
  GPoint center = grect_center_point(&unobstructed_bounds);
  
   GPoint center_seconds = (GPoint) {
    .x = center.x,
    .y = center.y+SECONDS_CENTER_OFFSET_Y,
  };

   GPoint center_info_left = (GPoint) {
    .x = center.x - SECONDS_CENTER_OFFSET_X,
    .y = center.y - SECONDS_CENTER_OFFSET_Y,
  };
   GPoint center_info_right = (GPoint) {
    .x = center.x + SECONDS_CENTER_OFFSET_X,
    .y = center.y - SECONDS_CENTER_OFFSET_Y,
  };

   
    
      if (config.enableSeconds){
        graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
        graphics_fill_circle(ctx, center_seconds,CONFIG_RADIUS_SECS_CIRCLE+3);
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.secondsBackColor));
        graphics_fill_circle(ctx, center_seconds,CONFIG_RADIUS_SECS_CIRCLE-1);
        graphics_draw_circle(ctx,center_seconds,CONFIG_RADIUS_SECS_CIRCLE);
       
      }

     if (config.enableInfoLeft){
        graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
        graphics_fill_circle(ctx,center_info_left,CONFIG_RADIUS_INFOLEFT_CIRCLE+3);
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoLeftBackColor));
        graphics_fill_circle(ctx,center_info_left,CONFIG_RADIUS_INFOLEFT_CIRCLE-1);
        graphics_draw_circle(ctx,center_info_left,CONFIG_RADIUS_INFOLEFT_CIRCLE);
      }
      if(config.enableInfoRight){
        graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
        graphics_fill_circle(ctx,center_info_right,CONFIG_RADIUS_INFORIGHT_CIRCLE+3);
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoRightBackColor));
        graphics_fill_circle(ctx,center_info_right,CONFIG_RADIUS_INFORIGHT_CIRCLE-1);
        graphics_draw_circle(ctx,center_info_right,CONFIG_RADIUS_INFORIGHT_CIRCLE);
      }
   
      // Marks for circles

    if (config.enableInfoLeft){
       for(int h = 0; h < 7; h++) { 


            GPoint point = (GPoint) {
              .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 7) * 20 / TRIG_MAX_RATIO) + center_info_left.x,
              .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 7) * 20 / TRIG_MAX_RATIO) + center_info_left.y,
            };

            GPoint point02 = (GPoint) {
              .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 7) * 22 / TRIG_MAX_RATIO) + center_info_left.x,
              .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 7) * 22 / TRIG_MAX_RATIO) + center_info_left.y,
            };

           
            graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
            graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoCirclesColor));
                    
          for(int y = 0; y < 1; y++) {
            for(int x = 0; x < 1; x++) {          
              graphics_draw_line(ctx, GPoint(point.x , point.y ), GPoint(point02.x+x, point02.y+y));
            }
          }
      
    }
    }


    if (config.enableInfoRight){
     for(int h = 0; h < 12; h++) { 


            GPoint point = (GPoint) {
              .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * 20 / TRIG_MAX_RATIO) + center_info_right.x,
              .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * 20 / TRIG_MAX_RATIO) + center_info_right.y,
            };

            GPoint point02 = (GPoint) {
              .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * 22 / TRIG_MAX_RATIO) + center_info_right.x,
              .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * 22 / TRIG_MAX_RATIO) + center_info_right.y,
            };

           
            graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
            graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoCirclesColor));
                    
            for(int y = 0; y < 1; y++) {
              for(int x = 0; x < 1; x++) {       
                graphics_draw_line(ctx, GPoint(point.x +x, point.y +x), GPoint(point02.x+x, point02.y+x));
              }
            }
          
      
    }
    }

    if (config.enableSeconds){
      for(int h = 0; h < 12; h++) { 
            if(h == 0 || h == 3 || h==6 || h==9){
              GPoint point = (GPoint) {
                .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * 20 / TRIG_MAX_RATIO) + center_seconds.x,
                .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * 20 / TRIG_MAX_RATIO) + center_seconds.y,
              };

              GPoint point02 = (GPoint) {
                .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * 22 / TRIG_MAX_RATIO) + center_seconds.x,
                .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * 22 / TRIG_MAX_RATIO) + center_seconds.y,
              };

             
              graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
              graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoCirclesColor));
                      
            for(int y = 0; y < THICKNESS_SECONDS; y++) {
              for(int x = 0; x < THICKNESS_SECONDS; x++) {       
                graphics_draw_line(ctx, GPoint(point.x +x, point.y +y), GPoint(point02.x+x, point02.y+y));
              }
            }
            }
        
      }
    }


    // Cuadros del dia de mes
    
    if (config.enableSeconds){
      graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.secondsBackColor));
    }else{
      graphics_context_set_stroke_color(ctx, GColorFromHEX(config.hourMarkersColor));
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
    }  
    graphics_draw_rect(ctx, GRect(64, center_seconds.y+5, 17, 15));  
    graphics_fill_rect(ctx, GRect(63, center_seconds.y+10, 18,5), 0, GCornersAll);  
    graphics_fill_rect(ctx, GRect(69, center_seconds.y+5, 6,16), 0, GCornersAll);  

    // Dibujar el circulo de los dias de la semana

// 42, 48  ---- 23 radius
// Dibujar el circulo que servirá para la bateria


}

/*
  Este procedimiento dibuja el fondo de la watchface
 */
static void bg_update_proc(Layer *layer, GContext *ctx) {
 
  GRect bounds = layer_get_bounds(layer);  
  GPoint center = grect_center_point(&bounds);


  // Marcas 
  if ( config.enableMinutesMarks ){
    for(int m = 0; m < 60; m++) { 
      GPoint point = (GPoint) {
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * m / 60) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.x,
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * m / 60) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.y,
          };

          GPoint point02 = (GPoint) {
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * m / 60) * 75 / TRIG_MAX_RATIO) + center.x,
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * m / 60) * 75 / TRIG_MAX_RATIO) + center.y,
          };

         
              
          graphics_context_set_stroke_color(ctx, GColorFromHEX(config.minuteMarkersColor));
          graphics_context_set_fill_color(ctx, GColorFromHEX(config.minuteMarkersColor));
          graphics_draw_line(ctx, GPoint(point02.x , point02.y ), GPoint(point.x , point.y));
              
    }
    
    // Delete long lines
    graphics_context_set_stroke_color(ctx, GColorFromHEX(config.backgroundcolor));
    graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
    graphics_fill_rect(ctx, GRect(2, 2, 140,164), 1, GCornersAll);  

  }

  if ( config.enableHourMarks ){
    for(int h = 0; h < 12; h++) { 



          GPoint point = (GPoint) {
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.x,
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.y,
          };

          GPoint point02 = (GPoint) {
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * h / 12) * getMarkSize(h) / TRIG_MAX_RATIO) + center.x,
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * h / 12) * getMarkSize(h) / TRIG_MAX_RATIO) + center.y,
          };

          if (h == 0 || h == 3 || h ==6 || h == 9){
              
            graphics_context_set_stroke_color(ctx, GColorFromHEX(config.hourMarkersColor));
            graphics_context_set_fill_color(ctx, GColorFromHEX(config.hourMarkersColor));
              
          }else{
              
            graphics_context_set_stroke_color(ctx, GColorFromHEX(config.hourMarkersColor));
            graphics_context_set_fill_color(ctx, GColorFromHEX(config.hourMarkersColor));
             
          }
                
          for(int y = 0; y < THICKNESSMARKS; y++) {
            for(int x = 0; x < THICKNESSMARKS; x++) {
              graphics_draw_line(ctx, GPoint(point02.x + x, point02.y + y), GPoint(point.x + x, point.y + y));
            }
          }
      
    }
  }




}


/*
  Este procedimiento dibuja las manecillas del reloj y todo lo que cambie cada TICK TIME

 */
static void draw_proc(Layer *layer, GContext *ctx) {

    GRect bounds = layer_get_bounds(layer);
    GRect unobstructed_bounds = layer_get_unobstructed_bounds(layer);
    GPoint center = grect_center_point(&unobstructed_bounds);
    GPoint center_seconds = (GPoint) {
      .x = center.x,
      .y = center.y+SECONDS_CENTER_OFFSET_Y,
    };

     GPoint center_info_left = (GPoint) {
      .x = center.x - SECONDS_CENTER_OFFSET_X,
      .y = center.y - SECONDS_CENTER_OFFSET_Y,
    };
     GPoint center_info_right = (GPoint) {
      .x = center.x + SECONDS_CENTER_OFFSET_X,
      .y = center.y - SECONDS_CENTER_OFFSET_Y,
    };

    GPoint infoleft_hand_long = make_hand_point(s_last_time.wday, 7, CONFIG_HAND_LENGTH_SEC-3, center_info_left);
    GPoint infoleft_hand_inverted = make_hand_point(inverse_hand(s_last_time.wday), 7, 10, center_info_left);
    GPoint inforight_hand_long = make_hand_point(s_last_time.month+1, 12, CONFIG_HAND_LENGTH_SEC-3, center_info_right);
    GPoint inforight_hand_inverted = make_hand_point(inverse_hand(s_last_time.month+1), 12, 10, center_info_right);


    Time now = s_last_time;

    GPoint second_hand_long = make_hand_point(now.seconds, 60, CONFIG_HAND_LENGTH_SEC, center_seconds);
    GPoint second_hand_inverted = make_hand_point(inverse_hand(now.seconds), 60, 10, center_seconds);
    float minute_angle = TRIG_MAX_ANGLE * now.minutes / 60; //now.minutes
    float hour_angle = TRIG_MAX_ANGLE * now.hours / 12; //now.hours
    hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);
    float hour_angle_inverse = TRIG_MAX_ANGLE * inverse_hand_hour(now.hours) / 12; //now.hours
    hour_angle_inverse += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);



    if(config.enableSeconds){
        // Dibujar los segundos
        
          graphics_context_set_stroke_color(ctx, GColorFromHEX(config.smallHandsColor));
          graphics_context_set_fill_color(ctx, GColorFromHEX(config.smallHandsColor));
        

          // SEGUNDOS
          for(int y = 0; y < THICKNESS_SECONDS; y++) {
            for(int x = 0; x < THICKNESS_SECONDS; x++) {       
              graphics_draw_line(ctx, GPoint(center_seconds.x + x, center_seconds.y+y ), GPoint(second_hand_long.x + x, second_hand_long.y+y ));
              if (CONFIG_SECONDS_HAND_INVERSED)
              graphics_draw_line(ctx, GPoint(center_seconds.x + x , center_seconds.y+y ), GPoint(second_hand_inverted.x+x, second_hand_inverted.y+y ));
            }
          }

           // El centro de los segundos
          
        graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoCirclesColor));         

        graphics_fill_circle(ctx, GPoint(center_seconds.x , center_seconds.y ), 2);
      }
      
        graphics_context_set_stroke_color(ctx, GColorFromHEX(config.smallHandsColor));
        graphics_context_set_fill_color(ctx, GColorFromHEX(config.smallHandsColor));
     
      if (config.enableInfoLeft){
       // INFO DEL DIA
        for(int y = 0; y < THICKNESS_SECONDS; y++) {
          for(int x = 0; x < THICKNESS_SECONDS; x++) {       
            graphics_draw_line(ctx, GPoint(center_info_left.x + x, center_info_left.y+y ), GPoint(infoleft_hand_long.x + x, infoleft_hand_long.y+y ));
            if (CONFIG_INFOLEFT_HAND_INVERSED)
            graphics_draw_line(ctx, GPoint(center_info_left.x + x , center_info_left.y+y ), GPoint(infoleft_hand_inverted.x+x, infoleft_hand_inverted.y+y ));
          }
        }      
       }
       if (config.enableInfoRight){
        // INFO DEL MES
        for(int y = 0; y < THICKNESS_SECONDS; y++) {
          for(int x = 0; x < THICKNESS_SECONDS; x++) {       
            graphics_draw_line(ctx, GPoint(center_info_right.x + x, center_info_right.y+y ), GPoint(inforight_hand_long.x + x, inforight_hand_long.y+y ));
            if (CONFIG_INFORIGHT_HAND_INVERSED)
              graphics_draw_line(ctx, GPoint(center_info_right.x + x , center_info_right.y+y ), GPoint(inforight_hand_inverted.x+x, inforight_hand_inverted.y+y ));
          }
        }     
     }

    graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoCirclesColor));
    graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoCirclesColor));         
 
    if (config.enableInfoLeft){
      graphics_fill_circle(ctx, GPoint(center_info_left.x , center_info_left.y ), 2);
    }
    if(config.enableInfoRight){
      graphics_fill_circle(ctx, GPoint(center_info_right.x , center_info_right.y ), 2);
    }
     

   if (config.enableSeconds){
    graphics_context_set_stroke_color(ctx, GColorFromHEX(config.secondsBackColor));
    graphics_context_set_fill_color(ctx, GColorFromHEX(config.secondsBackColor));    
    graphics_fill_circle(ctx, GPoint(center_seconds.x , center_seconds.y ), 1);
   }
   if (config.enableInfoLeft){
      graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoLeftBackColor));
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoLeftBackColor));    
      graphics_fill_circle(ctx, GPoint(center_info_left.x , center_info_left.y ), 1);
    }
    if(config.enableInfoRight){
      graphics_context_set_stroke_color(ctx, GColorFromHEX(config.infoRightBackColor));
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.infoRightBackColor));    
      graphics_fill_circle(ctx, GPoint(center_info_right.x , center_info_right.y ), 1);
    }




  if (DRAW_PATH_HAND){
    // Move paths depends on the minutes or hours
    if (s_last_time.minutes == 0 ||  s_last_time.minutes == 15 || s_last_time.minutes == 30 || s_last_time.minutes == 45){
      gpath_move_to(s_minute_hand_path_bold_ptr, GPoint(200, 200));
      gpath_move_to(s_minute_hand_path_ptr, center);
      if(s_last_time.minutes == 0){
        gpath_move_to(s_hour_hand_path_bold_ptr, GPoint(200, 200));
        gpath_move_to(s_hour_hand_path_ptr, center);
      }else{
        gpath_move_to(s_hour_hand_path_bold_ptr, center);
        gpath_move_to(s_hour_hand_path_ptr, GPoint(200, 200));
      }
    }else{
      gpath_move_to(s_hour_hand_path_bold_ptr, center);
      gpath_move_to(s_minute_hand_path_bold_ptr, center);
      gpath_move_to(s_hour_hand_path_ptr, GPoint(200, 200));
      gpath_move_to(s_minute_hand_path_ptr, GPoint(200, 200));
    }
      // Draw Hours Hand
    gpath_rotate_to(s_hour_hand_path_ptr,hour_angle);
    gpath_rotate_to(s_minute_hand_path_ptr,minute_angle);

    gpath_rotate_to(s_hour_hand_path_bold_ptr,hour_angle);
    gpath_rotate_to(s_minute_hand_path_bold_ptr,minute_angle);
        
    graphics_context_set_fill_color(ctx, GColorFromHEX(config.hourHandsColor));
    gpath_draw_filled(ctx, s_hour_hand_path_ptr);
    gpath_draw_filled(ctx, s_hour_hand_path_bold_ptr);
    graphics_context_set_stroke_color(ctx, GColorFromHEX(config.backgroundcolor));    
    gpath_draw_outline(ctx, s_hour_hand_path_ptr);  
    gpath_draw_outline(ctx, s_hour_hand_path_bold_ptr);  
  

    graphics_context_set_fill_color(ctx, GColorFromHEX(config.minuteHandsColor));
    gpath_draw_filled(ctx, s_minute_hand_path_ptr);
    gpath_draw_filled(ctx, s_minute_hand_path_bold_ptr);
    graphics_context_set_stroke_color(ctx, GColorFromHEX(config.backgroundcolor));    
    gpath_draw_outline(ctx, s_minute_hand_path_ptr);  
    gpath_draw_outline(ctx, s_minute_hand_path_bold_ptr);  




  }else{

    // En caso que no se utilice un path para las manecillas

   

   /**
    * 
    * Dibujar las horas    
    */
   
   GPoint hour_hand_long = (GPoint) {
      .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)CONFIG_HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)CONFIG_HAND_LENGTH_HOUR / TRIG_MAX_RATIO) + center.y,
    };

    GPoint hour_hand_inverse = (GPoint) {
      .x = (int16_t)(sin_lookup(hour_angle_inverse) * (int32_t)(8) / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(hour_angle_inverse) * (int32_t)(8) / TRIG_MAX_RATIO) + center.y,
    };
   
      graphics_context_set_stroke_color(ctx, GColorFromHEX(config.hourHandsColor));
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.hourHandsColor));
    
    for(int y = 0; y < THICKNESS_HOUR; y++) {
      for(int x = 0; x < THICKNESS_HOUR; x++) {
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_long.x + x, hour_hand_long.y + y));
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_inverse.x + x, hour_hand_inverse.y + y));
      }
    }

      /**
      * Dibujar los minutos
      */
    
      graphics_context_set_stroke_color(ctx, GColorFromHEX(config.minuteHandsColor));
      graphics_context_set_fill_color(ctx, GColorFromHEX(config.minuteHandsColor));
   

    GPoint minute_hand_long = make_hand_point(now.minutes, 60, CONFIG_HAND_LENGTH_MIN, center);    
    GPoint minute_hand_inverse = make_hand_point(inverse_hand(now.minutes), 60, 10, center);
    


    for(int y = 0; y < THICKNESS_MINUTES; y++) {
      for(int x = 0; x < THICKNESS_MINUTES; x++) {
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_inverse.x + x, minute_hand_inverse.y + y));
      }
    }
    
  }


  // CIRCULO DE LAS MANECILLAS CENTRALES

   
  graphics_context_set_stroke_color(ctx, GColorFromHEX(config.backgroundcolor));
  graphics_context_set_fill_color(ctx, GColorFromHEX(config.hourMarkersColor));
  
  graphics_draw_circle(ctx, GPoint(center.x + CENTER_HANDS_OFFSET, center.y + CENTER_HANDS_OFFSET), CENTER_HANDS_RADIUS);
  graphics_fill_circle(ctx, GPoint(center.x+ CENTER_HANDS_OFFSET , center.y + CENTER_HANDS_OFFSET), CENTER_HANDS_RADIUS-1);

 
  graphics_context_set_fill_color(ctx, GColorFromHEX(config.backgroundcolor));
 
  graphics_fill_circle(ctx, GPoint(center.x + CENTER_HANDS_OFFSET, center.y + CENTER_HANDS_OFFSET ), 1);

  
  
  if (!grect_equal(&bounds, &unobstructed_bounds)) {
    change_layers(true);
  }else{
    change_layers(false);
  }


}


static void get_weather(){
  if (DEBUG)
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
      if (DEBUG)
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
    }
  } else {
    // The outbox cannot be used right now
    if (DEBUG)
      APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }

}

static void tick_handler(struct tm *tick_time, TimeUnits changed) {
  
  s_last_time.days = tick_time->tm_mday;
  s_last_time.hours = tick_time->tm_hour;
  s_last_time.minutes = tick_time->tm_min;
  s_last_time.seconds = tick_time->tm_sec;
  s_last_time.wday = tick_time->tm_wday;
  s_last_time.month = tick_time->tm_mon;
  if (config.enableInfoLeft)
  strftime(s_weekday_buffer, sizeof(s_weekday_buffer), "%a", tick_time);
  else
    snprintf(s_weekday_buffer, sizeof(s_weekday_buffer), "%s", "  ");

  snprintf(s_day_in_month_buffer, sizeof(s_day_in_month_buffer), "%d", s_last_time.days);
  if(config.enableInfoRight)
    strftime(s_month_buffer, sizeof(s_month_buffer), "%b", tick_time);  
  else
    snprintf(s_month_buffer, sizeof(s_month_buffer), "%s", "  ");  

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?"%H:%M" : "%I:%M", tick_time);
  text_layer_set_text(s_digital_time_layer, s_buffer);
  layer_mark_dirty(s_canvas_layer);

  unsigned int now = mktime(tick_time);
  if (now > last_time_weather + SECONDS_FOR_POLL ){
    if (DEBUG)
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Calling weather");
    last_time_weather = mktime(tick_time);
   // get_weather();
  }
  if (DEBUG)
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Time :%u", last_time_weather);

}

static void window_load(Window *window) {
  s_connection_icons_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CONNECTION_ICONS_14));

  window_set_background_color(s_main_window, GColorFromHEX(config.backgroundcolor));

  Layer *window_layer = window_get_root_layer(window);
  s_window_layer = window_layer;

   UnobstructedAreaHandlers handlers = {
    .will_change = prv_unobstructed_will_change,
    .did_change = prv_unobstructed_did_change
  };
  unobstructed_area_service_subscribe(handlers, NULL);


  GRect bounds = layer_get_bounds(window_layer);
  GRect unobstructed_bounds = layer_get_unobstructed_bounds(window_layer);
  
  GPoint center = grect_center_point(&unobstructed_bounds);
  GPoint center_normal = grect_center_point(&bounds);

  GPoint center_seconds = (GPoint) {
    .x = center.x,
    .y = center.y+SECONDS_CENTER_OFFSET_Y,
  };

   GPoint center_info_left = (GPoint) {
    .x = center.x - SECONDS_CENTER_OFFSET_X,
    .y = center.y - SECONDS_CENTER_OFFSET_Y,
  };
   GPoint center_info_right = (GPoint) {
    .x = center.x + SECONDS_CENTER_OFFSET_X,
    .y = center.y - SECONDS_CENTER_OFFSET_Y,
  };

  // Create Background Layer
  s_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_bg_layer);

  s_battery_layer = layer_create(bounds);
  layer_set_update_proc(s_battery_layer, battery_layer_update);
  s_health_layer = layer_create(bounds);
  layer_set_update_proc(s_health_layer, health_layer_update);


 // DIA DEL MES
  s_day_in_month_layer = text_layer_create(GRect(50, center_normal.y+SECONDS_CENTER_OFFSET_Y+3, 44, 40));
  text_layer_set_text_alignment(s_day_in_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_day_in_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_day_in_month_layer, GColorFromHEX(config.dayInMonthcolor));
  text_layer_set_background_color(s_day_in_month_layer, GColorClear);
  //text_layer_set_text(s_day_in_month_layer, "31");  


  // DIA DE LA SEMANA
  if (SHOWINFOCIRCLES){
    s_weekday_layer = text_layer_create(GRect(20, 30, 44, 40));
  }else{
    s_weekday_layer = text_layer_create(GRect(40, 35, 44, 40));
  }
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_weekday_layer, GColorFromHEX(config.infoCirclesColor));
  text_layer_set_background_color(s_weekday_layer, GColorClear);

 
  // MES
  if (SHOWINFOCIRCLES){
    s_month_layer = text_layer_create(GRect(80, 30, 44, 40));
  }else{
    s_month_layer = text_layer_create(GRect(60, 35, 44, 40));
  }
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_color(s_month_layer, GColorFromHEX(config.infoCirclesColor)); 
  text_layer_set_background_color(s_month_layer, GColorClear);

  // DIGITAL TIME
  s_digital_time_layer = text_layer_create(GRect(CONFIG_X_START_INFO_BOX, 110, 44, 40));
  text_layer_set_text_alignment(s_digital_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_digital_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_color(s_digital_time_layer, GColorFromHEX(config.infoCirclesColor)); 
  text_layer_set_background_color(s_digital_time_layer, GColorClear);

  // CAPAS DE LAS HORAS
  s_12_hour_layer = text_layer_create(GRect(XPOS_12H, YPOS_12H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_12_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_12_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_12_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_12_hour_layer, GColorClear);
  text_layer_set_text(s_12_hour_layer, "12");  

  s_01_hour_layer = text_layer_create(GRect(XPOS_01H, YPOS_01H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_01_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_01_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_01_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_01_hour_layer, GColorClear);
  text_layer_set_text(s_01_hour_layer, "1");  

  s_02_hour_layer = text_layer_create(GRect(XPOS_02H, YPOS_02H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_02_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_02_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_02_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_02_hour_layer, GColorClear);
  text_layer_set_text(s_02_hour_layer, "2");  

  s_03_hour_layer = text_layer_create(GRect(XPOS_03H, YPOS_03H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_03_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_03_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_03_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_03_hour_layer, GColorClear);
  text_layer_set_text(s_03_hour_layer, "3");  

  s_04_hour_layer = text_layer_create(GRect(XPOS_04H, YPOS_04H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_04_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_04_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_04_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_04_hour_layer, GColorClear);
  text_layer_set_text(s_04_hour_layer, "4");  

  s_05_hour_layer = text_layer_create(GRect(XPOS_05H, YPOS_05H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_05_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_05_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_05_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_05_hour_layer, GColorClear);
  text_layer_set_text(s_05_hour_layer, "5");  

  s_06_hour_layer = text_layer_create(GRect(XPOS_06H, YPOS_06H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_06_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_06_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_06_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_06_hour_layer, GColorClear);
  text_layer_set_text(s_06_hour_layer, "6");  

  s_07_hour_layer = text_layer_create(GRect(XPOS_07H, YPOS_07H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_07_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_07_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_07_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_07_hour_layer, GColorClear);
  text_layer_set_text(s_07_hour_layer, "7");  

  s_08_hour_layer = text_layer_create(GRect(XPOS_08H, YPOS_08H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_08_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_08_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_08_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_08_hour_layer, GColorClear);
  text_layer_set_text(s_08_hour_layer, "8");  

  s_09_hour_layer = text_layer_create(GRect(XPOS_09H, YPOS_09H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_09_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_09_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_09_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_09_hour_layer, GColorClear);
  text_layer_set_text(s_09_hour_layer, "9");  

  s_10_hour_layer = text_layer_create(GRect(XPOS_10H, YPOS_10H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_10_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_10_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_10_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_10_hour_layer, GColorClear);
  text_layer_set_text(s_10_hour_layer, "10");  

  s_11_hour_layer = text_layer_create(GRect(XPOS_11H, YPOS_11H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_11_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_11_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_11_hour_layer, GColorFromHEX(config.numbersColor));
  text_layer_set_background_color(s_11_hour_layer, GColorClear);
  text_layer_set_text(s_11_hour_layer, "11");  



  s_connection_layer = text_layer_create(GRect(50, center_normal.y+SECONDS_CENTER_OFFSET_Y-17, 44, 40));
  text_layer_set_text_alignment(s_connection_layer, GTextAlignmentCenter);
  text_layer_set_font(s_connection_layer, s_connection_icons_14);
  text_layer_set_text_color(s_connection_layer, GColorFromHEX(config.dayInMonthcolor));
  text_layer_set_background_color(s_connection_layer, GColorClear);
  text_layer_set_text(s_connection_layer, "");

  


  s_seconds_layer = layer_create(bounds);
  layer_set_update_proc(s_seconds_layer, bg_update_seconds_proc);

  // s_temperature_layer = text_layer_create(GRect(CONFIG_X_START_INFO_BOX, 44, 44, 40));
  // text_layer_set_text_alignment(s_temperature_layer, GTextAlignmentCenter);
  // text_layer_set_font(s_temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  // if (CONFIG_INVERTED){
  //   text_layer_set_text_color(s_temperature_layer, GColorBlack);
  // }else{
  //   text_layer_set_text_color(s_temperature_layer, GColorWhite);
  // }  
  // text_layer_set_background_color(s_temperature_layer, GColorClear);


// TODO: Change sizes
  s_background_layer = layer_create(bounds);
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

  layer_add_child(window_layer, text_layer_get_layer(s_12_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_01_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_02_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_03_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_04_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_05_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_06_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_07_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_08_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_09_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_10_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_11_hour_layer));
  layer_add_child(window_layer, s_seconds_layer);
  layer_add_child(window_layer, text_layer_get_layer(s_day_in_month_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_month_layer));
  layer_add_child(window_layer, s_battery_layer);
  layer_add_child(window_layer, s_health_layer);
  layer_add_child(window_layer, text_layer_get_layer(s_connection_layer));


  //layer_add_child(window_layer, text_layer_get_layer(s_digital_time_layer));
  //layer_add_child(window_layer, text_layer_get_layer(s_temperature_layer));
  layer_add_child(window_layer, s_canvas_layer);
  

   s_hour_hand_path_bold_ptr = gpath_create(&HOUR_HAND_PATH_BOLD);
   s_minute_hand_path_bold_ptr = gpath_create(&MINUTE_HAND_PATH_BOLD);
   s_hour_hand_path_ptr = gpath_create(&HOUR_HAND_PATH);
   s_minute_hand_path_ptr = gpath_create(&MINUTE_HAND_PATH);
  
  // Translate by (5, 5):
  gpath_move_to(s_hour_hand_path_bold_ptr, center);
  gpath_move_to(s_minute_hand_path_bold_ptr, center);
  gpath_move_to(s_hour_hand_path_ptr, center);
  gpath_move_to(s_minute_hand_path_ptr, center);

}


static void window_unload(Window *window) {
  layer_destroy(s_background_layer);
  layer_destroy(s_bg_layer);
  layer_destroy(s_canvas_layer);
  text_layer_destroy(s_weekday_layer);
  text_layer_destroy(s_day_in_month_layer);
  text_layer_destroy(s_month_layer);
  text_layer_destroy(s_digital_time_layer);
  text_layer_destroy(s_12_hour_layer);
  text_layer_destroy(s_01_hour_layer);
  text_layer_destroy(s_02_hour_layer);
  text_layer_destroy(s_03_hour_layer);
  text_layer_destroy(s_04_hour_layer);
  text_layer_destroy(s_05_hour_layer);
  text_layer_destroy(s_06_hour_layer);
  text_layer_destroy(s_07_hour_layer);
  text_layer_destroy(s_08_hour_layer);
  text_layer_destroy(s_09_hour_layer);
  text_layer_destroy(s_10_hour_layer);
  text_layer_destroy(s_11_hour_layer);
  layer_destroy(s_seconds_layer);
  layer_destroy(s_battery_layer);
  layer_destroy(s_health_layer);
  text_layer_destroy(s_connection_layer);
 // text_layer_destroy(s_temperature_layer);
  gpath_destroy(s_hour_hand_path_ptr);
  gpath_destroy(s_minute_hand_path_ptr);
  gpath_destroy(s_hour_hand_path_bold_ptr);
  gpath_destroy(s_minute_hand_path_bold_ptr);

  tick_timer_service_unsubscribe();
  if (config.enableBattery)
    battery_state_service_unsubscribe();
  if (config.enableHealth)
    health_service_events_unsubscribe();


  connection_service_unsubscribe();

 

}

static void health_handler(HealthEventType event, void *context) {
  // Which type of event occurred?
  switch(event) {
    case HealthEventSignificantUpdate:    
      update_health_data();
      layer_mark_dirty(s_health_layer);
      if(DEBUG)
        APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventSignificantUpdate event");
      break;
    case HealthEventMovementUpdate:
      update_health_data();
      layer_mark_dirty(s_health_layer);
      if(DEBUG)
        APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventMovementUpdate event");
      break;
    case HealthEventSleepUpdate:
      if(DEBUG)
        APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventSleepUpdate event");
      break;
      case HealthEventMetricAlert:
      if(DEBUG)
        APP_LOG(APP_LOG_LEVEL_INFO, 
              "New HealthService HealthEventMetricAlert event");
      break;
    case HealthEventHeartRateUpdate:
      if(DEBUG)
        APP_LOG(APP_LOG_LEVEL_INFO,
              "New HealthService HealthEventHeartRateUpdate event");
      break;
  }
}


void health_init() {
    #if defined(PBL_HEALTH)
    // Attempt to subscribe 
    if(!health_service_events_subscribe(health_handler, NULL)) {
      APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    }
    #else
    APP_LOG(APP_LOG_LEVEL_ERROR, "Health not available!");
    #endif
      
}

static void read_configuration(){

  if (persist_exists(MESSAGE_KEY_enableSeconds)){
    config.enableSeconds = persist_read_bool(MESSAGE_KEY_enableSeconds);
  }else{
    config.enableSeconds = true;
  }

  if (persist_exists(MESSAGE_KEY_enableBattery)){
    config.enableBattery = persist_read_bool(MESSAGE_KEY_enableBattery);
  }else{
    config.enableBattery = true;
  }

  if (persist_exists(MESSAGE_KEY_enableInfoLeft)){
    config.enableInfoLeft = persist_read_bool(MESSAGE_KEY_enableInfoLeft);
  }else{
    config.enableInfoLeft = true;
  }

  if (persist_exists(MESSAGE_KEY_enableInfoRight)){
    config.enableInfoRight = persist_read_bool(MESSAGE_KEY_enableInfoRight);
  }else{
    config.enableInfoRight = true;
  }

  if (persist_exists(MESSAGE_KEY_enableHealth)){
    config.enableHealth = persist_read_bool(MESSAGE_KEY_enableHealth);
  }else{
    config.enableHealth = true;
  }

  if (persist_exists(MESSAGE_KEY_enableHourMarks)){
    config.enableHourMarks= persist_read_bool(MESSAGE_KEY_enableHourMarks);
  }else{
    config.enableHourMarks = true;
  }

  if (persist_exists(MESSAGE_KEY_enableMinutesMarks)){
    config.enableMinutesMarks = persist_read_bool(MESSAGE_KEY_enableMinutesMarks);
  }else{
    config.enableMinutesMarks = true;
  }

   if (persist_exists(MESSAGE_KEY_enableConnection)){
    config.enableConnection = persist_read_bool(MESSAGE_KEY_enableConnection);
  }else{
    config.enableConnection = false;
  }


  if (persist_exists(MESSAGE_KEY_backgroundcolor)){
    config.backgroundcolor = persist_read_int(MESSAGE_KEY_backgroundcolor);
  }else{
    config.backgroundcolor = 0;
  }

  if (persist_exists(MESSAGE_KEY_hourHandsColor)){
    config.hourHandsColor = persist_read_int(MESSAGE_KEY_hourHandsColor);
  }else{
    config.hourHandsColor = 11184810;
  }


  if (persist_exists(MESSAGE_KEY_minuteHandsColor)){
    config.minuteHandsColor = persist_read_int(MESSAGE_KEY_minuteHandsColor);
  }else{
    config.minuteHandsColor = 16777215;
  }


  if (persist_exists(MESSAGE_KEY_smallHandsColor)){
    config.smallHandsColor = persist_read_int(MESSAGE_KEY_smallHandsColor);
  }else{
    config.smallHandsColor = 43775;
  }

  if (persist_exists(MESSAGE_KEY_hourMarkersColor)){
    config.hourMarkersColor = persist_read_int(MESSAGE_KEY_hourMarkersColor);
  }else{
    config.hourMarkersColor = 16777215;
  }

  if (persist_exists(MESSAGE_KEY_minuteMarkersColor)){
    config.minuteMarkersColor = persist_read_int(MESSAGE_KEY_minuteMarkersColor);
  }else{
    config.minuteMarkersColor = 16777215;
  }

  if (persist_exists(MESSAGE_KEY_numbersColor)){
    config.numbersColor = persist_read_int(MESSAGE_KEY_numbersColor);
  }else{
    config.numbersColor = 11184810;
  }

  if (persist_exists(MESSAGE_KEY_batteryCircleColor)){
    config.batteryCircleColor = persist_read_int(MESSAGE_KEY_batteryCircleColor);
  }else{
    config.batteryCircleColor = 43775;
  }

  if (persist_exists(MESSAGE_KEY_healthCircleColor)){
    config.healthCircleColor = persist_read_int(MESSAGE_KEY_healthCircleColor);
  }else{
    config.healthCircleColor = 43775;
  }

  if (persist_exists(MESSAGE_KEY_infoCirclesColor)){
    config.infoCirclesColor = persist_read_int(MESSAGE_KEY_infoCirclesColor);
  }else{
    config.infoCirclesColor = 16777215;
  }

  if (persist_exists(MESSAGE_KEY_infoLeftBackColor)){
    config.infoLeftBackColor = persist_read_int(MESSAGE_KEY_infoLeftBackColor);
  }else{
    config.infoLeftBackColor = 0;
  }
  if (persist_exists(MESSAGE_KEY_infoRightBackColor)){
    config.infoRightBackColor = persist_read_int(MESSAGE_KEY_infoRightBackColor);
  }else{
    config.infoRightBackColor = 0;
  }
  if (persist_exists(MESSAGE_KEY_secondsBackColor)){
    config.secondsBackColor = persist_read_int(MESSAGE_KEY_secondsBackColor);
  }else{
    config.secondsBackColor = 0;
  }
  if (persist_exists(MESSAGE_KEY_dayInMonthcolor)){
    config.dayInMonthcolor = persist_read_int(MESSAGE_KEY_dayInMonthcolor);
  }else{
    config.dayInMonthcolor = 11184810;
  }



  // Dspues de leer, configurar adecuadamente las suscipciones
  tick_timer_service_subscribe(config.enableSeconds ? SECOND_UNIT : MINUTE_UNIT, tick_handler);


  if (config.enableBattery && config.enableInfoLeft){
    battery_state_service_subscribe(handle_battery);
    //layer_set_hidden(s_battery_layer, false);

  }else{
    battery_state_service_unsubscribe();
    //layer_set_hidden(s_battery_layer, true);
  }

 
  // Inicializa el servicio de Salud
  if (config.enableHealth && config.enableInfoRight){
    health_init();
    //layer_set_hidden(s_health_layer, true);
   }else{
    health_service_events_unsubscribe();
    //layer_set_hidden(s_health_layer, false);

   } 

   if(config.enableConnection){
       connection_service_subscribe((ConnectionHandlers) {
      .pebble_app_connection_handler = app_connection_handler,
      .pebblekit_connection_handler = kit_connection_handler
    });
   }else{
    connection_service_unsubscribe();
   }

   if(DEBUG){
     APP_LOG(APP_LOG_LEVEL_DEBUG, "infoRightBackColor: %d",config.infoRightBackColor);
     APP_LOG(APP_LOG_LEVEL_DEBUG, "infoLeftBackColor: %d",config.infoLeftBackColor);
   }

}



static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // // Read color preferences
  // Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  // if(bg_color_t) {
  //   GColor bg_color = GColorFromHEX(bg_color_t->value->int32);
  // }

  // Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
  // if(fg_color_t) {
  //   GColor fg_color = GColorFromHEX(fg_color_t->value->int32);
  // }

  // // Read boolean preferences
  // Tuple *second_tick_t = dict_find(iter, MESSAGE_KEY_SecondTick);
  // if(second_tick_t) {
  //   bool second_ticks = second_tick_t->value->int32 == 1;
  // }

  // Tuple *animations_t = dict_find(iter, MESSAGE_KEY_Animations);
  // if(animations_t) {
  //   bool animations = animations_t->value->int32 == 1;
  // }
  Tuple *configOption = dict_find(iter, MESSAGE_KEY_enableSeconds);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableSeconds,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_enableBattery);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableBattery,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_enableInfoLeft);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableInfoLeft,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_enableInfoRight);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableInfoRight,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_enableHealth);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableHealth,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_enableHourMarks);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableHourMarks,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_enableMinutesMarks);
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableMinutesMarks,configOption->value->int32 == 1);
  }
  if(configOption){
    persist_write_bool(MESSAGE_KEY_enableConnection,configOption->value->int32 == 1);
  }
  configOption = dict_find(iter, MESSAGE_KEY_backgroundcolor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_backgroundcolor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_hourHandsColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_hourHandsColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_minuteHandsColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_minuteHandsColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_smallHandsColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_smallHandsColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_hourMarkersColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_hourMarkersColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_minuteMarkersColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_minuteMarkersColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_numbersColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_numbersColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_batteryCircleColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_batteryCircleColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_healthCircleColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_healthCircleColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_infoCirclesColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_infoCirclesColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_infoLeftBackColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_infoLeftBackColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_infoRightBackColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_infoRightBackColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_secondsBackColor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_secondsBackColor,configOption->value->int32);
  }
  configOption = dict_find(iter, MESSAGE_KEY_dayInMonthcolor);
  if(configOption){
    persist_write_int(MESSAGE_KEY_dayInMonthcolor,configOption->value->int32);
  }
  

  read_configuration();
  refreshAllLayers();

}





static void init() {
  read_configuration();
  //tick_timer_service_subscribe(config.enableSeconds ? SECOND_UNIT : MINUTE_UNIT, tick_handler);
  


  
  s_main_window = window_create();
 // s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_BW_IMAGE);

  //Aplite
  
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = app_connection_handler,
    .pebblekit_connection_handler = kit_connection_handler
  });
  
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_main_window, true);

  time_t t = time(NULL);
  struct tm *tm_now = localtime(&t);
  tick_handler(tm_now, SECOND_UNIT);
    if (SUBSCRIBE_TO_BATTERY)

  handle_battery(battery_state_service_peek());

  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);



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
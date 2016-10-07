#include <pebble.h>

#define CONFIG_INVERTED 0
#define CONFIG_CENTER_SECONDS_X 72
#define CONFIG_CENTER_SECONDS_Y 120
#define CONFIG_HAND_LENGTH_SEC 22
#define CONFIG_RADIUS_SECS_CIRCLE 23
#define CONFIG_HAND_LENGTH_HOUR 45
#define CONFIG_HAND_LENGTH_MIN 65
#define THICKNESS_MINUTES 4
#define THICKNESS_HOUR 4
#define THICKNESSMARKS 5
#define THICKNESS_SECONDS 2
#define DRAW_PATH_HAND 0
#define CONFIG_X_START_INFO_BOX 100

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
#define HOUR_MARKERS_COLOR GColorLightGray
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
static Layer *s_bg_layer,  *s_canvas_layer, *s_forecast_layer, *s_seconds_layer;
static TextLayer *s_12_hour_layer, *s_01_hour_layer, *s_02_hour_layer, *s_03_hour_layer, *s_04_hour_layer, *s_05_hour_layer, *s_06_hour_layer, *s_07_hour_layer, *s_08_hour_layer, *s_09_hour_layer, *s_10_hour_layer, *s_11_hour_layer;
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
  Este procedimiento dibuja la parte de los segundos (background)
 */
static void bg_update_seconds_proc(Layer *layer, GContext *ctx) {
   GPoint center_seconds = (GPoint) {
    .x = (int16_t)CONFIG_CENTER_SECONDS_X,
    .y = (int16_t)CONFIG_CENTER_SECONDS_Y,
  };

   GPoint center_info_left = (GPoint) {
    .x = (int16_t)CONFIG_CENTER_INFOLEFT_X,
    .y = (int16_t)CONFIG_CENTER_INFOLEFT_Y,
  };
   GPoint center_info_right = (GPoint) {
    .x = (int16_t)CONFIG_CENTER_INFORIGHT_X,
    .y = (int16_t)CONFIG_CENTER_INFORIGHT_Y,
  };

    #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorBlack);
  #elif defined(PBL_BW)
    if (CONFIG_INVERTED){
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    }
  #endif
    
    if (CIRCLE_SECONDS_AREA){
      graphics_fill_circle(ctx, center_seconds,CONFIG_RADIUS_SECS_CIRCLE+3);
      graphics_draw_circle(ctx,center_seconds,CONFIG_RADIUS_SECS_CIRCLE);
      graphics_fill_circle(ctx,center_info_left,CONFIG_RADIUS_INFOLEFT_CIRCLE+3);
      graphics_draw_circle(ctx,center_info_left,CONFIG_RADIUS_INFOLEFT_CIRCLE);
      graphics_fill_circle(ctx,center_info_right,CONFIG_RADIUS_INFORIGHT_CIRCLE+3);
      graphics_draw_circle(ctx,center_info_right,CONFIG_RADIUS_INFORIGHT_CIRCLE);

    }else{
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_rect(ctx, GRect(49, 102, 48,48), 2, GCornersAll);  
      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_fill_rect(ctx, GRect(50, 103, 46,46), 0, GCornersAll);  
      graphics_fill_rect(ctx, GRect(49, 150, 48,2), 0, GCornersAll);  

     // graphics_draw_rect(ctx, GRect(49, 102, 48, 48));  
      //graphics_fill_rect(ctx, GRect(63, 135, 18,5), 2, GCornersAll);  
    }

      graphics_context_set_fill_color(ctx, GColorBlack);

    graphics_draw_rect(ctx, GRect(65, CONFIG_CENTER_SECONDS_Y+6, 14, 13));  
    graphics_fill_rect(ctx, GRect(63, CONFIG_CENTER_SECONDS_Y+10, 18,5), 0, GCornersAll);  
    graphics_fill_rect(ctx, GRect(69, CONFIG_CENTER_SECONDS_Y+5, 6,14), 0, GCornersAll);  


// 42, 48  ---- 23 radius
// Dibujar el circulo que servirá para la bateria
  //graphics_context_set_fill_color(ctx, GColorGreen);
  //graphics_fill_radial(ctx, GRect(21, 27, 43, 43), GOvalScaleModeFitCircle, 3, 0, DEG_TO_TRIGANGLE(360));


}

/*
  Este procedimiento dibuja el fondo de la watchface
 */
static void bg_update_proc(Layer *layer, GContext *ctx) {
 
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);


  // Marcas 
  if ( SHOW_MINUTES_MARKS ){
    for(int m = 0; m < 60; m++) { 
      GPoint point = (GPoint) {
            //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
            //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * m / 60) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.x,
            //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * m / 60) * (int32_t)(3 * (CONFIG_HAND_LENGTH_MIN)) / TRIG_MAX_RATIO) + center.y,
          };

          GPoint point02 = (GPoint) {
            //int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;
            //secondHand.x = (sin_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.x;
            .x = (int16_t)(sin_lookup(TRIG_MAX_ANGLE * m / 60) * 75 / TRIG_MAX_RATIO) + center.x,
            //secondHand.y = (-cos_lookup(second_angle) * secondHandLength / TRIG_MAX_RATIO) + center.y;
            .y = (int16_t)(-cos_lookup(TRIG_MAX_ANGLE * m / 60) * 75 / TRIG_MAX_RATIO) + center.y,
          };

         
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
              
                
              
                  graphics_draw_line(ctx, GPoint(point02.x , point02.y ), GPoint(point.x , point.y));
              
    }
    
    // Delete long lines
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(2, 2, 140,164), 1, GCornersAll);  

  }

  if ( SHOW_HOUR_MARKS ){
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
              }
                
              for(int y = 0; y < THICKNESSMARKS; y++) {
                for(int x = 0; x < THICKNESSMARKS; x++) {
                  graphics_draw_line(ctx, GPoint(point02.x + x, point02.y + y), GPoint(point.x + x, point.y + y));
                }
              }
      
    }
  }




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
      .x = (int16_t)CONFIG_CENTER_SECONDS_X,
      .y = (int16_t)CONFIG_CENTER_SECONDS_Y,
    };
    GPoint center_infoleft = (GPoint) {
      .x = (int16_t)CONFIG_CENTER_INFOLEFT_X,
      .y = (int16_t)CONFIG_CENTER_INFOLEFT_Y,
    };
    GPoint center_inforight = (GPoint) {
      .x = (int16_t)CONFIG_CENTER_INFORIGHT_X,
      .y = (int16_t)CONFIG_CENTER_INFORIGHT_Y,
    };

    GPoint infoleft_hand_long = make_hand_point(0, 60, CONFIG_HAND_LENGTH_SEC, center_infoleft);
    GPoint infoleft_hand_inverted = make_hand_point(inverse_hand(0), 60, 10, center_infoleft);
    GPoint inforight_hand_long = make_hand_point(0, 60, CONFIG_HAND_LENGTH_SEC, center_inforight);
    GPoint inforight_hand_inverted = make_hand_point(inverse_hand(0), 60, 10, center_inforight);


    Time now = s_last_time;

    GPoint second_hand_long = make_hand_point(now.seconds, 60, CONFIG_HAND_LENGTH_SEC, center_seconds);
    GPoint second_hand_inverted = make_hand_point(inverse_hand(now.seconds), 60, 10, center_seconds);
    float minute_angle = TRIG_MAX_ANGLE * now.minutes / 60; //now.minutes
    float hour_angle = TRIG_MAX_ANGLE * now.hours / 12; //now.hours
    hour_angle += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);
    float hour_angle_inverse = TRIG_MAX_ANGLE * inverse_hand_hour(now.hours) / 12; //now.hours
    hour_angle_inverse += (minute_angle / TRIG_MAX_ANGLE) * (TRIG_MAX_ANGLE / 12);

  if (DRAW_PATH_HAND){
      // Draw Hours Hand
    gpath_rotate_to(s_hour_hand_path_ptr,hour_angle);
    gpath_rotate_to(s_minute_hand_path_ptr,minute_angle);
    
    
    graphics_context_set_fill_color(ctx, GColorLightGray);
  
    gpath_draw_filled(ctx, s_hour_hand_path_ptr);
  
    graphics_context_set_fill_color(ctx, GColorWhite);
  
    gpath_draw_filled(ctx, s_minute_hand_path_ptr);

  
    graphics_context_set_fill_color(ctx, GColorBlack);
    
    gpath_draw_outline(ctx, s_hour_hand_path_ptr);  
    gpath_draw_outline(ctx, s_minute_hand_path_ptr);  

  

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
    #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorLightGray);
      graphics_context_set_fill_color(ctx, GColorLightGray);
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
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(hour_hand_inverse.x + x, hour_hand_inverse.y + y));
      }
    }

/**
      * Dibujar los minutos
      */
    if (CONFIG_INVERTED){
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorBlack);
    }else{
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    }

    GPoint minute_hand_long = make_hand_point(now.minutes, 60, CONFIG_HAND_LENGTH_MIN, center);    
    GPoint minute_hand_inverse = make_hand_point(inverse_hand(now.minutes), 60, 10, center);
    


    for(int y = 0; y < THICKNESS_MINUTES; y++) {
      for(int x = 0; x < THICKNESS_MINUTES; x++) {
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_long.x + x, minute_hand_long.y + y));
        graphics_draw_line(ctx, GPoint(center.x + x, center.y + y), GPoint(minute_hand_inverse.x + x, minute_hand_inverse.y + y));
      }
    }
    
  }


    // Dibujar los segundos
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

      // SEGUNDOS
      for(int y = 0; y < THICKNESS_SECONDS; y++) {
        for(int x = 0; x < THICKNESS_SECONDS; x++) {       
          graphics_draw_line(ctx, GPoint(center_seconds.x + x, center_seconds.y+y ), GPoint(second_hand_long.x + x, second_hand_long.y+y ));
          if (CONFIG_SECONDS_HAND_INVERSED)
          graphics_draw_line(ctx, GPoint(center_seconds.x + x , center_seconds.y+y ), GPoint(second_hand_inverted.x+x, second_hand_inverted.y+y ));
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
     // INFO DEL DIA
      for(int y = 0; y < THICKNESS_SECONDS; y++) {
        for(int x = 0; x < THICKNESS_SECONDS; x++) {       
          graphics_draw_line(ctx, GPoint(center_infoleft.x + x, center_infoleft.y+y ), GPoint(infoleft_hand_long.x + x, infoleft_hand_long.y+y ));
          if (CONFIG_INFOLEFT_HAND_INVERSED)
          graphics_draw_line(ctx, GPoint(center_infoleft.x + x , center_infoleft.y+y ), GPoint(infoleft_hand_inverted.x+x, infoleft_hand_inverted.y+y ));
        }
      }      

      // INFO DEL DIA
      for(int y = 0; y < THICKNESS_SECONDS; y++) {
        for(int x = 0; x < THICKNESS_SECONDS; x++) {       
          graphics_draw_line(ctx, GPoint(center_inforight.x + x, center_inforight.y+y ), GPoint(inforight_hand_long.x + x, inforight_hand_long.y+y ));
          if (CONFIG_INFORIGHT_HAND_INVERSED)
            graphics_draw_line(ctx, GPoint(center_inforight.x + x , center_inforight.y+y ), GPoint(inforight_hand_inverted.x+x, inforight_hand_inverted.y+y ));
        }
      }     

      // El centro de los segundos


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

    graphics_fill_circle(ctx, GPoint(center_seconds.x , center_seconds.y ), 2);
    graphics_fill_circle(ctx, GPoint(center_infoleft.x , center_infoleft.y ), 2);
    graphics_fill_circle(ctx, GPoint(center_inforight.x , center_inforight.y ), 2);
    
     #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorBlack);
    #elif defined(PBL_BW)
      if (CONFIG_INVERTED){
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_fill_color(ctx, GColorWhite);
      }else{
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorBlack);
      }
    #endif
    graphics_fill_circle(ctx, GPoint(center_seconds.x , center_seconds.y ), 1);
    graphics_fill_circle(ctx, GPoint(center_infoleft.x , center_infoleft.y ), 1);
    graphics_fill_circle(ctx, GPoint(center_inforight.x , center_inforight.y ), 1);

  // CIRCULO DE LAS MANECILLAS CENTRALES

    #if defined(PBL_COLOR)
      graphics_context_set_stroke_color(ctx, GColorBlack);
      graphics_context_set_fill_color(ctx, GColorWhite);
    #elif defined(PBL_BW)        
      if (CONFIG_INVERTED){
        graphics_context_set_stroke_color(ctx, GColorWhite);
        graphics_context_set_fill_color(ctx, GColorBlack);
      }else{
        graphics_context_set_stroke_color(ctx, GColorBlack);
        graphics_context_set_fill_color(ctx, GColorWhite);
      }
    #endif

  graphics_draw_circle(ctx, GPoint(center.x + 2, center.y + 2), 5);
  graphics_fill_circle(ctx, GPoint(center.x + 2, center.y + 2), 4);

   if (CONFIG_INVERTED){
    graphics_context_set_fill_color(ctx, GColorWhite);
  }else{
    graphics_context_set_fill_color(ctx, GColorBlack);
  }
  graphics_fill_circle(ctx, GPoint(center.x + 2, center.y + 2), 1);

  



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

 // DIA DEL MES
  s_day_in_month_layer = text_layer_create(GRect(50, CONFIG_CENTER_SECONDS_Y+3, 44, 40));
  text_layer_set_text_alignment(s_day_in_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_day_in_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_day_in_month_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_day_in_month_layer, GColorLightGray);
  }  
  text_layer_set_background_color(s_day_in_month_layer, GColorClear);
  //text_layer_set_text(s_day_in_month_layer, "31");  


  // DIA DE LA SEMANA
  s_weekday_layer = text_layer_create(GRect(100, 78, 44, 40));
  text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
  text_layer_set_font(s_weekday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_weekday_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_weekday_layer, GColorWhite);
  }
  text_layer_set_background_color(s_weekday_layer, GColorClear);

 
  // MES
  s_month_layer = text_layer_create(GRect(100, 94, 44, 40));
  text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
  text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_month_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_month_layer, GColorWhite);
  } 
  text_layer_set_background_color(s_month_layer, GColorClear);

  // DIGITAL TIME
  s_digital_time_layer = text_layer_create(GRect(CONFIG_X_START_INFO_BOX, 110, 44, 40));
  text_layer_set_text_alignment(s_digital_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_digital_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  if (CONFIG_INVERTED){
    text_layer_set_text_color(s_digital_time_layer, GColorBlack);
  }else{
    text_layer_set_text_color(s_digital_time_layer, GColorWhite);
  } 
  text_layer_set_background_color(s_digital_time_layer, GColorClear);

  // CAPAS DE LAS HORAS
  s_12_hour_layer = text_layer_create(GRect(XPOS_12H, YPOS_12H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_12_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_12_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_12_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_12_hour_layer, GColorClear);
  text_layer_set_text(s_12_hour_layer, "12");  

  s_01_hour_layer = text_layer_create(GRect(XPOS_01H, YPOS_01H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_01_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_01_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_01_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_01_hour_layer, GColorClear);
  text_layer_set_text(s_01_hour_layer, "1");  

  s_02_hour_layer = text_layer_create(GRect(XPOS_02H, YPOS_02H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_02_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_02_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_02_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_02_hour_layer, GColorClear);
  text_layer_set_text(s_02_hour_layer, "2");  

  s_03_hour_layer = text_layer_create(GRect(XPOS_03H, YPOS_03H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_03_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_03_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_03_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_03_hour_layer, GColorClear);
  text_layer_set_text(s_03_hour_layer, "3");  

  s_04_hour_layer = text_layer_create(GRect(XPOS_04H, YPOS_04H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_04_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_04_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_04_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_04_hour_layer, GColorClear);
  text_layer_set_text(s_04_hour_layer, "4");  

  s_05_hour_layer = text_layer_create(GRect(XPOS_05H, YPOS_05H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_05_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_05_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_05_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_05_hour_layer, GColorClear);
  text_layer_set_text(s_05_hour_layer, "5");  

  s_06_hour_layer = text_layer_create(GRect(XPOS_06H, YPOS_06H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_06_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_06_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_06_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_06_hour_layer, GColorClear);
  text_layer_set_text(s_06_hour_layer, "6");  

  s_07_hour_layer = text_layer_create(GRect(XPOS_07H, YPOS_07H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_07_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_07_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_07_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_07_hour_layer, GColorClear);
  text_layer_set_text(s_07_hour_layer, "7");  

  s_08_hour_layer = text_layer_create(GRect(XPOS_08H, YPOS_08H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_08_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_08_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_08_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_08_hour_layer, GColorClear);
  text_layer_set_text(s_08_hour_layer, "8");  

  s_09_hour_layer = text_layer_create(GRect(XPOS_09H, YPOS_09H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_09_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_09_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_09_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_09_hour_layer, GColorClear);
  text_layer_set_text(s_09_hour_layer, "9");  

  s_10_hour_layer = text_layer_create(GRect(XPOS_10H, YPOS_10H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_10_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_10_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_10_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_10_hour_layer, GColorClear);
  text_layer_set_text(s_10_hour_layer, "10");  

  s_11_hour_layer = text_layer_create(GRect(XPOS_11H, YPOS_11H, XLENHOURS, YLENHOURS));
  text_layer_set_text_alignment(s_11_hour_layer, GTextAlignmentCenter);
  text_layer_set_font(s_11_hour_layer, fonts_get_system_font(TIME_NUMERALS_FONT));
  text_layer_set_text_color(s_11_hour_layer, HOUR_MARKERS_COLOR);
  text_layer_set_background_color(s_11_hour_layer, GColorClear);
  text_layer_set_text(s_11_hour_layer, "11");  


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
 // layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
 // layer_add_child(window_layer, text_layer_get_layer(s_month_layer));

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
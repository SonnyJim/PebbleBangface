#include <pebble.h>
#include "src/c/bangface.h"

Window *my_window;

static GBitmap *s_bg_bitmap;
static GBitmap *s_hour_bitmap;
static GBitmap *s_head_bitmap;

static GRect s_hour_rect;

static BitmapLayer *s_bg_layer;
static BitmapLayer *s_head_layer;

static Layer *s_hour_layer;
static Layer *s_mins_layer;

int hours, minutes, hours_gmt, dst_gmt, month, day;
int foo;

GPoint hour_pts[12];
GPoint min_pts[60];

static void init_gpoints (void)
{
  hour_pts[0] = GPoint((144 / 2) - 14, 0); 
  hour_pts[1] = GPoint(96,0);
  hour_pts[2] = GPoint(114,36);
  hour_pts[3] = GPoint(115, 70);
  hour_pts[4] = GPoint(113, 103);
  hour_pts[5] = GPoint(96, 139);
  hour_pts[6] = GPoint(58, 138);
  hour_pts[7] = GPoint(16, 138);
  hour_pts[8] = GPoint(0, 104);
  hour_pts[9] = GPoint(0, 70);
  hour_pts[10] = GPoint(0, 34);
  hour_pts[11] = GPoint(19,0);
  
  min_pts[0] = GPoint(72,0);
  min_pts[1] = GPoint(83,0);
  min_pts[2] = GPoint(91,0);
  min_pts[3] = GPoint(101,0);
  min_pts[4] = GPoint(111,0);
  min_pts[5] = GPoint(120,0);
  min_pts[6] = GPoint(137,0);
  min_pts[7] = GPoint(143,7);
  min_pts[8] = GPoint(143,25);
  min_pts[9] = GPoint(143, 35);
  
  min_pts[10] = GPoint(143,43);
  min_pts[11] = GPoint(143,56);
  min_pts[12] = GPoint(143,64);
  min_pts[13] = GPoint(143,71);
  min_pts[14] = GPoint(143,78);
  min_pts[15] = GPoint(143, 84);
  min_pts[16] = GPoint(143, 89);
  min_pts[17] = GPoint(143, 96);
  min_pts[18] = GPoint(143, 104);
  min_pts[19] = GPoint(143, 112);

  min_pts[20] = GPoint(143,124);
  min_pts[21] = GPoint(143,133);
  min_pts[22] = GPoint(143, 143);
  min_pts[23] = GPoint(143, 160);
  min_pts[24] = GPoint(139,168);
  min_pts[25] = GPoint(119, 168);
  min_pts[26] = GPoint(111, 168);
  min_pts[27] = GPoint(101,168);
  min_pts[28] = GPoint(91, 168);
  min_pts[29] = GPoint(82, 168);
  
  min_pts[30] = GPoint(72,168);
  min_pts[31] = GPoint(61, 168);
  min_pts[32] = GPoint(51,168);
  min_pts[33] = GPoint(42,168);
  min_pts[34] = GPoint(33,168);
  min_pts[35] = GPoint(24,168);
  min_pts[36] = GPoint(5,168);
  min_pts[37] = GPoint(0,160);
  min_pts[38] = GPoint(0,143);
  min_pts[39] = GPoint(0,132);
  min_pts[40] = GPoint(0,124);
  
  min_pts[41] = GPoint(0,112);
  min_pts[42] = GPoint(0,104);
  min_pts[43] = GPoint(0,96);
  min_pts[44] = GPoint(0,89);
  min_pts[45] = GPoint(0,84);
  min_pts[46] = GPoint(0,78);
  min_pts[47] = GPoint(0,71);
  min_pts[48] = GPoint(0,63);
  min_pts[49] = GPoint(0,56);
    
  min_pts[50] = GPoint(0,43);
  min_pts[51] = GPoint(0,35);
  min_pts[52] = GPoint(0,24);
  min_pts[53] = GPoint(0,7);
  min_pts[54] = GPoint(6,0);
  min_pts[55] = GPoint(25,0);
  min_pts[56] = GPoint(33,0);
  min_pts[57] = GPoint(44,0);
  min_pts[58] = GPoint(52,0);
  min_pts[59] = GPoint(61,0);
}

static int time_convert (int hours)
{
  if (hours >= 12)
    hours -= 12;
  
  return hours;
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *now_tm = localtime(&temp);
  struct tm *gmt_tm = gmtime(&temp);

  //Extract the hours and minutes
  hours = now_tm->tm_hour;
  minutes = now_tm->tm_min;
  hours_gmt = gmt_tm->tm_hour;
  dst_gmt = gmt_tm->tm_isdst;
  
  //Month is from 0 - 11, duuh
  month = now_tm->tm_mon + 1;
  day = now_tm->tm_mday;
  
  
  //Only show 12hr time format
  hours = time_convert (hours);
  hours_gmt = time_convert (hours_gmt);
}

void mins_update_proc(Layer *layer, GContext *ctx) 
{
  graphics_context_set_stroke_color(ctx, GColorBlack);
  
  int i;
  
  for (i = 0; i < 60; i ++)
  {
    if (i % 15 == 0)
      graphics_context_set_stroke_width(ctx, 5);
    else if (i % 5 == 0)
       graphics_context_set_stroke_width(ctx,3);
    else
      graphics_context_set_stroke_width(ctx, 1);
   
      
    graphics_draw_line(ctx, GPoint(144 / 2, 166/2), min_pts[i]);
  }
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 11);
  graphics_draw_line(ctx, GPoint(144 / 2, 166/2), min_pts[minutes]);
  
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, GPoint(144 / 2, 166/2), min_pts[minutes]);
  
}

void hour_update_proc(Layer *layer, GContext *ctx) 
{
  s_hour_rect.origin = hour_pts[hours];
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_hour_bitmap, s_hour_rect);
}

static void layer_create_hour (Layer *window_layer)
{
  // Load the image data
  s_hour_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BF_SMILEY);
  s_hour_layer = layer_create(layer_get_bounds(window_layer));
  //Get the smiley face size;
  s_hour_rect = gbitmap_get_bounds(s_hour_bitmap);
  layer_set_update_proc(s_hour_layer, hour_update_proc);
  layer_add_child(window_layer, s_hour_layer);
}

static void layer_create_bg (Layer *window_layer)
{
  s_bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BF_BG);
  s_bg_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_compositing_mode(s_bg_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_bg_layer, s_bg_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bg_layer));
}

static void layer_create_head (Layer *window_layer)
{
  s_head_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMG_BF_FACE);
  s_head_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_compositing_mode(s_head_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_head_layer, s_head_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_head_layer));
}

static void layer_create_mins (Layer *window_layer)
{
  s_mins_layer = layer_create(layer_get_bounds(window_layer));
  layer_set_update_proc(s_mins_layer, mins_update_proc);
  layer_add_child(window_layer, s_mins_layer);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);  
  window_stack_push(my_window, true);

  init_gpoints ();
  //layer_create_bg (window_layer);  
  layer_create_mins (window_layer);

  
  layer_create_hour (window_layer); 
  layer_create_head (window_layer);
}

static void main_window_unload(Window *window) {
  gbitmap_destroy(s_bg_bitmap);
  gbitmap_destroy(s_hour_bitmap);
  gbitmap_destroy(s_hour_bitmap);
  
  bitmap_layer_destroy(s_bg_layer);
  layer_destroy(s_hour_layer);
  layer_destroy(s_mins_layer);
  bitmap_layer_destroy(s_head_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time ();
  layer_mark_dirty (s_hour_layer);
  layer_mark_dirty (s_mins_layer);  
  /*
  rot_bitmap_layer_increment_angle(s_face_layer, DEG_TO_TRIGANGLE (360 / 60));
  layer_mark_dirty ((Layer *)s_face_layer);
  */
}

void handle_init(void) {
  my_window = window_create();
   window_set_window_handlers(my_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  window_stack_push(my_window, true);
}

void handle_deinit(void) {
  window_destroy(my_window);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

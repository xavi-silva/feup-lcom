// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include "graphics.h"
#include "../lab3/keyboard.c"
#include "../lab2/timer.c"
#include "vbe.h"

extern uint16_t h_res;	   
extern uint16_t v_res;
extern int counter;
extern uint8_t scancode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g2/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g2/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int wait_for_ESC(){
  if(wait_for_key(ESC_RELEASE) != 0)return 1;
  return 0;
}
int(video_test_init)(uint16_t mode, uint8_t delay) {
  if(graph_set_graphic_mode(mode) != 0)return 1;
  sleep(delay);
  vg_exit();
  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  if(map_vram(mode) != 0)return 1;
  if(graph_set_graphic_mode(mode) != 0)return 1;
  if (vg_draw_rectangle(x, y, width, height, color) != 0) return 1;
  if(wait_for_ESC() != 0)return 1;
  vg_exit();
  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if(map_vram(mode) != 0)return 1;
  if(graph_set_graphic_mode(mode) != 0)return 1;
  vg_draw_matrix(no_rectangles, first, step);
  if(wait_for_ESC() != 0)return 1;
  vg_exit();
  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if(map_vram(GRAPHIC_1024x768_INDEXED) != 0)return 1;
  if(graph_set_graphic_mode(GRAPHIC_1024x768_INDEXED) != 0)return 1;
  uint8_t* map; //pixmap itself (2D uint8_t array)
  xpm_image_t img; //pixmap and metadata (width, height, size, type)
  map = xpm_load(xpm, XPM_INDEXED, &img);
  vg_draw_pixmap(map,&img,x, y);
  if(wait_for_ESC() != 0)return 1;
  vg_exit();
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {                  
  if(map_vram(GRAPHIC_1024x768_INDEXED) != 0)return 1;
  if(graph_set_graphic_mode(GRAPHIC_1024x768_INDEXED) != 0)return 1;
  uint8_t* map;
  xpm_image_t img; 
  map = xpm_load(xpm, XPM_INDEXED, &img);

  vg_draw_pixmap(map,&img,xi, yi);
  int16_t x_movement = (xi == xf) ? 0 : ((xf > xi) ? 1 : -1);
  //If speed < 0, the sprite only moves one pixel at a time
  x_movement = (speed > 0) ? x_movement * speed : x_movement;
  int16_t y_movement = (yi == yf) ? 0 : ((yf > yi) ? 1 : -1);
  y_movement = (speed > 0) ? y_movement * speed : y_movement;
  
  int ipc_status;
  message msg;
  uint8_t timer_irq_set, kbd_irq_set;
  int r;
  kbd_subscribe_int(&kbd_irq_set);
  timer_subscribe_int(&timer_irq_set);

  uint16_t wait_frames = -speed;
  while(scancode != ESC_RELEASE){
    if((r = driver_receive(ANY, &msg, &ipc_status)) != 0){ 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if(is_ipc_notify(ipc_status)){
      switch(_ENDPOINT_P(msg.m_source)){
        case HARDWARE:{
          if(msg.m_notify.interrupts & kbd_irq_set){
            kbc_ih();
          }
          if(msg.m_notify.interrupts & timer_irq_set){
            timer_int_handler();
            if((xi < xf || yi < yf) && (counter % (60 / fr_rate) == 0)){
              
              if(speed < 0){
                wait_frames--;
                if(wait_frames == 0){
                  vg_erase_pixmap(&img, xi, yi);
                  wait_frames = -speed;
                  xi = xi + x_movement;
                  yi = yi + y_movement;
                  vg_draw_pixmap(map,&img,xi, yi);
                }
              }
              else if(speed > 0){
                vg_erase_pixmap(&img, xi, yi);
                xi = xi + x_movement;
                yi = yi + y_movement;
                if(xi > xf) xi = xf;
                if(yi > yf) yi = yf;
                vg_draw_pixmap(map,&img,xi, yi);
              }
            }
          }
          break;
        }
      }
    }
  }
  vg_exit();
  if (timer_unsubscribe_int() != 0) return 1;
  if (kbd_unsubscribe_int() != 0) return 1;
  return 0;
}



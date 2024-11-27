#ifndef _LCOM_GRAPHICS_H_
#define _LCOM_GRAPHICS_H_

#include <lcom/lcf.h>

int graph_set_graphic_mode(uint16_t mode);
int graph_set_text_mode(uint16_t mode);
int map_vram(uint16_t mode);
int (vg_draw_pixel)(uint16_t, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_matrix)(uint8_t no_rectangles, uint32_t first, uint8_t step);

int(vg_draw_pixmap)(uint32_t* map, xpm_image_t* img, uint16_t x, uint16_t y);

//int (vg_erase_pixmap)(sprite* pixmap);

int graph_init();
int graph_end();
int graph_swap_video_mem();
uint32_t bit_range(uint8_t low, uint8_t high);

int graph_get_v_res();

int graph_get_h_res();

int graph_draw_background();

int graph_get_current_buf(); //For debbuging at the moment

#endif

#include "graphics.h"
#include "vbe.h"

static char* video_mems[3] = {NULL, NULL, NULL}; /*Triple Buffering;  Process (virtual) address to which VRAM is mapped */
static uint8_t current_video_mem = 0;
static vbe_mode_info_t mode_info;


uint32_t bit_range(uint8_t low, uint8_t size){
  uint32_t result = 0;
  for(int i = low; i < low + size; i++){
    result |= BIT(i);
  }
  return result;
}


int graph_set_graphic_mode(uint16_t mode){
    reg86_t r86;
    memset(&r86, 0, sizeof(r86));
    r86.ah = AH_FUNC;
    r86.al = AL_SET_MODE;
    r86.bx = mode | BX_LINEAR_MAP;
    r86.intno = ITNO_VBE;
    if( sys_int86(&r86) != 0) {
    printf("\tvg_exit(): sys_int86() failed \n");
    return 1;
    }
    return 0;
}

int map_vram(uint16_t mode){
    struct minix_mem_range mr;

    //Returns information on the input VBE mode, including screen dimensions, color depth and VRAM physical address. 
    vbe_get_mode_info(mode, &mode_info);  

    unsigned int vram_base = mode_info.PhysBasePtr;  /* VRAM's physical addresss */
    unsigned int vram_size = mode_info.YResolution * mode_info.BytesPerScanLine;  /* VRAM's size, but you can use
                        the frame-buffer size, instead */
    int r;				    
   
    /* Allow memory mapping */

    mr.mr_base = (phys_bytes) vram_base;	
    mr.mr_limit = mr.mr_base + 3 * vram_size;  

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        printf("sys_privctl (ADD_MEM) failed: %d\n", r);
        return 1;
    }

    /* Map memory for Triple Buffers*/
    for(uint8_t i = 0; i < 3; i++){
        video_mems[i] = vm_map_phys(SELF, (void *) (phys_bytes) (vram_size * i + mr.mr_base), vram_size);
        if(video_mems[i] == MAP_FAILED){
            printf("Error mapping video memory\n");
            return 1;
        }
        else{
            memset(video_mems[i], 0, vram_size);
        }
    }
    current_video_mem = 1;
    return 0;
}
int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
    if(color == TRANSPARENCY_COLOR_8_8_8_8)return 0;
    uint16_t h_res = mode_info.XResolution;
    uint16_t v_res = mode_info.YResolution;
    uint16_t bytes_per_pixel = mode_info.BytesPerScanLine / mode_info.XResolution;
    
    if(x > h_res || y > v_res)return 1;
    char* pos = video_mems[current_video_mem] + (y * h_res * bytes_per_pixel) + (x * bytes_per_pixel);
    memcpy(pos, &color, bytes_per_pixel);
    //printf("Drawing pixel with coordinates (%u, %u)\n", x, y);
    return 0;
}
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    for(unsigned i = x; i < x + len; i++){
        if(vg_draw_pixel(i, y, color) != 0){
            printf("Error drawing pixel with coordinates (%u, %u)", i, y);
            return 1;
        }
    }
    return 0;
}
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    //printf("Drawing rectangle  with coordinates (%u, %u) to (%u, %u)\n", x, y, x + width, y + height);
    for(unsigned i = y; i < y + height; i++){
        if(vg_draw_hline(x, i, width, color) != 0){
            return 1;
        }
    }
    return 0;
}

int (vg_draw_matrix)(uint8_t no_rectangles, uint32_t first, uint8_t step){
    uint32_t color;
    uint16_t h_res = mode_info.XResolution;
    uint16_t v_res = mode_info.YResolution;
    uint16_t bytes_per_pixel = mode_info.BytesPerScanLine / mode_info.XResolution;
    
    int rectangle_width = h_res / no_rectangles;
    int rectangle_height = v_res / no_rectangles;
    
    for(unsigned i = 0; i < no_rectangles; i++){
        for(unsigned j = 0; j < no_rectangles; j++){
            //Indexed Color Model is the only mode that uses one byte per pixel
            if(bytes_per_pixel == 1){
                color = (first + (i * no_rectangles + j) * step) % (1 << mode_info.BitsPerPixel);
            }
            else{
                //red_first will store the red of the colour of the first rectangle
                uint8_t red_first = (first & (bit_range(mode_info.RedFieldPosition, mode_info.RedMaskSize))) >> mode_info.RedFieldPosition;
                uint8_t green_first = (first & (bit_range(mode_info.GreenFieldPosition, mode_info.GreenMaskSize))) >> mode_info.GreenFieldPosition;
                uint8_t blue_first = (first & (bit_range(mode_info.BlueFieldPosition, mode_info.BlueMaskSize))) >> mode_info.BlueFieldPosition;
                
                uint8_t red = (red_first + j * step) % (1 << mode_info.RedMaskSize);
                uint8_t green = (green_first + i * step) % (1 << mode_info.GreenMaskSize);
                uint8_t blue = (blue_first + (i + j) * step) % (1 << mode_info.BlueMaskSize);
               
                color = (red << mode_info.RedFieldPosition) | (green << mode_info.GreenFieldPosition)  | (blue << mode_info.BlueFieldPosition);
            }
            vg_draw_rectangle(j * rectangle_width, i * rectangle_height, rectangle_width, rectangle_height, color);
        }
    }
    return 0;
}

int (vg_draw_pixmap)(uint32_t* map, xpm_image_t* img, uint16_t x, uint16_t y){
    //printf("Drawing pixmap with coordinates (%u, %u)\n", x, y);
    for(unsigned i = 0; i < img->width; i++){
        for(unsigned j = 0; j < img->height; j++){
            vg_draw_pixel(x + i, y + j, map[i + j * img->width]);
        }
    }
    return 0;
}
/*
int (vg_erase_pixmap)(Sprite* pixmap){
    for(unsigned i = 0; i < pixmap->img.width; i++){
        for(unsigned j = 0; j < pixmap->img.height; j++){
            vg_draw_pixel(pixmap->position.x + i, pixmap->position.y + j, xpm_transparency_color(pixmap->img.type));
        }
    }
    return 0;
}*/

int graph_init(){
    if(map_vram(GRAPHIC_1152x864_DIRECT) != 0)return 1;
    if(graph_set_graphic_mode(GRAPHIC_1152x864_DIRECT) != 0)return 1;
    return 0;
}
int graph_end(){
    for (int i = 0; i < 3; i++) {
        if (video_mems[i] != NULL) {
            free(video_mems[i]); // Free memory of each member if it's not NULL
            video_mems[i] = NULL; // Set the pointer to NULL after freeing
        }
    }
    return vg_exit();
}
int graph_get_v_res(){
    return mode_info.YResolution;
}

int graph_get_h_res(){
    return mode_info.XResolution;
}


int graph_swap_video_mem(){
    reg86_t r86;
    memset(&r86, 0, sizeof(r86));
    r86.ah = AH_FUNC;
    r86.al = AL_SET_DISPLAY_START;
    r86.bl = BL_SET_DISPLAY_START;
    r86.cx = 0; /*First Displayed Pixel In Scan Line*/
    r86.dx = current_video_mem * mode_info.YResolution; /*First Displayed Scan Line*/
    r86.intno = ITNO_VBE;
    if( sys_int86(&r86) != 0) {
        printf("\tvg_exit(): sys_int86() failed \n");
        return 1;
    }
    if((r86.ah != AH_SUCCESSFUL_CALL) || (r86.al != AL_SUCCESSFUL_CALL)){
        printf("Error invoking VBE function. r86.ah : 0x%x  r86.al: 0x%x\n", r86.ah, r86.al);
        return 1;
    }
    current_video_mem = (current_video_mem + 1) % 3;
    return 0;
}

int graph_draw_background(){
   
    vg_draw_rectangle(0, 0, 1152, 864, 0x000000);
    return 0;
}


int graph_get_current_buf(){
    uint16_t v_res = mode_info.YResolution;
    printf("Swapping!\n");
    reg86_t r86;
    memset(&r86, 0, sizeof(r86));
    r86.ax = 0x4F07;
    r86.bx = 0x01;
    r86.intno = 0x10;

    if (sys_int86(&r86) != OK) {
        printf("\tgetCurrentBUffer(): sys_int86() failed \n");
        return -1;
    }

    if (r86.dx == 0) {
        printf("Graph  0\n");
    }
    else if (r86.dx == v_res){
        printf("Graph  1\n");
    }else{
        printf("Graph  2\n");
    }
    printf("Draw  %u\n", current_video_mem);
    return 0;
}

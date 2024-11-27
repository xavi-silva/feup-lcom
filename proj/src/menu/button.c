#include "button.h"

Button* create_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t default_color, uint32_t touched_color) {
    Button* button = (Button*)malloc(sizeof(Button));
    if (button == NULL) {
        return NULL;
    }

    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->default_color = default_color;
    button->touched_color = touched_color;
    button->touched = false;

    return button;
}

void draw_button(Button* button) {
    if(button->touched){
        vg_draw_rectangle(button->x - 2, button->y - 2, button->width + 4, button->height + 4, 0x000000);
        vg_draw_rectangle(button->x, button->y, button->width, button->height,button->touched_color);
    }
    vg_draw_rectangle(button->x, button->y, button->width, button->height, button->touched ? button->touched_color : button->default_color);
}

bool check_collision(Button* button, uint16_t mouse_x, uint16_t mouse_y) {
    return (mouse_x >= button->x && mouse_x <= (button->x + button->width) &&
            mouse_y >= button->y && mouse_y <= (button->y + button->height));
}


void free_button(Button* button) {
    if (button == NULL) {
        return;
    }
    free(button);
}

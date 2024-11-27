#include "menu.h"
#include "assets/font/xpm_numbers.h"
#include "assets/font/xpm_letters.h"

Menu* create_menu(Sprite* background, Sprite* foreground) {
    Menu* menu = (Menu*)malloc(sizeof(Menu));
    if (menu == NULL) {
        return NULL;
    }
    menu->background = background;
    menu->foreground = foreground;
    menu->current_time = "0-0-0";
    menu->num_buttons = 3;
    for(unsigned i = 0; i < menu->num_buttons; i++){
        menu->buttons[i] = create_button(DEFAULT_BUTTON_X, DEFAULT_BUTTON_Y + i * DEFAULT_BUTTON_Y_MARGIN, DEFAULT_BUTTON_WIDTH, DEFAULT_BUTTON_HEIGHT, DEFAULT_BUTTON_COLOR, DEFAULT_TOUCHED_BUTTON_COLOR);
    }
    return menu;
}

void add_button(Menu* menu, Button* button) {
    if (menu->num_buttons < MAX_BUTTONS) {
        menu->buttons[menu->num_buttons++] = button;
    }
}

void draw_menu(Menu* menu) {
    draw_sprite(menu->background);
    for (unsigned i = 0; i < menu->num_buttons; i++) {
        draw_button(menu->buttons[i]);
    }
    draw_sprite(menu->foreground);
    draw_time(menu);
}

void set_current_time(Menu* menu, char* current_time){
    menu->current_time = current_time;
}
void free_menu(Menu* menu) {
    if (menu == NULL) {
        return;
    }
    free_sprite(menu->background);
    free_sprite(menu->foreground);
    for (unsigned i = 0; i < menu->num_buttons; i++) {
        free_button(menu->buttons[i]);
    }
    free(menu);
}

void draw_time(Menu* menu) {
    vg_draw_rectangle(DEFAULT_TIME_X,DEFAULT_TIME_Y, DEFAULT_TIME_WIDTH, DEFAULT_TIME_HEIGHT,DEFAULT_BUTTON_COLOR);
    size_t len = strlen(menu->current_time);
    uint16_t x = DEFAULT_TIME_X + 20;
    uint16_t y = DEFAULT_TIME_Y - 7;
    for (size_t i = 0; i < len; i++) {
        char current_char = menu->current_time[i];
        const char** xpm = NULL;
        if (current_char >= '0' && current_char <= '9') {
            xpm = numbers_xpms[current_char - '0'];
        } else {
            switch (current_char) {
                case '-':
                    xpm = hyphen_xpm;
                    break;
                case ':':
                    xpm = colon_xpm;
                    break;
                case '/':
                    xpm = slash_xpm;
                    break;
                default:
                    x += DEFAULT_TIME_PADDING;
                    break;
            }
        }
        if (xpm != NULL) {
            uint32_t* map;
    xpm_image_t img;
    map = (uint32_t*)xpm_load(xpm, XPM_8_8_8_8, &img);
            vg_draw_pixmap(map, &img, x, y);
            x += DEFAULT_TIME_PADDING;
        }
    }

}

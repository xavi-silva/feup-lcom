#include "game_logic.h"
#include "assets/menu.xpm"
#include "assets/menu_foreground.xpm"

Character* police = NULL;
Sprite* mouse = NULL;
uint16_t player_speed = 10;
Menu* menu = NULL;
static GameState current_state = MAIN_MENU;
static EventQueue* event_queue;

int game_logic_init() {
    police = create_police_character();
    mouse = sprite_convert_xpm(cursor_xpm, 576, 432);
    menu = create_menu(sprite_convert_xpm(menu_xpm, 0, 0), sprite_convert_xpm(menu_foreground_xpm, 0, 0));
    event_queue = event_queue_create(30);
    return 0;
}
int game_logic_cleanup() {
    free_character(police);
    free_sprite(mouse);
    free_menu(menu);
    event_queue_destroy(event_queue);
    return 0;
}

GameState get_game_state(){
    return current_state;
}
EventQueue* get_event_queue(){
    return event_queue;
}

void update_game_state(GameState state) {
    current_state = state;
}

void process_timer_interrupt() {
    switch (current_state) {
        case MAIN_MENU:
            draw_menu(menu);
            draw_sprite(mouse);
            break;
        case GAMEPLAY:
            graph_draw_background();
            draw_character(police);
            update_character_animation_timer(police, 1);
            graph_swap_video_mem();
            break;
        case PAUSE_MENU:
            //draw_pause_menu(menu);
            break;
        case EXIT:
            break;    
    }

    graph_swap_video_mem();
}

void process_keyboard_input(uint8_t scancode) {
    switch (current_state) {
        case MAIN_MENU:
            if(scancode == ESC_RELEASE){
                current_state = EXIT;
            }
            break;
        case GAMEPLAY:
            process_keyboard_player_input(scancode);
            break;
        case PAUSE_MENU:
            break;
        case EXIT:
            break;
    }
}

void process_keyboard_player_input(uint8_t scancode) {
    Direction new_direction = police->current_direction;

    switch (scancode) {
        case KBD_W_KEY_MK:
            new_direction = DIRECTION_UP;
            break;
        case KBD_S_KEY_MK:
            new_direction = DIRECTION_DOWN;
            break;
        case KBD_A_KEY_MK:
            new_direction = DIRECTION_LEFT;
            break;
        case KBD_D_KEY_MK:
            new_direction = DIRECTION_RIGHT;
            break;
        case ESC_RELEASE:
            current_state = MAIN_MENU;
            break;    
        default:
            return;
    }
    set_character_direction(police, new_direction);
    move_player(police);

}

void process_mouse_input(int new_x, int new_y, bool mouse_button_pressed){
    int mouse_x = mouse->position.x + new_x;
    int mouse_y = mouse->position.y - new_y;
    if((mouse_x + mouse->img.width > graph_get_h_res()) || mouse_x < 0 || (mouse_y + mouse->img.height > graph_get_v_res()) || mouse_y < 0)return;
        mouse->position.x = mouse_x;
        mouse->position.y = mouse_y;
    switch(current_state){
        case MAIN_MENU:{
            update_menu_state(menu, mouse->position.x, mouse->position.y, mouse_button_pressed);
        }
        default:
            break;
    }
}

void process_rtc_update(char* current_time){
    switch(current_state){
        case MAIN_MENU:
            set_current_time(menu, current_time);
            break;
        default:
            break;
    }
}

void move_player(Character* player) {
     player->ticks_until_stop = player->move_duration;
    int dx = 0, dy = 0;
    switch (player->current_direction) {
        case DIRECTION_UP:
            dy = -1;
            break;
        case DIRECTION_DOWN:
            dy = 1;
            break;
        case DIRECTION_LEFT:
            dx = -1;
            break;
        case DIRECTION_RIGHT:
            dx = 1;
            break;
        default:
            break;
    }

    uint16_t new_x = player->frames[player->current_frame]->position.x + dx * player_speed;
    uint16_t new_y = player->frames[player->current_frame]->position.y + dy * player_speed;

    if (new_x >= 0 && new_x + player->frames[player->current_frame]->img.width <= graph_get_h_res() &&
        new_y >= 0 && new_y + player->frames[player->current_frame]->img.height <= graph_get_v_res()) {
            player->frames[player->current_frame]->position.x = new_x;
            player->frames[player->current_frame]->position.y = new_y;
    }
}

void update_menu_state(Menu* menu, uint16_t mouse_x, uint16_t mouse_y, bool is_mouse_down) {
    for (unsigned i = 0; i < menu->num_buttons; i++) {
        if (check_collision(menu->buttons[i], mouse_x, mouse_y)) {
            menu->buttons[i]->touched = true;
            if(is_mouse_down){
                switch(i){
                    case 0:{
                        update_game_state(GAMEPLAY);
                        break;
                    }
                    case 1:{
                        break;
                    }
                    case 2:{
                        update_game_state(EXIT);
                        break;
                    }
                    default:{
                        return;
                    }
                }
            }
        }
        else {
            menu->buttons[i]->touched = false;
        }
    }
}


void process_event_queue(){
    while(!event_queue_is_empty(event_queue)){
        Event event = dequeue_event(event_queue);
        switch(event.type){
            case KEY_EVENT:{
                process_keyboard_input(event.key_code);
                break;
            }
            case MOUSE_EVENT:{
                process_mouse_input(event.mouse_x, event.mouse_y, event.mouse_button_pressed);
                break;
            }
            case RTC_EVENT:{
                process_rtc_update(event.current_time);
                break;
            }
            default:
                break;
        }
    }
}

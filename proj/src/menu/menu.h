#ifndef _LCOM_MENU_H_
#define _LCOM_MENU_H_

#include "button.h"
#include "objects/sprite.h"
#include <lcom/lcf.h>

#define MAX_BUTTONS 3
#define DEFAULT_BUTTON_COLOR 0xF0EACE
#define DEFAULT_TOUCHED_BUTTON_COLOR 0x9C896D
#define DEFAULT_BUTTON_Y_MARGIN 100
#define DEFAULT_BUTTON_WIDTH 380
#define DEFAULT_BUTTON_HEIGHT 60
#define DEFAULT_BUTTON_X 680
#define DEFAULT_BUTTON_Y 370

#define DEFAULT_TIME_X 640
#define DEFAULT_TIME_Y 800
#define DEFAULT_TIME_WIDTH 450
#define DEFAULT_TIME_HEIGHT 40
#define DEFAULT_TIME_PADDING 20

typedef struct {
    Sprite* background; // Background sprite for the menu
    Sprite* foreground; //Foreground sprite for the menu
    Button* buttons[MAX_BUTTONS]; // Array of buttons in the menu
    unsigned num_buttons; // Number of buttons in the menu
    char* current_time; //Current time gotten from RTC
} Menu;

/**
 * @brief Creates a menu with the given background sprite and default buttons
 *
 * @param background Background sprite for the menu
 * @param background Foreground sprite for the menu
 * @return Pointer to the created menu, or NULL on failure
 */
Menu* create_menu(Sprite* background, Sprite* foreground);

/**
 * @brief Adds a button to the menu
 *
 * @param menu Pointer to the menu
 * @param button Pointer to the button to add
 */
void add_button(Menu* menu, Button* button);

/**
 * @brief Draws the menu on the screen
 *
 * @param menu Pointer to the menu to draw
 */
void draw_menu(Menu* menu);

void set_current_time(Menu* menu, char* current_time);

/**
 * @brief Frees the memory allocated for the menu and its buttons
 *
 * @param menu Pointer to the menu to free
 */
void free_menu(Menu* menu);

/**
 * @brief Draws the current time on the screen.
 *
 * This function draws the current time represented by the menu's `current_time`
 * string on the screen at the default time position.
 *
 * @param menu A pointer to the Menu structure.
 */
void draw_time(Menu* menu);
#endif

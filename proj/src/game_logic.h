/**
 * @file game_logic.h
 * @brief Module for managing game logic
 */

#ifndef _LCOM_GAME_LOGIC_H_
#define _LCOM_GAME_LOGIC_H_

#include <lcom/lcf.h>
#include "event.h"
#include "devices/keyboard.h"
#include "objects/character.h"
#include "assets/cursor.xpm"
#include "menu/menu.h"

/**
 * @enum GameState
 * @brief Represents the possible states of the game
 */
typedef enum {
    MAIN_MENU,    /**< Main menu state */
    GAMEPLAY,     /**< Gameplay state */
    PAUSE_MENU,   /**< Pause menu state */
    EXIT          /**< Exit menu state*/
} GameState;

/**
 * @brief Updates the game state
 *
 * @param state The new game state
 */
void update_game_state(GameState state);

/**
 * @brief Processes keyboard input
 *
 * @param scancode The scancode received from the keyboard
 */
void process_keyboard_input(uint8_t scancode);

/**
 * @brief Processes keyboard input for player actions
 *
 * @param scancode The scancode received from the keyboard
 */
void process_keyboard_player_input(uint8_t scancode);

/**
 * @brief Processes timer interrupt
 */
void process_timer_interrupt();

/**
 * @brief Initializes the game logic
 *
 * @return 0 upon success, non-zero otherwise
 */
int game_logic_init();

/**
 * @brief Cleans up the game logic
 *
 * @return 0 upon success, non-zero otherwise
 */
int game_logic_cleanup();

/**
 * @brief Moves a player character
 *
 * @param player Pointer to the player character to move
 */
void move_player(Character* player);

/**
 * @brief Processes mouse input and enqueues a corresponding event into the event queue
 *
 * @param mouse_x The X-coordinate of the mouse cursor
 * @param mouse_y The Y-coordinate of the mouse cursor
 * @param mouse_button_pressed Indicates if a mouse button is pressed
 */
void process_mouse_input(int new_x, int new_y, bool mouse_button_pressed);


/**
 * @brief Processes the real-time clock input, updating the current time.
 * 
 * This function is responsible for updating the
 * current time string accordingly.
 * 
 * @param current_time Pointer to the string holding the current time.
 */
void process_rtc_update(char* current_time);

/**
 * @brief Updates the state of the menu based on mouse input
 *
 * @param menu Pointer to the menu
 * @param mouse_x X-coordinate of the mouse cursor
 * @param mouse_y Y-coordinate of the mouse cursor
 * @param is_mouse_down Flag indicating whether the mouse button is down
 */
void update_menu_state(Menu* menu, uint16_t mouse_x, uint16_t mouse_y, bool is_mouse_down);

/**
 * @brief Processes an event queue
 *
 * This function is responsible for processing events and updating the game state accordingly.
 */
void process_event_queue();

/**
 * @brief Retrieves the current state of the game
 * 
 * @return The current game state.
 */
GameState get_game_state();

/**
 * @brief Retrieves the event queue associated with the game logic.
 * 
 * @return Pointer to the event queue.
 */
EventQueue* get_event_queue();

#endif // _LCOM_GAME_LOGIC_H_

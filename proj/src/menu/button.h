#ifndef _LCOM_BUTTON_H_
#define _LCOM_BUTTON_H_

#include <lcom/lcf.h>

typedef struct {
    uint16_t x, y;           // Position of the button's top-left corner
    uint16_t width, height;  // Width and height of the button
    uint32_t default_color;  // Default color of the button
    uint32_t touched_color;  // Color of the button when touched
    bool touched;            // Flag indicating whether the button is currently touched
} Button;

/**
 * @brief Creates a button with the given parameters
 *
 * @param x X-coordinate of the button's top-left corner
 * @param y Y-coordinate of the button's top-left corner
 * @param width Width of the button
 * @param height Height of the button
 * @param default_color Default color of the button
 * @param touched_color Color of the button when touched
 * @return Pointer to the created button, or NULL on failure
 */
Button* create_button(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t default_color, uint32_t touched_color);

/**
 * @brief Draws the button on the screen
 *
 * @param button Pointer to the button to draw
 */
void draw_button(Button* button);

/**
 * @brief Checks if the mouse cursor collides with the button
 *
 * @param button Pointer to the button
 * @param mouse_x X-coordinate of the mouse cursor
 * @param mouse_y Y-coordinate of the mouse cursor
 * @return true if the cursor collides with the button, false otherwise
 */
bool check_collision(Button* button, uint16_t mouse_x, uint16_t mouse_y);


/**
 * @brief Frees the memory allocated for the button
 *
 * @param button Pointer to the button to free
 */
void free_button(Button* button);

#endif // _LCOM_BUTTON_H_

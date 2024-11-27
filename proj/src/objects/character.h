/**
 * @file character.h
 * @brief Module for character management
 */

#ifndef _LCOM_CHARACTER_H_
#define _LCOM_CHARACTER_H_

#include "sprite.h"
#include "lcom/lcf.h"
#include "assets/police/police_1.xpm"
#include "assets/police/police_2.xpm"
#include "assets/police/police_3.xpm"
#include "assets/police/police_4.xpm"
#include "assets/police/police_5.xpm"
#include "assets/police/police_6.xpm"

/**
 * @brief Enumeration representing different directions
 */
typedef enum {
    DIRECTION_UP,       /**< Up direction */
    DIRECTION_DOWN,     /**< Down direction */
    DIRECTION_LEFT,     /**< Left direction */
    DIRECTION_RIGHT     /**< Right direction */
} Direction;

/**
 * @brief Structure representing a character
 */
typedef struct {
    Sprite** frames;            /**< Array of Sprites representing frames */
    uint8_t num_frames;         /**< Number of frames */
    uint8_t current_frame;      /**< Index of the current frame */
    Direction current_direction;/**< Current direction */
    uint8_t frame_duration;     /**< Frame duration */
    uint8_t ticks_until_frame_change; /**< Ticks until frame change */
    uint8_t move_duration;      /**< Move duration */
    uint8_t ticks_until_stop;   /**< Ticks until stop */
    bool is_moving;             /**< Flag indicating if character is moving */
} Character;

/**
 * @brief Creates a character with the given frames
 *
 * @param frames Array of sprites representing frames
 * @param num_frames Number of frames
 * @return Pointer to the created character
 */
Character* create_character(Sprite** frames, uint8_t num_frames);

/**
 * @brief Updates the animation timer of the character
 *
 * @param character Pointer to the character
 * @param ticks Number of ticks passed since the last update
 */
void update_character_animation_timer(Character* character, uint8_t ticks);

/**
 * @brief Updates the animation of the character
 *
 * @param character Pointer to the character
 */
void update_character_animation(Character* character);

/**
 * @brief Resets the character animation to the default picture
 *
 * @param character Pointer to the character
 */
void reset_character_animation(Character* character);

/**
 * @brief Gets the current Sprite of the character
 *
 * @param character Pointer to the character
 * @return Pointer to the current Sprite
 */
Sprite* get_current_sprite(Character* character);

/**
 * @brief Creates a police character with predefined frames
 *
 * @return Pointer to the created police character
 */
Character* create_police_character();

/**
 * @brief Sets the next direction of the character
 *
 * @param character Pointer to the character
 * @param direction Next direction to set
 */
void set_character_direction(Character* character, Direction direction);

/**
 * @brief Frees the memory allocated for the character
 *
 * @param character Pointer to the character to free
 */
void free_character(Character* character);

/**
 * @brief Draws the character on the screen
 *
 * @param character Pointer to the character to draw
 */
void draw_character(Character* character);

#endif // _LCOM_CHARACTER_H_

/**
 * @file sprite.h
 * @brief Module for sprite management
 */

#ifndef _LCOM_SPRITE_H_
#define _LCOM_SPRITE_H_

#include <lcom/lcf.h>
#include "../devices/graphics.h"

/**
 * @brief Structure representing coordinates
 */
typedef struct {
    uint16_t x, y; /**< x and y coordinates */
} Coordinates;

/**
 * @brief Enumeration representing rotation angles
 */
typedef enum {
    ROTATION_0,   /**< 0 degrees rotation */
    ROTATION_90,  /**< 90 degrees rotation */
    ROTATION_180, /**< 180 degrees rotation */
    ROTATION_270  /**< 270 degrees rotation */
} RotationAngle;

/**
 * @brief Structure representing a sprite
 */
typedef struct {
    Coordinates position;   /**< Position of the sprite */
    uint32_t* map;          /**< Map of colors */
    xpm_image_t img;        /**< Image */
} Sprite;

/**
 * @brief Converts an XPM to a sprite
 *
 * @param xpm XPM map
 * @param x X coordinate
 * @param y Y coordinate
 * @return Pointer to the created sprite
 */
Sprite* sprite_convert_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

/**
 * @brief Rotates a sprite by 90 degrees
 *
 * @param sprite Pointer to the sprite to rotate
 */
void rotate_xpm_90(Sprite* sprite);

/**
 * @brief Rotates a sprite by 180 degrees
 *
 * @param sprite Pointer to the sprite to rotate
 */
void rotate_xpm_180(Sprite* sprite);

/**
 * @brief Rotates a sprite by 270 degrees
 *
 * @param sprite Pointer to the sprite to rotate
 */
void rotate_xpm_270(Sprite* sprite);

/**
 * @brief Frees the memory allocated for a sprite
 *
 * @param sprite Pointer to the sprite to free
 */
void free_sprite(Sprite* sprite);

void draw_sprite(Sprite* sprite);
/**
 * @brief Draws a sprite with rotation
 *
 * @param sprite Pointer to the sprite to draw
 * @param rotation_angle Rotation angle
 */
void draw_sprite_with_rotation(Sprite* sprite, RotationAngle rotation_angle);

/**
 * @brief Erases a sprite with rotation
 *
 * @param sprite Pointer to the sprite to erase
 * @param rotation_angle Rotation angle
 */
void erase_sprite_with_rotation(Sprite* sprite, RotationAngle rotation_angle);

/**
 * @brief Gets the dimensions of a rotated sprite
 *
 * @param sprite Pointer to the sprite
 * @param rotation_angle Rotation angle
 * @param rotated_width Pointer to store the rotated width
 * @param rotated_height Pointer to store the rotated height
 */
void get_rotated_dimensions(Sprite* sprite, RotationAngle rotation_angle, uint16_t* rotated_width, uint16_t* rotated_height);

#endif // _LCOM_SPRITE_H_

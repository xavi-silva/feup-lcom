#include "sprite.h"

Sprite* sprite_convert_xpm(xpm_map_t xpm, uint16_t x, uint16_t y){
    Sprite* result = (Sprite*) malloc(sizeof(Sprite));
    if(result == NULL){
        printf("Error allocating mem for Sprite\n");
        return NULL;
    }
    Coordinates position;
    position.x = x; position.y = y;
    result->position = position;

    uint32_t* map;
    xpm_image_t img;
    map = (uint32_t*)xpm_load(xpm, XPM_8_8_8_8, &img);

    if(map == NULL){
        printf("Error creating map of colors\n");
        free(result);
        return NULL;
    }
    result->map = map;
    result->img = img;
    return result;
}


void rotate_xpm_90(Sprite* pixmap) {
    uint32_t* rotated_map = (uint32_t*)malloc(pixmap->img.height * pixmap->img.width * sizeof(uint32_t));
    if (rotated_map == NULL) {
        return;
    }

    for (unsigned i = 0; i < pixmap->img.width; i++) {
        for (unsigned j = 0; j < pixmap->img.height; j++) {
            rotated_map[i * pixmap->img.height + j] = pixmap->map[(pixmap->img.height - j - 1) * pixmap->img.width + i];
        }
    }

    uint16_t temp = pixmap->img.width;
    pixmap->img.width = pixmap->img.height;
    pixmap->img.height = temp;

    free(pixmap->map);

    pixmap->map = rotated_map;

    return;
}

void rotate_xpm_180(Sprite* pixmap) {
    uint32_t* rotated_map = (uint32_t*)malloc(pixmap->img.width * pixmap->img.height * sizeof(uint32_t));
    if (rotated_map == NULL) {
        return;
    }

    for (unsigned i = 0; i < pixmap->img.height; i++) {
        for (unsigned j = 0; j < pixmap->img.width; j++) {
            rotated_map[i * pixmap->img.width + j] = pixmap->map[(pixmap->img.height - i - 1) * pixmap->img.width + (pixmap->img.width - j - 1)];
        }
    }

    free(pixmap->map);

    pixmap->map = rotated_map;

    return;
}

void rotate_xpm_270(Sprite* pixmap) {
    uint32_t* rotated_map = (uint32_t*)malloc(pixmap->img.height * pixmap->img.width * sizeof(uint32_t));
    if (rotated_map == NULL) {
        return;
    }

    for (unsigned i = 0; i < pixmap->img.width; i++) {
        for (unsigned j = 0; j < pixmap->img.height; j++) {
            rotated_map[i * pixmap->img.height + j] = pixmap->map[j * pixmap->img.width + (pixmap->img.width - i - 1)];
        }
    }

    uint16_t temp = pixmap->img.width;
    pixmap->img.width = pixmap->img.height;
    pixmap->img.height = temp;

    free(pixmap->map);

    pixmap->map = rotated_map;

    return;
}

void draw_sprite(Sprite* sprite){
    vg_draw_pixmap(sprite->map, &sprite->img, sprite->position.x, sprite->position.y);
}
void draw_sprite_with_rotation(Sprite* pixmap, RotationAngle rotation_angle) {
    uint16_t rotated_width, rotated_height;
    switch (rotation_angle) {
        case ROTATION_90:
        case ROTATION_270:
            rotated_width = pixmap->img.height;
            rotated_height = pixmap->img.width;
            break;
        case ROTATION_180:
            rotated_width = pixmap->img.width;
            rotated_height = pixmap->img.height;
            break;
        default:
            rotated_width = pixmap->img.width;
            rotated_height = pixmap->img.height;
            break;
    }
    for (unsigned i = 0; i < rotated_width; i++) {
        for (unsigned j = 0; j < rotated_height; j++) {
            unsigned rotated_i, rotated_j;
            switch (rotation_angle) {
                case ROTATION_90:
                    rotated_i = j;
                    rotated_j = pixmap->img.height - i - 1;
                    break;
                case ROTATION_180:
                    rotated_i = pixmap->img.width - i - 1;
                    rotated_j = pixmap->img.height - j - 1;
                    break;
                case ROTATION_270:
                    rotated_i = pixmap->img.width - j - 1;
                    rotated_j = i;
                    break;
                default:
                    rotated_i = i;
                    rotated_j = j;
                    break;
            }
            vg_draw_pixel(pixmap->position.x + i, pixmap->position.y + j, pixmap->map[rotated_i + rotated_j * pixmap->img.width]);
        }
    }
}

void get_rotated_dimensions(Sprite* pixmap, RotationAngle rotation_angle, uint16_t* rotated_width, uint16_t* rotated_height) {
    switch (rotation_angle) {
        case ROTATION_90:
        case ROTATION_270:
            *rotated_width = pixmap->img.height;
            *rotated_height = pixmap->img.width;
            break;
        case ROTATION_180:
            *rotated_width = pixmap->img.width;
            *rotated_height = pixmap->img.height;
            break;
        default:
            *rotated_width = pixmap->img.width;
            *rotated_height = pixmap->img.height;
            break;
    }
}



void free_sprite(Sprite* pixmap) {
    if (pixmap == NULL) {
        return;
    }
    free(pixmap->map);
    free(pixmap);
}

void erase_sprite_with_rotation(Sprite* pixmap, RotationAngle rotation_angle) {
    uint16_t rotated_width, rotated_height;
    switch (rotation_angle) {
        case ROTATION_90:
        case ROTATION_270:
            rotated_width = pixmap->img.height;
            rotated_height = pixmap->img.width;
            break;
        case ROTATION_180:
            rotated_width = pixmap->img.width;
            rotated_height = pixmap->img.height;
            break;
        default:
            rotated_width = pixmap->img.width;
            rotated_height = pixmap->img.height;
            break;
    }
    vg_draw_rectangle(pixmap->position.x, pixmap->position.y, rotated_width, rotated_height, TRANSPARENCY_COLOR_8_8_8_8);
}

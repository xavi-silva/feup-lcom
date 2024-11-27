#include "character.h"

Character* create_character(Sprite** frames, uint8_t num_frames) {
    Character* character = (Character*)malloc(sizeof(Character));
    if (character == NULL) {
        free(character);
        return NULL;
    }

    character->frames = frames;
    character->num_frames = num_frames;
    character->current_frame = 0;
    character->current_direction = DIRECTION_RIGHT;
    character->frame_duration = 6;
    character->ticks_until_frame_change = character->frame_duration;
    character->move_duration = 3;
    character->ticks_until_stop = character->move_duration;

    return character;
}

void update_character_animation_timer(Character* character, uint8_t ticks) {
    if(character->ticks_until_frame_change > 0)character->ticks_until_frame_change -= ticks;
    if(character->ticks_until_stop > 0)character->ticks_until_stop -= ticks;

}

void update_character_animation(Character* character) {
   // printf("updatinf\n");
    uint8_t next_frame_index = (character->current_frame + 1) % character->num_frames;
    Sprite* next_frame = character->frames[next_frame_index];
    next_frame->position = get_current_sprite(character)->position;
    character->current_frame = next_frame_index;
    character->ticks_until_frame_change = character->frame_duration;
}

void reset_character_animation(Character* character) {
    uint8_t next_frame_index = 0;
    Sprite* next_frame = character->frames[next_frame_index];
    next_frame->position = get_current_sprite(character)->position;
    character->current_frame = next_frame_index;
    character->ticks_until_frame_change = character->frame_duration;
    character->ticks_until_stop = character->move_duration;


}

Sprite* get_current_sprite(Character* character) {
    if (character == NULL || character->frames == NULL || character->num_frames == 0) {
        return NULL;
    }
    return character->frames[character->current_frame];
}


Character* create_police_character() {
    // Define the filenames of police sprites from police1_xpm to police6_xpm
    const xpm_map_t police_files[] = { police1_xpm, police2_xpm, police3_xpm, police4_xpm, police5_xpm, police6_xpm};
    const uint8_t num_frames = sizeof(police_files) / sizeof(police_files[0]);

    // Load the police sprites
    Sprite** police_frames = (Sprite**)malloc(num_frames * sizeof(Sprite*));
    if (police_frames == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    // Load each police sprite
    for (uint8_t i = 0; i < num_frames; i++) {
        police_frames[i] = sprite_convert_xpm(police_files[i], 50, 50); // Adjust position as needed
        if (police_frames[i] == NULL) {
            // Handle sprite loading failure
            // Free memory for previously loaded sprites
            for (uint8_t j = 0; j < i; j++) {
                free(police_frames[j]);
            }
            free(police_frames);
            return NULL;
        }
    }

    // Create the police character
    Character* police_character = create_character(police_frames, num_frames); // Adjust position as needed

    // Free memory allocated for individual police sprites (frames)
 
    return police_character;
}

void free_character(Character* character) {
    if (character == NULL) {
        return;
    }

    // Free memory for each sprite frame
    for (uint8_t i = 0; i < character->num_frames; i++) {
        free_sprite(character->frames[i]);
    }

    // Free memory for the frames array
    free(character->frames);

    // Free memory for the character struct itself
    free(character);
}

void set_character_direction(Character* character, Direction direction){
    character->current_direction = direction;
}

void draw_character(Character* character){
    //printf("Ticks until change: %u\n", character->ticks_until_frame_change);
    if (character->ticks_until_frame_change <= 0){
           // printf("UPDATING!\n");
            update_character_animation(character);
        }
    else if(character->ticks_until_stop <= 0){
        reset_character_animation(character);
     //   printf("NOT MOVNG");
    }
    switch (character->current_direction){
        case DIRECTION_UP:{
            draw_sprite_with_rotation(character->frames[character->current_frame], ROTATION_270);
            break;
        }
        case DIRECTION_LEFT:{
            draw_sprite_with_rotation(character->frames[character->current_frame], ROTATION_180);
            break;
        } 
        case DIRECTION_DOWN:{
            draw_sprite_with_rotation(character->frames[character->current_frame], ROTATION_90);
            break;
        }     
        case DIRECTION_RIGHT:{
            draw_sprite_with_rotation(character->frames[character->current_frame], ROTATION_0);
            break;
        } 
    }
}

#ifndef _LCOM_MOUSE_GESTURE_H_
#define _LCOM_MOUSE_GESTURE_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "i8042.h"
#include "mouse.h"

typedef enum {
  START,
  DIAGONAL1,
  VERTEX,
  DIAGONAL2,
  END
} GestureState;

typedef struct{
    bool LeftButtonPressed;
    bool RightButtonPressed;
    bool NoButtonPressed;
    bool Ascend;
    bool Descend;
    bool Tolerance;
} MouseEvents;

int update_gesture_state(uint8_t x_len, uint8_t tolerance);
int update_mouse_events(uint8_t tolerance);
#endif

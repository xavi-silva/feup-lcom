#ifndef _LCOM_KEYBOARD_H_
#define _LCOM_KEYBOARD_H_

#include <lcom/lcf.h>
#include "utils.c"
#include "i8042.h"


int kbc_read_status(uint8_t *status);
int kbc_read_output(int port, uint8_t *value);
int kbc_write_input(int port, uint8_t command);
void (kbc_ih)();
int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
int(kbd_restore)();

int wait_for_key(uint8_t keycode);
#endif /*_LCOM_KEYBOARD_H_*/

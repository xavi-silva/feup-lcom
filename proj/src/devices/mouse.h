#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "KBC.h"
#include "i8042.h"


int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
void (mouse_ih)();
void mouse_track_byte_n();
void mouse_bytes_to_struct();
int (mouse_write_dflt_cmd)();
int (mouse_write_input)(uint8_t command);
int mouse_enable_data_report();
int mouse_disable_data_report();
int mouse_restore();

int mouse_updated_packet();

#endif /*_LCOM_MOUSE_H_*/

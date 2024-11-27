#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "i8042.h"


int (kbc_read_status)(uint8_t* status);
int (kbc_read_output)(int port, uint8_t *value, uint8_t mouse);
int (kbc_write_input)(int port, uint8_t command);

#endif /*_LCOM_KBC_H_*/

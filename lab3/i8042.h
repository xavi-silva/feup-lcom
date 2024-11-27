#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBC_IRQ 1

/* I/O port addresses */
#define KBC_STAT_REG 0x64
#define KBC_WRITE_CMD 0x64  //Used to write commands
#define KBC_OUT_BUF 0x60    //Return values
#define KBC_IN_BUF 0x60     //Used to write arguments


/*Status Register*/
#define PARITY_ERROR BIT(7)
#define TIMEOUT_ERROR BIT(6)
#define IBF BIT(1)
#define OBF BIT(0)


/*Scancode MSB*/
#define BREAK_CODE BIT(7)
#define ESC_RELEASE 0x81
#define TWO_BYTE_SIGNAL 0xE0 //Two-byte long scancodes usually use 0xE0 as their first byte.


#define DELAY_US 20000

/*KBC Commands*/
#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60


/*KBC Command Byte*/
#define INT BIT(0) //Enable Interrupt on OBF, from keyboard

#endif /* _LCOM_I8042_H_*/

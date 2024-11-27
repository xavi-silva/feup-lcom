#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

#define KBD_IRQ 1
#define MOUSE_IRQ 12

/* I/O port addresses */
#define KBC_STAT_REG 0x64
#define KBC_WRITE_CMD 0x64  //Used to write commands
#define KBC_OUT_BUF 0x60    //Return values
#define KBC_IN_BUF 0x60     //Used to write arguments


/*Status Register*/
#define PARITY_ERROR BIT(7)
#define TIMEOUT_ERROR BIT(6)
#define MOUSE_DATA BIT(5)
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
#define REMOTE_MODE 0xF0
#define STREAM_MODE 0xEA
#define READ_DATA 0xEB

/*KBC Command Byte*/
#define INT BIT(0) //Enable Interrupt on OBF, from keyboard

/*PS/2 Mouse Data Packet*/
/*BYTE 1*/
#define FIRST_BYTE BIT(3)
#define LB BIT(0) //Left mouse button pressed 
#define RB BIT(1) //Right mouse button pressed 
#define MB BIT(2) //Middle mouse button pressed
#define MSB_X_DELTA BIT(4) //9th bit in mouse's delta_x
#define MSB_Y_DELTA BIT(5) //9th bit in mouse's delta_y
#define X_OVF BIT(6) //Flag that the mouse displacement is too large to be represented as a 9-bit 2-complement integer
#define Y_OVF BIT(7) //Flag that the mouse displacement is too large to be represented as a 9-bit 2-complement integer

/*PS/2 Mouse Commands*/
#define WRITE_BYTE_TO_MOUSE 0xD4
#define DISABLE_DATA_REPORT 0xF5
#define ENABLE_DATA_REPORT 0xF4
#define ACK 0xFA
#define NACK 0xFE


/*Keyboard Keys Used in-game*/
#define KBD_W_KEY_MK 0x11
#define KBD_W_KEY_BRK 0x91
#define KBD_A_KEY_MK 0x1e
#define KBD_A_KEY_BRK 0x9e
#define KBD_S_KEY_MK 0x1F
#define KBD_S_KEY_BRK 0x9f
#define KBD_D_KEY_MK 0x20
#define KBD_D_KEY_BRK 0xa0

#endif /* _LCOM_I8042_H_*/

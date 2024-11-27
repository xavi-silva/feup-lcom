#ifndef _LCOM_VBE_H_
#define _LCOM_VBE_H_

#include <lcom/lcf.h>

//Uncussessful call return codes
#define FUNC_CALL_FAIL 0x01
#define FUNC_NOT_SUPPORTED 0x02
#define FUNC_INVALID 0x03


//Setting Video Modes
#define AH_FUNC 0x4F
#define AL_SET_MODE 0x02
#define AL_GET_MODE_INFO 0x01
#define AL_GET_CTRL_INFO 0x00
#define ITNO_VBE 0x10
#define BX_LINEAR_MAP BIT(14)
 
//VBE Modes
#define GRAPHIC_1024x768_INDEXED 0x105
#define GRAPHIC_640x480_DIRECT 0x110
#define GRAPHIC_800x600_DIRECT 0x115
#define GRAPHIC_1280x1024_DIRECT 0x11A
#define GRAPHIC_1152x864_DIRECT 0x14C

#endif

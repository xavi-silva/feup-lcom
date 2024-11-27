#include "KBC.h"
#include "i8042.h"


int kbc_read_status(uint8_t *status){
    return util_sys_inb(KBC_STAT_REG, status);
}
int kbc_read_output(int port, uint8_t *value, uint8_t mouse){
    uint8_t attempts = 10;
    uint8_t status;
    while(attempts){
        if(kbc_read_status(&status) != 0)return 1;
        //Check if the Output buffer is full (OBF). Discard if it isn't full 
        if(status & OBF){
            //If it is full, read the OUT_BUF, even if the status had some error.
            //(Only check if the status had any error after reading OUT_BUF. If it had, discard the value read in OUT_BUF)
            if(util_sys_inb(port, value) != 0)return 1;
            //Check if there was some error reading signalized by the status register
            //If there was -> return 1
            uint8_t error_check = PARITY_ERROR | TIMEOUT_ERROR;
            if((status & error_check) != 0) return 1;
            
            //If there is the need to check mouse output and data isn't from the mouse
            if(mouse && !(status & MOUSE_DATA)){
                return 1;
            }
            //If there is the need to check keyboard output and data isn't from the keyboard
            if(!mouse && (status & MOUSE_DATA)){
                return 1;
            }
            //If everything went well...
            return 0;
        }
        attempts--;
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;

}


int kbc_write_input(int port, uint8_t command){
    uint8_t attempts = 10;
    uint8_t status;
    while(attempts){
        if(kbc_read_status(&status) != 0)return 1;
        //Check if the Input buffer is full (OBF). Discard if it is full 
        if(!(status & IBF)){
            if(sys_outb(port, command) != 0)return 1;
            return 0;
        }
        attempts--;
        tickdelay(micros_to_ticks(DELAY_US));
    }
    return 1;
}

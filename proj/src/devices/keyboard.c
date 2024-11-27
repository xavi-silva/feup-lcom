#include "keyboard.h"


uint8_t scancode = 0;
int kbd_hook_id = 1;

void(kbc_ih)(){
    kbc_read_output(KBC_OUT_BUF, &scancode, 0);
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL)return 1;
  //sys_irqsetpolicy is used by a Device Driver to tell GIH that it is interedted in an interrupt
  //irqline->for KBC is the KBD_IRQ
  //policy->IRQ_REENABLE informs the GIH that it can unmask the IRQ line in the PIC
  //policy->IRQ_EXCLUSIVE informs the GIH(Generic Interrupt Handler) to not be notified of the occurence of KBC interrupts, thus preventing it from interfering with the "driver"
  //hook_id-> is a pointer to an arbitrary value that will correspond to a specific device. In this case, I chose the keyboard's hook_id = 1
  *bit_no = BIT(kbd_hook_id);
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id);
}

int (kbd_unsubscribe_int)() {
  //sys_irqmpolicy unsubscribes a previous subscription of the interrupt notification associated with the specific hook_id
  return sys_irqrmpolicy(&kbd_hook_id);
}


int(kbd_restore)(){
    //First, tell the keyboard that I want to read the Command Byte
    if(kbc_write_input(KBC_WRITE_CMD, READ_COMMAND_BYTE) != 0)return 1;
    uint8_t command_byte;
    //Then read the command byte
    if(kbc_read_output(KBC_OUT_BUF, &command_byte, 0) != 0)return 1;
    //Change the command byte in the bit with index 0, to enable the interrupt on OBF, from the keyboard
    command_byte |= INT;
    //Tell the keyboard I am going to write a new command byte
    if(kbc_write_input(KBC_WRITE_CMD, WRITE_COMMAND_BYTE) != 0)return 1;
    //Pass to the keyboard the new command byte
    if(kbc_write_input(KBC_IN_BUF, command_byte) != 0)return 1;
    //Interrupts restored
    return 0;
}


int wait_for_key(uint8_t keycode){
  int ipc_status;
  message msg;
  int r;
  uint8_t irq_set;
  if(kbd_subscribe_int(&irq_set) != 0)return 1;
  while(scancode != keycode) { 
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:{ 
          if (msg.m_notify.interrupts & irq_set) { 
            kbc_ih();
          }
          break;
        }
        default:
          break;
      }
    }
  }
  if(kbd_unsubscribe_int() != 0)return 1;
  return 0;
}

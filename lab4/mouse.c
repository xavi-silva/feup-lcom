#include "mouse.h"
#include "KBC.h"
#include "i8042.h"

uint8_t scancode = 0;
int mouse_hook_id = 2;
uint8_t byte_counter; //Keep track of byte number
uint8_t bytes[3];
struct packet packetStruct; //Parce the bytes received into a packet

int (mouse_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL)return 1;
  //sys_irqsetpolicy is used by a Device Driver to tell GIH that it is interedted in an interrupt
  //irqline->for KBC is the KBC_IRQ
  //policy->IRQ_REENABLE informs the GIH that it can unmask the IRQ line in the PIC
  //policy->IRQ_EXCLUSIVE informs the GIH(Generic Interrupt Handler) to not be notified of th
  //policy->IRQ_EXCLUSIVE informs the GIH(Generic Interrupt Handler) to not be notified of the occurence of KBC interrupts, thus preventing it from interfering with the "driver"
  //hook_id-> is a pointer to an arbitrary value that will correspond to a specific device. In this case, I chose the mouse's hook_id = 2
  *bit_no = BIT(mouse_hook_id);
  return sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id);
}

int (mouse_unsubscribe_int)() {
  //sys_irqmpolicy unsubscribes a previous subscription of the interrupt notification associated with the specific hook_id
  return sys_irqrmpolicy(&mouse_hook_id);
}



void(mouse_ih)(){
    kbc_read_output(KBC_OUT_BUF, &scancode, 1);
}

void mouse_track_byte_n(){
  if(byte_counter == 0 && (scancode & FIRST_BYTE)){
    bytes[byte_counter++] = scancode;
    return;
  }
  else if(byte_counter > 0){
    bytes[byte_counter++] = scancode;
  }
}

void mouse_bytes_to_struct(){
  for(int i = 0; i < 3; i++){
    packetStruct.bytes[i] = bytes[i];
  }
  packetStruct.rb = bytes[0] & RB;
  packetStruct.mb = bytes[0] & MB;
  packetStruct.lb = bytes[0] & LB;
  uint16_t delta_x;
  if(bytes[0] & MSB_X_DELTA){
    delta_x = 0xFF00 | bytes[1];
  }
  else{
    delta_x = 0x0000 | bytes[1]; //Just to clarify that is a 9-bit 2's complement
  }
  packetStruct.delta_x = delta_x;
  uint16_t delta_y;
  if(bytes[0] & MSB_Y_DELTA){
    delta_y = 0xFF00 | bytes[2];
  }
  else{
    delta_y = 0x0000 | bytes[2];
  }
  packetStruct.delta_y = delta_y;
  packetStruct.x_ov = bytes[0] & X_OVF;
  packetStruct.y_ov = bytes[0] & Y_OVF;

}


int mouse_enable_data_report(){
  return mouse_write_input(ENABLE_DATA_REPORT);
}
int mouse_disable_data_report(){
  return mouse_write_input(DISABLE_DATA_REPORT);
}
int (mouse_write_input)(uint8_t command) {

  uint8_t attemps = 10;
  while (attemps){
    //To issue a command to the mouse, the "driver" must first write command 0xD4 to the KBC, i.e. using port 0x64
    if (kbc_write_input(KBC_WRITE_CMD, WRITE_BYTE_TO_MOUSE) != 0)return 1;
    //And afterwards the mouse command to port 0x60
    if (kbc_write_input(KBC_IN_BUF, command) != 0) return 1;
    tickdelay(micros_to_ticks(DELAY_US));
    uint8_t acknowledgmentByte;
    if (util_sys_inb(KBC_OUT_BUF, &acknowledgmentByte) != 0) return 1;
    if (acknowledgmentByte == ACK) return 0;
    if(acknowledgmentByte == NACK){
      attemps--;
      continue;
    }
    return 1;
  }
  return 1;
}

int (mouse_write_dflt_cmd)(){
  uint8_t cmd = minix_get_dflt_kbc_cmd_byte();
  if(kbc_write_input(KBC_WRITE_CMD, WRITE_COMMAND_BYTE != 0))return 1;
  //And afterwards the default command byte to port 0x60
  if(kbc_write_input(KBC_IN_BUF, cmd) != 0)return 1;
  return 0;
}

int mouse_restore(){
  if(mouse_write_input(STREAM_MODE) != 0)return 1;
  if(mouse_disable_data_report() != 0)return 1;
  if(mouse_write_dflt_cmd() != 0)return 1;
  return 0;
}

int mouse_updated_packet(){
  mouse_track_byte_n();
  if(byte_counter == 3){
    byte_counter = 0;
    mouse_bytes_to_struct();
    return 0;
  }
  return 1;
}

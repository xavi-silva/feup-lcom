#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int timer_hook_id = 0;
int counter = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */

  if(freq > TIMER_FREQ || freq < 19)return 1;
  uint8_t rb_command;
  if(timer_get_conf(timer, &rb_command) != 0)return 1;
  uint8_t new_config = rb_command & 0x0F; //4 least significant bits should be untouched
  new_config = new_config | TIMER_LSB_MSB;
  switch(timer){
    case 0:
      new_config = new_config | TIMER_SEL0;
      break;
    case 1:
      new_config = new_config | TIMER_SEL1;
      break;
    case 2:
      new_config = new_config | TIMER_SEL2;
      break;    
  }
  uint16_t counter_value = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  util_get_LSB(counter_value, &lsb);
  util_get_MSB(counter_value, &msb);

  if(sys_outb(TIMER_CTRL, new_config) != 0) return 1;
  if(sys_outb(TIMER_0 + timer, lsb) != 0) return 1;
  if(sys_outb(TIMER_0 + timer, msb) != 0) return 1;


  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL)return 1;
  //sys_irqsetpolicy is used by a Device Driver to tell GIH that it is interedted in an interrupt
  //irqline->for timer 0 is the TIMER0_IRQ
  //policy->IRQ_REENABLE informs the GIH that it can unmask the IRQ line in the PIC
  //hook_id-> is a pointer to an arbitrary value that will correspond to a specific device. In this case, I chose the timer's hook_id = 0
  *bit_no = BIT(timer_hook_id);
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id);
}

int (timer_unsubscribe_int)() {
  //sys_irqmpolicy unsubscribes a previous subscription of the interrupt notification associated with the specific hook_id
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  if(timer < 0 || timer > 2 || st == NULL)return 1;
  
  uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_COUNT_| TIMER_RB_SEL(timer);

  int resultOut = sys_outb(TIMER_CTRL, rb_command);
  if(resultOut != 0) return 1;

  int resultIn = util_sys_inb(timer + TIMER_0, st);
  if(resultIn != 0) return 1;
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  /* To be implemented by the students */
  //timer_status_field can be: tsf_all (configuration/status in hexadecimal), tsf_initial (initialization mode), tsf_mode (counting mode), tsf_base(counting base)

  union timer_status_field_val conf;
  //timer_status_field_val: uint8_t byte(status in hexadecimal), enum timer_init in_mode (initialization mode), uint8_t count_mode (counting mode: 0, 1, ..., 5), bool bcd (true if counting in BCD)

  //st corresponds to the "Read-Back Status Format"

  switch(field){
    case tsf_all:{
      conf.byte = st;
      break;
    }
    case tsf_initial:{
    //enum timer_init (INVAL_val, LSB_only, MSB_only, MSB_after_LSB)
      uint8_t in_mode = st & 0x30; //Counter Initialization corresponds to bits 5 and 4 of st. Isolating those bits
      switch(in_mode){
        case TIMER_LSB:
          conf.in_mode = LSB_only;
          break;
        case TIMER_MSB:
          conf.in_mode = MSB_only;
          break;
        case TIMER_LSB_MSB:
          conf.in_mode = MSB_after_LSB;
          break;
        default:
          conf.in_mode = INVAL_val;
          break;  
      }
      break;
    }
    case tsf_mode:{
      uint8_t count_mode = (st >> 1) & 0x07;//Programmed Mode corresponds to bits 3, 2 and 1 of st. Getting only those bits
      //Dúvida: counting mode tem 3 bits logo pode ir de 0 a 7, mas conf.count_mode só pode ir de 0 a 5
      if(count_mode == 6)count_mode = 2;
      if(count_mode == 7)count_mode = 3;
      conf.count_mode = count_mode;
      break;
    }
    case tsf_base:{
      conf.bcd = st & TIMER_BCD;
      break;
    }
    default:
      return 1;  
  }
  if(timer_print_config(timer, field, conf) != 0) return 1;
  return 0;
}

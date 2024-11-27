#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"


#include "../lab2/timer.c"

extern uint32_t sys_calls_counter;
extern uint8_t scancode;
uint8_t bytes[2];
int index_bytes = 0;
extern int counter; //Timer counter for last function

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/g2/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g2/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  int ipc_status;
  message msg;
  int r;
  uint8_t irq_set;
  if(kbd_subscribe_int(&irq_set) != 0)return 1;
  //Should only exit when the user releases the "ESC" key
  while(scancode != ESC_RELEASE) { 
    /* Get a request message. */
    //driver_receive allows device drivers to receive messages
    //first argument: specifies the sender off the messages we want to receive(ANY)
    //second argument and third argument: adresses of varibales that will receive a message and IPC related status respectively
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    //is_ipc_notify returns true if the message received is a notification or false otherwise
    if (is_ipc_notify(ipc_status)) { 
      //m_source contains the endpoint of the sender of the message.
      //endpoint is an andress used my Minix to specify the communication endpoints
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:{ /* hardware interrupt notification */
          if (msg.m_notify.interrupts & irq_set) { 
            //kbc_ih reads the status register and checks if there was some communications error;
            //kbc_ih also reads the scancode byte from the output buffer;
            kbc_ih();
            //kbd_print_scancode(bool make, uint8_t size, uint8_t* bytes)
            //make->true if scancode is a makecode (most significant bit is 0)
            //make->false if scancode is breakcode (most significant bit is 1)
            //size-> size in bytes of the scancode. if the scancode is two bytes, scancode == TWO_BYTE_SIGNAL
            //bytes->scancode
            bytes[index_bytes] = scancode;
            if(scancode == TWO_BYTE_SIGNAL && index_bytes == 0){
              index_bytes++;
            }
            else{
              index_bytes = 0;
              if(bytes[0] == TWO_BYTE_SIGNAL){
                kbd_print_scancode((bytes[1] & BREAK_CODE) == 0, 2, bytes);
              }
              else{
                kbd_print_scancode((bytes[0] & BREAK_CODE) == 0, 1, bytes);
              }
            }
          }
          break;
        }
        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
    // no standard messages expected: do nothing //
    }
  }
  if(kbd_unsubscribe_int() != 0)return 1;
  if(kbd_print_no_sysinb(sys_calls_counter) != 0)return 1;
  return 0;
}

int(kbd_test_poll)() {
  //Should only exit when the user releases the "ESC" key
  while(scancode != ESC_RELEASE){

    //Keep polling the status register (0x64) until OBF is set, then read the OB
    if(kbc_read_output(KBC_OUT_BUF, &scancode) == 0){
      //Same for kbd_test_scan()
      bytes[index_bytes] = scancode;
      if(scancode == TWO_BYTE_SIGNAL && index_bytes == 0){
        index_bytes++;
      }
      else{
        index_bytes = 0;
        if(bytes[0] == TWO_BYTE_SIGNAL){
          kbd_print_scancode((bytes[1] & BREAK_CODE) == 0, 2, bytes);
        }
        else{
          kbd_print_scancode((bytes[0] & BREAK_CODE) == 0, 1, bytes);
        }
      }
    }
  }
  //Enable interrupts by writing command byte before exiting
  if(kbd_restore() != 0)return 1;
  //Print number of system calls to compare interrupt vs polling
  if(kbd_print_no_sysinb(sys_calls_counter) != 0)return 1;
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  message msg;
  int r;
  uint8_t timer0_int_bit;
  uint8_t kbd_int_bit;

  uint8_t time = n;

  //By default, the timer 0 frequency should be 60. However, to avoid potential changes in the frequency made in other functions, it's best to set the frequency to 60
  uint8_t timer = 0;
  uint32_t freq = 60;
  if(timer_set_frequency(timer, freq) != 0)return 1;

  
  if(kbd_subscribe_int(&kbd_int_bit) != 0)return 1;
  if(timer_subscribe_int(&timer0_int_bit) != 0)return 1;
  //Should only exit when the user releases the "ESC" key
  while(scancode != ESC_RELEASE && time > 0) { 
    /* Get a request message. */
    //driver_receive allows device drivers to receive messages
    //first argument: specifies the sender off the messages we want to receive(ANY)
    //second argument and third argument: adresses of varibales that will receive a message and IPC related status respectively
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    //is_ipc_notify returns true if the message received is a notification or false otherwise
    if (is_ipc_notify(ipc_status)) { 
      //m_source contains the endpoint of the sender of the message.
      //endpoint is an andress used my Minix to specify the communication endpoints
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:{ /* hardware interrupt notification */
          if (msg.m_notify.interrupts & kbd_int_bit) { 
            //kbc_ih reads the status register and checks if there was some communications error;
            //kbc_ih also reads the scancode byte from the output buffer;
            kbc_ih();
            //kbd_print_scancode(bool make, uint8_t size, uint8_t* bytes)
            //make->true if scancode is a makecode (most significant bit is 0)
            //make->false if scancode is breakcode (most significant bit is 1)
            //size-> size in bytes of the scancode. if the scancode is two bytes, scancode == TWO_BYTE_SIGNAL
            //bytes->scancode
            bytes[index_bytes] = scancode;
            if(scancode == TWO_BYTE_SIGNAL && index_bytes == 0){
              index_bytes++;
            }
            else{
              index_bytes = 0;
              if(bytes[0] == TWO_BYTE_SIGNAL){
                kbd_print_scancode((bytes[1] & BREAK_CODE) == 0, 2, bytes);
              }
              else{
                kbd_print_scancode((bytes[0] & BREAK_CODE) == 0, 1, bytes);
              }
            }
            time = n;
            counter = 0;

          }
          if (msg.m_notify.interrupts & timer0_int_bit) { 
            timer_int_handler();
            if(counter % 60 == 0){
              time--;
            }

          }
          break;
        }
        default:
          break; // no other notifications expected: do nothing
      }
    }
    else { // received a standard message, not a notification
    // no standard messages expected: do nothing //
    }
  }
  if(timer_unsubscribe_int() != 0) return 1;
  if(kbd_unsubscribe_int() != 0)return 1;
  if(kbd_print_no_sysinb(sys_calls_counter) != 0)return 1;
  return 0;
  
}

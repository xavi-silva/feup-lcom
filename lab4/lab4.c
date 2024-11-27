// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "../lab2/timer.c"
#include "mouse.h"
#include "KBC.h"
#include "mouse_gesture.h"

extern uint8_t scancode;
struct packet packetStruct; //Parce the bytes received into a packet

extern int counter; //Timer counter for mouse_test_async()

extern GestureState currentState; //State for DFA from mouse_test_gesture

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/g2/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/g2/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status;
  message msg;
  int r;
  uint8_t irq_set;
  uint32_t packetsRead = 0;
  //To prevent the mouse from generating useless interrupts, Minix disables (stream mode) data reporting. We need to enable it manually. (0 = enable)
  if(mouse_enable_data_report() != 0){
    printf("enable data report failure");
    return 1;
  }
  if(mouse_subscribe_int(&irq_set) != 0){
    printf("error subscribing mouse interrupts");
    return 1;
  }
  while(packetsRead < cnt) { 
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
            mouse_ih();
            if(mouse_updated_packet() == 0){
              mouse_print_packet(&packetStruct);
              packetsRead++;
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
  //To prevent the mouse from generating useless interrupts, Minix disables (stream mode) data reporting. As we manually enabled data reporting, now we have to disable it. (1 = disable)
  if(mouse_unsubscribe_int() != 0)return 1;
  if(mouse_disable_data_report() != 0){
    printf("disable data report failure");
    return 1;
  }
  return r;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  int r;
  uint8_t mouse_int_bit;
  uint8_t timer0_int_bit;
  uint8_t time = idle_time;
  uint32_t packetsRead = 0;
  //To prevent the mouse from generating useless interrupts, Minix disables (stream mode) data reporting. We need to enable it manually. (0 = enable)
  if(mouse_enable_data_report() != 0){
    printf("enable data report failure");
    return 1;
  }
  if(mouse_subscribe_int(&mouse_int_bit) != 0)return 1;
  if(timer_subscribe_int(&timer0_int_bit) != 0)return 1;
  while(time > 0) { 
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
          if (msg.m_notify.interrupts & mouse_int_bit) { 
            time = idle_time;
            counter = 0;
            mouse_ih();
            if(mouse_updated_packet() == 0){
              mouse_print_packet(&packetStruct);
              packetsRead++;
            }
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
  //To prevent the mouse from generating useless interrupts, Minix disables (stream mode) data reporting. As we manually enabled data reporting, now we have to disable it. (1 = disable)
  if(mouse_unsubscribe_int() != 0)return 1;
   if(mouse_disable_data_report() != 0){
    printf("disable data report failure");
    return 1;
  }
  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  int ipc_status;
  message msg;
  int r;
  uint8_t irq_set;
  if(mouse_enable_data_report() != 0){
    printf("enable data report failure");
    return 1;
  }
  if(mouse_subscribe_int(&irq_set) != 0){
    printf("error subscribing mouse interrupts");
    return 1;
  }
  while(currentState != END) { 
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
            mouse_ih();
            if(mouse_updated_packet() == 0){
              update_gesture_state(x_len, tolerance);
              mouse_print_packet(&packetStruct);
              printf("Current State = %d\n", currentState);
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
  //To prevent the mouse from generating useless interrupts, Minix disables (stream mode) data reporting. As we manually enabled data reporting, now we have to disable it. (1 = disable)
  if(mouse_unsubscribe_int() != 0)return 1;
  if(mouse_disable_data_report() != 0){
    printf("disable data report failure");
    return 1;
  }
  return r;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  if(mouse_write_input(REMOTE_MODE) != 0)return 1;

  while(cnt > 0){
    if(mouse_write_input(READ_DATA) != 0)return 1;
    for(int i = 0; i < 3; i++){
      kbc_read_output(KBC_OUT_BUF, &scancode, 1);
      if(mouse_updated_packet() == 0){
        cnt--;
        mouse_print_packet(&packetStruct);
      }
    }
    tickdelay(micros_to_ticks(period * 1000));
  }
  if(mouse_restore() != 0){
    printf("Mouse restore Error");
    return 1;
  }
  return 0;
}


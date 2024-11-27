#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>
extern int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st;
  if(timer_get_conf(timer, &st) != 0)return 1;
  if(timer_display_conf(timer, st, field) != 0)return 1;

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if(timer_set_frequency(timer, freq) != 0) return 1;

  return 0;
}

int(timer_test_int)(uint8_t time) {
  int ipc_status;
  message msg;
  int r;
  uint8_t irq_set;

  //By default, the timer 0 frequency should be 60. However, to avoid potential changes in the frequency made in other functions, it's best to set the frequency to 60
  uint8_t timer = 0;
  uint32_t freq = 60;
  if(timer_set_frequency(timer, freq) != 0)return 1;

  if(timer_subscribe_int(&irq_set) != 0)return 1;
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
          if (msg.m_notify.interrupts & irq_set) { 
            timer_int_handler();
            if(counter % 60 == 0){
              timer_print_elapsed_time();
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
  if(timer_unsubscribe_int() != 0)return 1;
  return 0;
}

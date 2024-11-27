#include <lcom/lcf.h>
#include <stdint.h>
#include "rtc.h"

extern char current_time[20];

int main(int argc, char **argv) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/g2/lab6/trace.txt");
  lcf_log_output("/home/lcom/labs/g2/lab6/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();

  return 0;
}

int rtc_test_conf(void) {
	for(unsigned int i = SECONDS; i <= RTC_REG_C; i++){
		uint8_t data;
		if(rtc_read(i, &data) != 0)return 1;
		printf("Register %x data: 0x%x\n", i, data);
	}
	return 0;
}

int rtc_test_date(void) {
	if(rtc_read_time(current_time) != 0)return 1;
	printf("Current time: %s\n", current_time);
	return 0;
}


int rtc_test_int() { 
	int ipc_status;
	uint8_t rtc_irq_set;
	message msg;
	int r;
	if(rtc_subscribe_int(&rtc_irq_set) != 0){
		printf("Error subscribing rtc");
		return 1;
	}
	if(rtc_set_continuous_alarm() != 0)return 1;
	while(true) { 
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { 
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:{
          if (msg.m_notify.interrupts & rtc_irq_set) { 
			rtc_ih();
			printf("Current time: %s\n", current_time);
          }
          break;
        }
        default:
          break; 
      }
    }
  }
  if(rtc_unsubscribe_int() != 0){
		printf("Error unsubscribing rtc");
		return 1;
	}
}



int (timer_test_read_config)(uint8_t timer, enum timer_status_field field){
	printf("%d %d\n", timer, field);
	return rtc_test_conf();
}
int (timer_test_time_base)(uint8_t choice, uint32_t nope){
	printf("%d\n", nope);
	return rtc_test_date();
}

int(timer_test_int)(uint8_t time){
    return rtc_test_int();
}

#include <lcom/lcf.h>
#include "serial_port.h"
#include "uart.h"
#include <stdint.h>

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");
  lcf_trace_calls("/home/lcom/labs/g2/lab7/trace.txt");
  lcf_log_output("/home/lcom/labs/g2/lab7/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();

  return 0;
}

int ser_test_conf(unsigned short base_addr) {
    uint8_t dataLCR;
    if(sp_read_register(base_addr, LCR, &dataLCR) != 0){
        printf("Error reading register LCR");
    }
    printf("----LCR----\n");

    uint8_t bits_per_char = dataLCR & (BIT(0) | BIT(1));
    printf("Number of bits per char: ");
    switch(bits_per_char){
        case LCR_5_BITS_PER_CHAR:{
            printf("5\n");
            break;
        }
        case LCR_6_BITS_PER_CHAR:{
            printf("6\n");
            break;
        }
        case LCR_7_BITS_PER_CHAR:{
            printf("7\n");
            break;
        }
        case LCR_8_BITS_PER_CHAR:{
            printf("8\n");
            break;
        }
        default:{
            printf("Error\n");
        }
    } 
    if(dataLCR & LCR_TWO_STOP_BITS){
        printf("Number of stop bits: 2\n");
    }
    else{
         printf("Number of stop bits: 1\n");
    }
    uint8_t parity = dataLCR & (LCR_PARITY_0);
    printf("Parity: ");
    switch(parity){
        case LCR_ODD_PARITY:{
            printf("Odd parity\n");
            break;
        }
        case LCR_EVEN_PARITY:{
            printf("Even parity\n");
            break;
        }
        case LCR_PARITY_1:{
            printf("Always 1\n");
            break;
        }
        case LCR_PARITY_0:{
            printf("Always 0\n");
            break;
        }
        default:{
            printf("No parity\n");
            break;
        }
    }
    unsigned long bit_rate;
    if(sp_get_bit_rate(base_addr, &bit_rate) != 0)return 1;
    printf("Bit rate = %u\n", bit_rate);
    uint8_t dataIER;
    if(sp_read_register(base_addr, IER, &dataIER) != 0){
        printf("Error reading register IER");
    }
    printf("----IER----\n");
    if(dataIER & IER_ENABLE_RECEIVED_DATA_INT){
        printf("Received Data Available Interrupt Enabled\n");
    }
    if(dataIER & IER_ENABLE_TRANSMIT_EMPTY_INT){
        printf("Transmiter Holding Register Empty Interrupt Enabled\n");
    }
    if(dataIER & IER_ENABLE_RECEIVER_LINE_STATUS_INT){
        printf("Receiver Line Status Interrupt Enabled\n");
    }
    if(dataIER & IER_ENABLE_MODEM_STATUS_INT){
        printf("MODEM Status Interrupt Enabled\n");
    }
    return 0;
}

int ser_test_set(unsigned short base_addr, unsigned long bits, unsigned long stop, 
	           long parity, unsigned long rate) { 
    return sp_set_config(base_addr, bits, stop, parity, rate);
}

int ser_test_poll(unsigned short base_addr, unsigned char tx, unsigned long bits, 
                    unsigned long stop, long parity, unsigned long rate, 
                    int stringc, char *strings[]) {
    uint8_t irq_set;
    if(sp_subscribe_int(&irq_set) != 0)return 1;

    if(sp_set_config(base_addr, bits, stop, parity, rate) != 0)return 1;
    sp_disable_int();
    //Receiver
    if(tx == 0){
        char* data = (char*) malloc(250);
        if(sp_receive_data_poll(base_addr, &data) != 0)return 1;
        if (data != NULL) {
        printf("Data received: ");
        for (int i = 0; data[i] != '\0'; i++) {
            printf("%c", data[i]);
        }
        printf("\n");
        }
        free(data);
    }

    //Transmiter
    else if(tx == 1){
        if(sp_send_data_poll(base_addr, stringc, strings) != 0)return 1;
    }
    if(sp_unsubscribe_int() != 0)return 1;

    return 0;
}

int ser_test_int(/* details to be provided */) { 
    int ipc_status, r;
    message msg;
    uint8_t sp_irq_set;

    sp_init();
    sp_enable_int();
    if(sp_subscribe_int(&sp_irq_set) != 0){
        printf("Error subscribing serial port\n");
        return 1;
    }

    while(true) { 
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", r);
        continue;
    }
    if (is_ipc_notify(ipc_status)) { 
        switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:{
            if (msg.m_notify.interrupts & sp_irq_set) {
                sp_ih();
                while(!queue_is_empty(get_received_queue())){
                    uint8_t val = queue_pop(get_received_queue());
                    printf("Received value = %x\n", val);
                } 
            }
                break;
        }
        default:
            break;
        }
    }
    }

    sp_unsubscribe_int();
    sp_exit();
    return 0;
}

int ser_test_fifo(/* details to be provided */) {
    /* To be completed */
    return 0;
}

int (timer_test_read_config)(uint8_t timer, enum timer_status_field field){
    //ser_test_conf(COM1);

    //Test of sender
    sp_disable_int();
    sp_init();
    sp_send_byte(0x1);
    sp_send_byte(0x2);
    sp_send_byte(0x3);
    sp_send_byte(0x4);
    sp_send_byte(0x5);
    sp_send_byte(0x6);
    sp_send_byte(0x7);
    sp_send_byte(0x8);
    sp_send_byte(0x9);
    sp_send_byte(0xA);
    sp_dump_send_queue();
    sp_exit();
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
    /*  printf("Testing ser_test_set with arguments: 8 bits per char, 2 stop bits, Odd Parity, 4 bit-rate\n\n");
    if(ser_test_set(COM1, 8, 2, 0, 4) != 0)return 1;
    if(ser_test_conf(COM1) != 0)return 1;
    */

    //Test receiver
    ser_test_int();
    return 0;
}

int(timer_test_int)(uint8_t time){
    unsigned short base_addr = COM1;
    char *strings[] = {"Hello", "World", "!", NULL};
    int stringc = 3;
    unsigned long bits_per_char = 8;
    ser_test_poll(base_addr, time == 0 ? 0 : 1, bits_per_char, 2, 0, 40, stringc, strings);
    return 0;
}

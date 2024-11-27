#include "serial_port.h"
#include "uart.h"
#include "../lab2/utils.c"

int sp_hook_id = 4;
static Queue *send_queue, *received_queue;

int sp_read_register(unsigned short base_addr, unsigned short port, uint8_t* data){
    if(data == NULL) return 1;
    return util_sys_inb(base_addr + port, data);
}

int sp_write_to_register(unsigned short base_addr, unsigned short port, uint8_t cmd){
    return sys_outb(base_addr + port, cmd);
}

int sp_get_bit_rate(unsigned short base_addr, unsigned long* bit_rate){
    if(bit_rate == NULL) return 1;
    uint8_t bit_rate_lsb, bit_rate_msb;
    if(sp_lcr_dlab_aux(base_addr, true) != 0) return 1;
    if(sp_read_register(base_addr, DL_LSB, &bit_rate_lsb) != 0){
        printf("Error reading Divisor Latch LSB");
        return 1;
    }
    if(sp_read_register(base_addr, DL_MSB, &bit_rate_msb) != 0){
        printf("Error reading Divisor Latch MSB");
        return 1;
    }
    if(sp_lcr_dlab_aux(base_addr, false) != 0) return 1;
    *bit_rate = BIT_RATE_CONST / ((bit_rate_msb << 8) | (bit_rate_lsb));
    return 0;
}

int sp_set_bit_rate(unsigned short base_addr, unsigned long bit_rate){
    uint8_t bit_rate_lsb, bit_rate_msb;
    unsigned long rate = BIT_RATE_CONST / bit_rate;

    if(sp_lcr_dlab_aux(base_addr, false) != 0) return 1;

    if(util_get_LSB(rate, &bit_rate_lsb) != 0) return 1;
    if(util_get_MSB(rate, &bit_rate_msb) != 0) return 1;
    if(sp_write_to_register(base_addr, DL_LSB, bit_rate_lsb) != 0){
        printf("Error writing to Divisor Latch LSB");
        return 1;
    }
    if(sp_write_to_register(base_addr, DL_MSB, bit_rate_msb) != 0){
        printf("Error writing to Divisor Latch MSB");
        return 1;
    }

    if(sp_lcr_dlab_aux(base_addr, true) != 0) return 1;
    return 0;
}

int sp_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop, 
	           long parity, unsigned long rate){
    uint8_t conf = 0;
    switch(bits){
        case 5: {
            conf |= LCR_5_BITS_PER_CHAR;
            break;
        }
        case 6: {
            conf |= LCR_6_BITS_PER_CHAR;
            break;
        }
        case 7: {
            conf |= LCR_7_BITS_PER_CHAR;
            break;
        }
        case 8: {
            conf |= LCR_8_BITS_PER_CHAR;
            break;
        }
        default: {
            printf("Wrong Number of Bits: %u", bits);
            return 1;
        }
    }
    if(stop == 2) conf |= LCR_TWO_STOP_BITS;
    switch(parity){
        case -1: {
            break;
        }
        case 0: {
            conf |= LCR_ODD_PARITY;
            break;
        }
        case 1: {
            conf |= LCR_EVEN_PARITY;
            break;
        }
        default: {
            printf("Wrong Number of Stop Bits: %u", stop);
            return 1;
        }
    }
    if(sp_write_to_register(base_addr, LCR, conf) != 0) return 1;
    if(sp_set_bit_rate(base_addr, rate) != 0) return 1;
    return 0;
}

int sp_subscribe_int(uint8_t *bit_no){
    if(bit_no == NULL) return 1;
    *bit_no = BIT(sp_hook_id);
    return sys_irqsetpolicy(COM1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &sp_hook_id);
}


int sp_unsubscribe_int(){
    return sys_irqrmpolicy(&sp_hook_id);
}

int sp_lcr_dlab_aux(unsigned short base_addr, bool enable){
    uint8_t config;
    if(sp_read_register(base_addr, LCR, &config) != 0) return 1;
    if(enable){
        config |= LCR_ACCESS_DLAB;
    }
    else{
        config &= ~LCR_ACCESS_DLAB;
    }
    if(sp_write_to_register(base_addr, LCR, config) != 0) return 1;
    return 0;
}

int sp_send_data_poll(unsigned short base_addr, int stringc, char *strings[]){
    if(strings == NULL) return 1;
    sp_lcr_dlab_aux(base_addr, false);
    for(int i = 0; i < stringc; i++){
        // If sending last word, it should end with a dot. Else end with " ".
        if(sp_send_string_poll(base_addr, strings[i], i == (stringc - 1) ? true : false)) return 1;
    }
    return 0;
}

int sp_send_string_poll(unsigned short base_addr, char* string, bool last){
    if(string == NULL) return 1;
    unsigned i = 0;
    char c = string[i];
    while(c != '\0'){
        c = string[i];
        if(sp_send_char_poll(base_addr, c) != 0) return 1;
        i++;
    }
    c = last ? '.' : ' ';
    if(sp_send_char_poll(base_addr, c) != 0) return 1;
    return 0;
}

int sp_send_char_poll(unsigned short base_addr, char c){
    uint8_t status;
    if(sp_read_register(base_addr, LSR, &status) != 0) return 1;
    /* busy wait for transmitter ready */
    while( !(status & LSR_THR_EMPTY) ) {
        tickdelay(micros_to_ticks(20000));
        if(sp_read_register(base_addr, LSR, &status) != 0) return 1;
    }
    /* send character */
    if(sp_write_to_register(base_addr, THR, c) != 0) return 1;
   
    printf("Printing char: %c\n", c);
    return 0;
}

int sp_receive_char_poll(unsigned short base_addr, uint8_t* c){
    if(c == NULL) return 1;
    // read LSR to check for errors
    uint8_t status;
    if(sp_read_register(base_addr, LSR, &status) != 0) return 1;
    if((status & (LSR_OVERRUN_ERR | LSR_PARITY_ERR | LSR_FRAMING_ERR)) != 0){
        printf("Error detected in LSR\n");
        return 1;
    }
    sp_lcr_dlab_aux(base_addr, false);
    while(!(status & LSR_RECEIVER_DATA)){
        tickdelay(micros_to_ticks(20000));
          // printf("The variable in hexadecimal is: 0x%x\n", status);
        if(sp_read_register(base_addr, LSR, &status) != 0) return 1;
    }
    if(sp_read_register(base_addr,RBR, c) != 0){
        printf("Error reading char from RBR\n");
        return 1;
    }
   // printf("Inside receive char c : %u\n", c);
    return 0;
}

int sp_receive_data_poll(unsigned short base_addr, char** data){
    if(data == NULL) return 1;
    uint8_t c;
    unsigned int i = 0;
    char stringAux[250];
    while(true){
        if(sp_receive_char_poll(base_addr, &c) != 0) return 1;
        if(c != '\0'){
            stringAux[i] = (char) c;
            i++;
        }
       // printf("Received data is %c\n", c);
        if(c == '.') break;
    }
    stringAux[i] = '\0';
    *data = (char*) malloc(sizeof(char) * (i + 1)); 
    if(*data == NULL) return 1; 
    strncpy(*data, stringAux, i + 1);
 
    return 0;
}

int sp_send_byte(uint8_t byte){
    uint8_t lsr, attempts = 10;
   // printf("Sending byte 0x%x\n", byte);
    queue_push(send_queue, byte);
    while(attempts){
        sp_read_register(COM1, LSR, &lsr);
        if(lsr & LSR_THR_EMPTY)break;
        attempts--;
        tickdelay(micros_to_ticks(20000));
    }
    if(!lsr) return 1;
    return sp_write_to_register(COM1, THR, queue_pop(send_queue));
}

int sp_dump_send_queue(){
    uint8_t lsr = 0;
    while(!queue_is_empty(send_queue)){
        while(!(lsr & LSR_THR_EMPTY)){
           sp_read_register(COM1, LSR, &lsr);
        }
        if(sp_write_to_register(COM1, THR, queue_pop(send_queue)) != 0) return 1;
    }
    return 0;
}

void sp_ih(){
    uint8_t reg;
    sp_read_register(COM1, IIR, &reg);
    //printf("Reg status: 0x%x\n", reg);
    while(!(reg & IIR_NO_INT)) {
        switch( reg &  IIR_INT_ORIGIN) {
        case IIR_RECEIVED_DATA_AVAILABLE:
        case IIR_FIFO_CHAR_TIMEOUT:{
            int r = sp_read_byte();
            while(r == 0){
                r = sp_read_byte();
            }
            sp_read_register(COM1, IIR, &reg);
        }
    }
    }

    
}

void sp_exit(){
    sp_clear_fifo();
    free(send_queue->values);
    free(received_queue->values);
    free(send_queue);
    free(received_queue);
}

void sp_init(){
    send_queue = queue_create(QUEUE_SIZE);
    received_queue = queue_create(QUEUE_SIZE);
    sp_enable_fifo();
    //uint8_t reg;
   // sp_read_register(COM1, IIR, &reg);
   // printf("IIR status: 0x%x\n", reg);
}

int sp_disable_int(){
    uint8_t ier;
    sp_read_register(COM1, IER, &ier);
    ier &= ~(IER_ENABLE_RECEIVED_DATA_INT | IER_ENABLE_TRANSMIT_EMPTY_INT | IER_ENABLE_RECEIVER_LINE_STATUS_INT | IER_ENABLE_MODEM_STATUS_INT);
    return sp_write_to_register(COM1, IER, ier);
}

int sp_enable_int(){
    uint8_t ier;
    sp_read_register(COM1, IER, &ier);
    ier |= IER_ENABLE_RECEIVED_DATA_INT;
    ier &= ~(IER_ENABLE_TRANSMIT_EMPTY_INT | IER_ENABLE_RECEIVER_LINE_STATUS_INT | IER_ENABLE_MODEM_STATUS_INT);
    return sp_write_to_register(COM1, IER, ier);
}

int sp_read_byte(){
    uint8_t lsr, byte;
    sp_read_register(COM1, LSR, &lsr);
    if(lsr & LSR_RECEIVER_DATA){
        sp_read_register(COM1, RBR, &byte);
        if((lsr & (LSR_OVERRUN_ERR | LSR_PARITY_ERR | LSR_FRAMING_ERR | LSR_FIFO_ERR)) != 0) return 1;
        //printf("Cur byte: 0x%x\n", byte);
        queue_push(received_queue, byte);
        return 0;
    }
    return 1;
}

int sp_clear_fifo(){
    uint8_t fcr;
    if(sp_read_register(COM1, FCR, &fcr) != 0) return 1;
    fcr |= (FCR_CLEAR_RECEIVE_FIFO | FCR_CLEAR_TRANSMIT_FIFO);
    return sp_write_to_register(COM1, FCR, fcr);
}

int sp_enable_fifo(){
    uint8_t fcr;
    if(sp_read_register(COM1, FCR, &fcr) != 0) return 1;
    fcr |= FCR_ENABLE_FIFO;
    return sp_write_to_register(COM1, FCR, fcr);
}

Queue* get_received_queue(){
    return received_queue;
}

Queue* get_send_queue(){
    return send_queue;
}

#include "interrupt_handler.h"


extern uint8_t scancode;
extern struct packet packetStruct;
extern int counter;
uint8_t bytes[2];
int index_bytes = 0;

int init_interrupt_handler(uint8_t *timer_irq_set, uint8_t *kbd_irq_set, uint8_t *mouse_irq_set, uint8_t *rtc_irq_set, uint8_t *sp_irq_set) {
    if(timer_subscribe_int(timer_irq_set) != 0){
        printf("Error subscribing timer\n");
        return 1;
    }
    if(kbd_subscribe_int(kbd_irq_set) != 0){
        printf("Error subscribing keyboard\n");
        return 1;
    }
    if(mouse_enable_data_report() != 0){
        printf("Error enabling mouse data report\n");
        return 1;
    }
    if(mouse_subscribe_int(mouse_irq_set) != 0){
        printf("Error subscribing mouse\n");
        return 1;
    }
    if(rtc_set_continuous_alarm() != 0){
        printf("Error setting continuous rtc alarm\n");
        return 1;
    }
    if(rtc_subscribe_int(rtc_irq_set) != 0){
        printf("Error subscribing rtc\n");
        return 1;
    }
    sp_init();
    if(sp_enable_int() != 0){
        printf("Error enabling serial port interrupts\n");
        return 1;
    }
    if(sp_subscribe_int(sp_irq_set) != 0){
        printf("Error subscribing serial port\n");
        return 1;
    }
    return 0;
}

void process_interrupts(InterruptType type, EventQueue *event_queue) {
    switch (type) {
        case RTC_INTERRUPT:
            rtc_interrupt_handler(event_queue);
        case TIMER_INTERRUPT:
            timer_interrupt_handler();
            break;
        case KEYBOARD_INTERRUPT:
            keyboard_interrupt_handler(event_queue);
            break;
        case MOUSE_INTERRUPT:
            mouse_interrupt_handler(event_queue);
            break;
        case SERIAL_PORT_INTERRUPT:
            serial_port_interrupt_handler();
            break;
    }
}

int cleanup_interrupt_handler() {
    if(sp_unsubscribe_int() != 0){
        printf("Error unsubscribing serial port\n");
        return 1;
    }
    sp_exit();
    if(rtc_unsubscribe_int() != 0){
        printf("Error unsubscribing rtc\n");
        return 1;
    }
    if(mouse_unsubscribe_int() != 0){
        printf("Error unsubscribing mouse\n");
        return 1;
    }
    if(mouse_disable_data_report() != 0){
        printf("Error disabling mouse data report\n");
        return 1;
    }
   
    if(kbd_unsubscribe_int() != 0){
        printf("Error unsubscribing keyboard\n");
        return 1;
    }
    if(timer_unsubscribe_int() != 0){
        printf("Error unsubscribing timer\n");
        return 1;
    }
    return 0;
}

void timer_interrupt_handler() {
    timer_int_handler();  
}

void keyboard_interrupt_handler(EventQueue *event_queue) {
    kbc_ih();
    bytes[index_bytes] = scancode;
    if(scancode == TWO_BYTE_SIGNAL && index_bytes == 0){
        index_bytes++;
    }
    else{
        index_bytes = 0;
        /*
        if(bytes[0] == TWO_BYTE_SIGNAL){
            kbd_print_scancode((bytes[1] & BREAK_CODE) == 0, 2, bytes);
        }
        else{
            kbd_print_scancode((bytes[0] & BREAK_CODE) == 0, 1, bytes);
        }*/
        Event event = create_event(KEY_EVENT, scancode, 0, 0, false, NULL);
        enqueue_event(event_queue, event);
    }
}

void mouse_interrupt_handler(EventQueue *event_queue) {
    mouse_ih();
    if (mouse_updated_packet() == 0) {
        //mouse_print_packet(&packetStruct);
         //If MSB_X_DELTA Bit is set, the displacement is negative, so we add 0xFF00 (2's complement)
        int16_t new_x = (packetStruct.bytes[0] & MSB_X_DELTA) ? (0xFF00 | packetStruct.bytes[1]) : packetStruct.bytes[1];
        int16_t new_y = (packetStruct.bytes[0] & MSB_Y_DELTA) ? (0xFF00 | packetStruct.bytes[2]) : packetStruct.bytes[2];
        bool mouse_button_pressed = packetStruct.lb;
        Event event = create_event(MOUSE_EVENT, 0, new_x, new_y, mouse_button_pressed, NULL);
        enqueue_event(event_queue, event);

    }
}

void rtc_interrupt_handler(EventQueue *event_queue) {
    rtc_ih();
    //printf("Current time: %s\n", current_time);
    //Event event = create_event(RTC_EVENT, 0, 0, 0, false, current_time);
    //enqueue_event(event_queue, event);
}

void serial_port_interrupt_handler() {
    
}

#include "game.h"
static uint8_t timer_irq_set, kbd_irq_set, mouse_irq_set, rtc_irq_set, sp_irq_set;
extern int counter;
extern char current_time[20];

int game_init(){
    if(graph_init() != 0){
        printf("Error initializing graphic mode\n");
        return 1;
    }
    if (init_interrupt_handler(&timer_irq_set, &kbd_irq_set, &mouse_irq_set, &rtc_irq_set, &sp_irq_set) != 0) {
        return 1;
    }
    game_logic_init();
    return 0;
}

int game_end(){
    cleanup_interrupt_handler();
    if(game_logic_cleanup() != 0)return 1;
    graph_end();
    return 0;
}

int game_main_loop(){
    int r, ipc_status;
    message msg;
    while ((get_game_state() != EXIT)) {
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:{
                    if (msg.m_notify.interrupts & mouse_irq_set) { 
                        process_interrupts(MOUSE_INTERRUPT, get_event_queue());
                    }
                    if (msg.m_notify.interrupts & rtc_irq_set) { 
                        process_interrupts(RTC_INTERRUPT, get_event_queue());
                        process_rtc_update(current_time);
                    }
                    if (msg.m_notify.interrupts & kbd_irq_set) {
                        process_interrupts(KEYBOARD_INTERRUPT, get_event_queue());
                    }
                    if (msg.m_notify.interrupts & timer_irq_set) {
                        process_interrupts(TIMER_INTERRUPT, get_event_queue());
                        if (counter % 2 == 0) {
                            process_timer_interrupt();
                        }
                    }
                }
            }
            process_event_queue();
        }
    }
    return 0;
}

/**
 * @file interrupt_handler.h
 * @brief Module for handling interrupts
 */

#ifndef _LCOM_INTERRUPT_HANDLER_H
#define _LCOM_INTERRUPT_HANDLER_H

#include <lcom/lcf.h>
#include "event.h"
#include "devices/timer.h"
#include "devices/keyboard.h"
#include "devices/mouse.h"
#include "devices/rtc.h"
#include "devices/serial_port.h"

/**
 * @brief Enum representing different types of interrupts
 */
typedef enum {
    TIMER_INTERRUPT,         /**< Timer interrupt */
    KEYBOARD_INTERRUPT,      /**< Keyboard interrupt */
    MOUSE_INTERRUPT,         /**< Mouse interrupt */
    RTC_INTERRUPT,           /**< RTC (Real-Time Clock) interrupt */
    SERIAL_PORT_INTERRUPT    /**< Serial port interrupt */
} InterruptType;

/**
 * @brief Initializes the interrupt handler
 *
 * @param timer_irq_set Pointer to the variable storing the timer IRQ set
 * @param kbd_irq_set Pointer to the variable storing the keyboard IRQ set
 * @param mouse_irq_set Pointer to the variable storing the mouse IRQ set
 * @param rtc_irq_set Pointer to the variable storing the RTC IRQ set
 * @param sp_irq_set Pointer to the variable storing the serial port IRQ set
 * @return 0 upon success, non-zero otherwise
 */
int init_interrupt_handler(uint8_t *timer_irq_set, uint8_t *kbd_irq_set, uint8_t *mouse_irq_set, uint8_t *rtc_irq_set, uint8_t *sp_irq_set);

/**
 * @brief Processes interrupts of the given type
 *
 * @param type Type of interrupt to process
 * @param event_queue Pointer to the event queue for handling the interrupt
 */
void process_interrupts(InterruptType type, EventQueue *event_queue);

/**
 * @brief Cleans up the interrupt handler
 *
 * @return 0 upon success, non-zero otherwise
 */
int cleanup_interrupt_handler();

/**
 * @brief Interrupt handler for timer interrupts
 */
void timer_interrupt_handler();

/**
 * @brief Interrupt handler for keyboard interrupts
 *
 * @param event_queue Pointer to the event queue for handling the interrupt
 */
void keyboard_interrupt_handler(EventQueue *event_queue);

/**
 * @brief Interrupt handler for mouse interrupts
 *
 * @param event_queue Pointer to the event queue for handling the interrupt
 */
void mouse_interrupt_handler(EventQueue *event_queue);

/**
 * @brief Interrupt handler for RTC (Real-Time Clock) interrupts
 *
 * @param event_queue Pointer to the event queue for handling the interrupt
 */
void rtc_interrupt_handler(EventQueue *event_queue);

/**
 * @brief Interrupt handler for serial port interrupts
 */
void serial_port_interrupt_handler();

#endif // _LCOM_INTERRUPT_HANDLER_H

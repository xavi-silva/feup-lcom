#ifndef _LCOM_SERIAL_PORT_H_
#define _LCOM_SERIAL_PORT_H_

#include <lcom/lcf.h>
#include <queue.h>

int sp_read_register(unsigned short base_addr, unsigned short port, uint8_t* data);
int sp_write_to_register(unsigned short base_addr, unsigned short port, uint8_t cmd);
int sp_get_bit_rate(unsigned short base_addr, unsigned long* bit_rate);
int sp_set_bit_rate(unsigned short base_addr, unsigned long bit_rate);
int sp_set_config(unsigned short base_addr, unsigned long bits, unsigned long stop, 
	           long parity, unsigned long rate);
int sp_subscribe_int(uint8_t *bit_no);
int sp_lcr_dlab_aux(unsigned short base_addr, bool enable);
int sp_unsubscribe_int();

//Polling
int sp_send_char_poll(unsigned short base_addr, char c);
int sp_send_string_poll(unsigned short base_addr, char* string, bool last);
int sp_send_data_poll(unsigned short base_addr, int stringc, char *strings[]);
int sp_receive_char_poll(unsigned short base_addr, uint8_t* c);
int sp_receive_data_poll(unsigned short base_addr, char** data);

int sp_enable_int();
int sp_disable_int();

void sp_ih();
int sp_send_byte(uint8_t byte);
int sp_read_byte();
int sp_dump_send_queue();
void sp_exit();
void sp_init();

int sp_enable_fifo();
int sp_clear_fifo();

Queue* get_received_queue();
Queue* get_send_queue();

#endif

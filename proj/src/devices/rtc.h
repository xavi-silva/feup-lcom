#ifndef _LCOM_RTC_H
#define _LCOM_RTC_H

#include <lcom/lcf.h>

#define RTC_IRQ 8
#define RTC_ADDR_REG 0x70 //writing the address of the register
#define RTC_DATA_REG 0x71 //reading/writing one byte

#define SECONDS 0x00   
#define SECONDS_ALARM 0x01   
#define MINUTES 0x02  
#define MINUTES_ALARM 0x03  
#define HOURS 0x04 
#define HOURS_ALARM 0x05 
#define DAY_WEEK 0x06   
#define DAY_MONTH 0x07  
#define MONTH 0x08 
#define YEAR 0x09 


#define RTC_REG_A 0x0A
#define UIP BIT(7)
#define RS2 BIT(2)  
#define RS1 BIT(1)  
#define RS0 BIT(0)  

#define RTC_REG_B 0x0B
#define SET BIT(7)  //Set to 1 to inhibit updates of time/date registers.
#define PIE BIT(6)  //Set to 1 to enable the corresponding interrupt source
#define AIE BIT(5)   //Set to 1 to enable the corresponding interrupt source
#define UIE BIT(4)  //Set to 1 to enable the corresponding interrupt source
#define SQWE BIT(3) //Set to 1 to enable square-wave generation
#define DM BIT(2) //Don't change;Set to 1 to set time, alarm and date registers in binary. Set to 0, for BCD.
#define HOUR_RANGE BIT(1)  //Don't change;Set to 1 to set hours range from 0 to 23, and to 0 torange from 1 to 12
#define DSE BIT(0)  //Don't change;Set to 1 to enable Daylight Savings Time, and to 0 to disable
 
#define RTC_REG_C 0x0C   
#define IRQF BIT(7) //IRQ line active
#define PF BIT(6)  //Periodic interrupt pending
#define AF BIT(5)  //Alarm interrupt pending
#define UF BIT(4)  //Update interrupt pending

#define RTC_DONT_CARE_VALUE (BIT(7) | BIT(6)) //value to be passed to alarm register for alarm every second

#define RTC_REG_D 0x0D  
#define VRT BIT(7)  //Valid RAM/time – set to 0 when the internal lithium battery runs out of energy – RTC readings are questionable


int rtc_read(uint8_t reg, uint8_t* data);
int rtc_write(uint8_t reg, uint8_t data);
int rtc_wait_valid();
int rtc_subscribe_int(uint8_t *bit_no);
int rtc_unsubscribe_int();
uint8_t bcd_to_binary(uint8_t bcd);
uint8_t rtc_read_second(uint8_t* second);
uint8_t rtc_read_minute(uint8_t *minute);
uint8_t rtc_read_hour(uint8_t* hour);
uint8_t rtc_read_day(uint8_t *minute);
uint8_t rtc_read_month(uint8_t* month);
uint8_t rtc_read_year(uint8_t* year);
int rtc_read_time(char* current_time);


//Sets the SET bit of Register_B and the UIE bit
//Prevents the RTC from updating the time/date registers with the values of the date/time keeping counters
//Receive alarms every second to update time
//Unsets the SET bit of Register_B
int rtc_set_continuous_alarm();

int rtc_ih();
#endif

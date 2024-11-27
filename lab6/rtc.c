#include "rtc.h"
#include "../lab2/utils.c"

int rtc_hook_id = 3;
char current_time[20];

int rtc_read(uint8_t reg, uint8_t* data){
    if(data == NULL)return 1;
    if(sys_outb(RTC_ADDR_REG, reg) != 0){
        printf("Error writing to RTC_ADDR_REG");
        return 1;
    }
    if(util_sys_inb(RTC_DATA_REG, data) != 0){
        printf("Error reading RTC_DATA_REG");
        return 1;
    }
    return 0;
}

int rtc_write(uint8_t reg, uint8_t data){
    if(sys_outb(RTC_ADDR_REG, reg) != 0){
        printf("Error writing to RTC_ADDR_REG");
        return 1;
    }
    if(sys_outb(RTC_DATA_REG, data) != 0){
        printf("Error writing to RTC_DATA_REG");
        return 1;
    }
    return 0;
}

int rtc_wait_valid(){
    uint8_t data;
    while(true){
        if(rtc_read(RTC_REG_A, &data) == 0){
             if(!(data & UIP))return 0;
        }
        tickdelay(micros_to_ticks(20000));
    }
    return data & UIP;
}

int rtc_subscribe_int(uint8_t *bit_no){
    if(bit_no == NULL)return 1;
    *bit_no = BIT(rtc_hook_id);
    return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook_id);
}

int rtc_unsubscribe_int(){
    return sys_irqrmpolicy(&rtc_hook_id);
}

uint8_t bcd_to_binary(uint8_t bcd){
    return ((bcd >> 4) * 10) + (bcd & 0xF);
}

uint8_t rtc_read_second(uint8_t* second){
    if(second == NULL) return 1;
    if(rtc_read(SECONDS, second) != 0) return 1;
    *second = bcd_to_binary(*second);
    return 0;
}

uint8_t rtc_read_minute(uint8_t *minute){
    if(minute == NULL) return 1;
    if(rtc_read(MINUTES, minute) != 0) return 1;
    *minute = bcd_to_binary(*minute);
    return 0;
}

uint8_t rtc_read_hour(uint8_t* hour){
    if(hour == NULL) return 1;
    if(rtc_read(HOURS, hour) != 0) return 1;
    *hour = bcd_to_binary(*hour);
    return 0;
}

uint8_t rtc_read_day(uint8_t *day){
    if(day == NULL) return 1;
    if(rtc_read(DAY_MONTH, day) != 0) return 1;
    *day = bcd_to_binary(*day);
    return 0;
}

uint8_t rtc_read_month(uint8_t* month){
    if(month == NULL) return 1;
    if(rtc_read(MONTH, month) != 0) return 1;
    *month = bcd_to_binary(*month);
    return 0;
}

uint8_t rtc_read_year(uint8_t* year){
    if(year == NULL) return 1;
    if(rtc_read(YEAR, year) != 0) return 1;
    *year = bcd_to_binary(*year);
    return 0;
}

int rtc_read_time(char* current_time){
    if(current_time == NULL)return 1;
    uint8_t second, minute, hour, day, month, year;
    rtc_wait_valid();
    if (rtc_read_second(&second) != 0) return 1;
    if (rtc_read_minute(&minute) != 0) return 1;
    if (rtc_read_hour(&hour) != 0) return 1;
    if (rtc_read_day(&day) != 0) return 1;
    if (rtc_read_month(&month) != 0) return 1;
    if (rtc_read_year(&year) != 0) return 1;
    sprintf(current_time, "%02d/%02d/%04d   %02d:%02d:%02d",
            day, month, year + 2000, hour, minute, second);
    return 0;
    
}



int rtc_set_continuous_alarm(){
    uint8_t reg;
    if(rtc_read(RTC_REG_B, &reg) != 0){
        printf("Error reading Register B\n");
        return 1;
    }
    reg |= SET;
    if(rtc_write(RTC_REG_B, reg) != 0){
        printf("Error setting bit SET of Register B\n");
        return 1;
    }
   
    if(rtc_write(SECONDS_ALARM, RTC_DONT_CARE_VALUE) != 0)return 1;
    if(rtc_write(MINUTES_ALARM, RTC_DONT_CARE_VALUE) != 0)return 1;
    if(rtc_write(HOURS_ALARM, RTC_DONT_CARE_VALUE) != 0)return 1;
   
    if(rtc_read(RTC_REG_B, &reg) != 0){
        printf("Error reading Register B\n");
        return 1;
    }
    reg &= ~SET;
    reg |= UIE;
    if(rtc_write(RTC_REG_B, reg) != 0){
        printf("Error setting bit SET of Register B\n");
        return 1;
    }

    return 0;
}


int rtc_ih(){
    uint8_t regC;
    if(rtc_read(RTC_REG_C, &regC) != 0)return 1;
    if(regC & UF){
        if(rtc_read_time(current_time) != 0)return 1;
    }
    return 0;

}

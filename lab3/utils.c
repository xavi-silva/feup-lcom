#include <lcom/lcf.h>

#include <stdint.h>
uint32_t sys_calls_counter = 0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL)return 1;
  *lsb = val & 0xFF;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL)return 1;
  *msb = val >> 8;
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  sys_calls_counter++;
  if(value == NULL) return 1;
  uint32_t value32;
  int result = sys_inb(port, &value32);
  *value = value32 & 0xFF;
  return result;
}

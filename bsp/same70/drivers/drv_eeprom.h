#ifndef _DRV_EEPROM_H
#define _DRV_EEPROM_H
bool eeprom_init();
bool eeprom_write(int addr, uint8_t *data, uint8_t len)	;
bool eeprom_read(int addr, uint8_t *data, uint8_t len);
bool ms5611_read(uint16_t *calc, uint32_t *temp, uint32_t *press);
#endif

#ifndef _DRV_EEPROM_H
#define _DRV_EEPROM_H
bool eeprom_init();
bool eeprom_write(int addr, uint8_t *data, uint8_t len)	;
bool eeprom_read(int addr, uint8_t *data, uint8_t len);
int ms5611_read();
#endif

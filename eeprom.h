#ifndef  __EEPROM_H__
#define  __EEPROM_H__

unsigned char EEPROM_read (unsigned int uiAddress);
void EEPROM_write (unsigned int uiAddress, unsigned char ucData);

void store_param(void);
void restore_param(void);
void store_param_cal(void);
void restore_param_cal(void);

void  write_mode_U(unsigned char group,unsigned char mode,short value);
void  write_mode_T(unsigned char group,unsigned char mode,short value);
short read_mode_U(unsigned char group,unsigned char mode);
short read_mode_T(unsigned char group,unsigned char mode);


#endif

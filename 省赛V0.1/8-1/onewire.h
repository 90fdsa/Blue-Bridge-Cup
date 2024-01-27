#ifndef _ONEWIRE_H
#define _ONEWIRE_H

#define OW_SKIP_ROM 0xcc
#define DS18B20_CONVERT 0x44
#define DS18B20_READ 0xbe

//º¯ÊıÉùÃ÷
//bit init_ds18b20(void);
unsigned char rd_temperature(void);

#endif
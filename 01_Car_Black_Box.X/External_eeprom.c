
#include "main.h"

void write_external_eeprom(unsigned char address, unsigned char data)
{
	i2c_start();
	i2c_write(SLAVE_WRITE_EXTERNAL);
	i2c_write(address);
	i2c_write(data);
	i2c_stop();
    
    for ( unsigned long int wait = 3000; wait--; );
}

unsigned char read_external_eeprom(unsigned char address)
{
	unsigned char data;

	i2c_start();
	i2c_write(SLAVE_WRITE_EXTERNAL);
	i2c_write(address);
	i2c_rep_start();
	i2c_write(SLAVE_READ_EXTERNAL);
	data = i2c_read();
	i2c_stop();

	return data;
}
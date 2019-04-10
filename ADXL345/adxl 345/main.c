#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>

void TWI_start(void);
void TWI_repeated_start(void);
void TWI_init_master(void);
void TWI_write_address(uint8_t);
void TWI_read_address(uint8_t);
void TWI_write_data(uint8_t);
uint8_t TWI_read_data(void);
uint8_t TWI_read_data_last(void);
void TWI_stop(void);

void error();
void nack_error();

uint8_t address= (0x53) << 1;
uint8_t read=1, write=0;
uint8_t  recv_data;
uint8_t data_rate = 0x0A;
int main(void)
{
    DDRB=0xFF;
    PORTB = address;
    _delay_ms(1000);
    TWI_init_master();   // Function to initialize TWI
    TWI_start();
    TWI_write_address(address+write);
    TWI_write_data(0x2C);
    TWI_write_data(0x0A);
    TWI_stop();
    
    TWI_start();
    TWI_write_address(address+write);
    TWI_write_data(0x2D);
    TWI_write_data(0x08);
    TWI_stop();
    
    TWI_start();
    TWI_write_address(address+write);
    TWI_write_data(0x31);
    TWI_write_data(0x08);
    TWI_stop();
    
    _delay_ms(300);
    
    unsigned int values[6] = {0,0,0,0,0,0};
    while(1)
    {
        int i;
        
        for(i = 50;i<=55;i++)
        {
            
            TWI_start();
            TWI_write_address(address+write);
            TWI_write_data(i);
            
            TWI_repeated_start();
            TWI_read_address(address+read);
            if( (i-50)!=5)
                values[i-50] = TWI_read_data();
            else
                values[i-50] = TWI_read_data_last();
            TWI_stop();
        }
        PORTB = values[0];
    }
    
    
}

void TWI_init_master(void) // Function to initialize master
{
    TWBR=66;  // Bit rate
    TWSR=(0<<TWPS1)|(0<<TWPS0); // Setting prescalar bits
    // SCL freq= F_CPU/(16+2(TWBR).4^TWPS)
}

void TWI_start(void)
{
    // Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT))); // Wait till start condition is transmitted
    if(TWSR & 0xF8 != 0x08)
        error();
}

void TWI_repeated_start(void)
{
    // Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
    TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT))); // wait till restart condition is transmitted
    if(TWSR & 0xF8 != 0x10)
        error();
}

void TWI_write_address(uint8_t data)
{
    
    TWDR=data; // Address and write instruction
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
    if(TWSR & 0xF8 == 0x20)
        nack_error();
    if(TWSR & 0xF8 != 0x18)
        error();
    
}

void TWI_read_address(uint8_t data)
{
    TWDR=data;  // Address and read instruction
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte received
    if(TWSR & 0xF8 == 0x48)
        nack_error();
    if(TWSR & 0xF8 != 0x40)
        error();
}

void TWI_write_data(uint8_t data)
{
    TWDR=data;  // put data in TWDR
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
    if(TWSR & 0xF8 == 0x30)
        nack_error();
    if(TWSR & 0xF8 != 0x28)
        error();
    
}

uint8_t TWI_read_data(void)
{
    TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
    if(TWSR & 0xF8 == 0x58)
        nack_error();
    if(TWSR & 0xF8 != 0x50)
        error();
    recv_data=TWDR;
    return recv_data;
}

uint8_t TWI_read_data_last(void)
{
    TWCR=(1<<TWINT)|(1<<TWEN);    // Clear TWI interrupt flag,Enable TWI
    while (!(TWCR & (1<<TWINT))); // Wait till complete TWDR byte transmitted
    if(TWSR & 0xF8 != 0x58)
        error();
    recv_data=TWDR;
    return recv_data;
}

void TWI_stop(void)
{
    // Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
    TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
    while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
    if(TWSR & 0xF8 != 0x10)
        error();
}

void error()
{
    while(1)
    {
        PORTB = 0xFF;
        _delay_ms(3000);
        PORTB = 0x00;
        _delay_ms(3000);
    }
    
}

void nack_error()
{
    while(1)
    {
        PORTB = 0x01;
        _delay_ms(3000);
        PORTB = 0x00;
        _delay_ms(3000);
    }
    
}

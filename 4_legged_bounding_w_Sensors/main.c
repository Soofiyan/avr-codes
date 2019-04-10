#define F_CPU 14745600UL
#define BAUD 9600
#define BAUDRATE ((F_CPU/(BAUD*16UL)-1))

#include "avr_compiler.h"

bool start = 0,stand_aft = 0;;
uint8_t RX_raw,butt[16],RX_ad[4],RX_ad1,RX[4];
float result;


void bluetooth_init(void)
{
    DDRH |= (1<<PH2);
    PORTH |= (1<<PH2);//bluetooth
}

void standing()
{
    PORTJ &= ~((1<<0)&(1<<1));
    PORTA |= (1<<0)|(1<<1);
    _delay_ms(3000);
}

void usart_init(void)
{
    UBRR2H=BAUDRATE>>8;
    UBRR2L=BAUDRATE;
    UCSR2B=0b10011000;//enable RXEN TXEN
    UCSR2C=0b00000110;// UCSZ1 UCSZ0
}

void bounding()
{
    
}

void ps2_values(void)
{
    if (butt[10]==1)//up
    {
        butt[10] = 0;
    }
    if (butt[11]==1)//left
    {
        butt[11] = 0;
    }
    if (butt[12]==1)//down
    {
        butt[12] = 0;
    }
    if (butt[13]==1)//right
    {
        butt[13] = 0;
    }
    if (butt[14]==1)//start
    {
        start ^= 1;
        standing();
        butt[14] = 0;
    }
}

void pneumatic_init()
{
    DDRA = 0xFF;
    DDRJ = 0xFF;
    DDRE = 0x00;
}

uint8_t sensor_read(uint8_t sens_no)
{
    return(PIND & sens_no);
}

void receive(void)
{
    if ((RX_raw>200) && (RX_raw<255))
    {
        RX_ad1=RX_raw;
        if ((RX_raw>230) && (RX_raw<247))
        {
            uint8_t r_temp0=(RX_raw-231);
            butt[r_temp0]=1;
        }
    }
    else if ((RX_raw>=0) && (RX_raw<201))
    {
        uint8_t r_temp1=(RX_ad1-201);
        if (r_temp1<16)
        {
            RX[r_temp1]=RX_raw;
        }
    }
}

ISR(USART2_RX_vect)
{
    RX_raw=UDR2;
    receive();
}

int main()
{
    bluetooth_init();
    pneumatic_init();
    usart_init();
    standing();
    while(1)
    {
        if(start)
        {
            if(stand_aft)
            {
                PORTJ |= (1<<0);
                while(!(PIND & (1<<0)));
                stand_aft = 0;
            }
            PORTJ &= ~(1<<0);
            PORTJ |= (1<<1);
            PORTA &= ~(1<<1);
            PORTA &= ~(1<<0);
            _delay_ms(250);
            PORTJ &= ~(1<<1);
            PORTA |= (1<<0);
            PORTA |= (1<<1);
            PORTJ |= (1<<0);
            _delay_ms(250);
        }
        else
        {
            stand_aft = 0;
        }
    }
}
//bluetooth h2
//hip a0-front a1-back
//leg j0-back j1-front
//0-out 1-in
//0     2
//1     3

#define F_CPU 14745600UL
#define BAUD 9600
#define BAUDRATE ((F_CPU/(BAUD*16UL)-1))

#include "avr_compiler.h"
#include "lcd.h"

bool actuate_out = 0,actuate_in = 0,start = 0;
uint8_t RX_raw,butt[16],RX_ad[4],RX_ad1,RX[4];
float result;

void bluetooth_init(void)
{
    DDRJ |= (1<<PJ0)|(1<<PJ1);
    PORTJ |= (1<<PJ1)|(1<<PJ0);//bluetooth
}

void usart_init(void)
{
    UBRR3H=BAUDRATE>>8;
    UBRR3L=BAUDRATE;
    UCSR3B=0b10011000;//enable RXEN TXEN
    UCSR3C=0b00000110;// UCSZ1 UCSZ0
}

void ps2_values(void)
{
    if (butt[10]==1)//up
    {
        actuate_out ^=1;
        TCCR3A = 0x00;
        TCCR3B = 0x00;
        TCNT3 = 0;
        butt[10] = 0;
    }
    if (butt[11]==1)//left
    {
        actuate_in ^= 1;
        timer3_init();
        butt[11] = 0;
    }
    if (butt[12]==1)//down
    {
        actuate_out ^= 1;
        TCCR3A = 0x00;
        TCCR3B = 0x00;
        TCNT3 = 0;
        butt[12] = 0;
    }
    if (butt[13]==1)//right
    {
        actuate_in ^= 1;
        timer3_init();
        butt[13] = 0;
    }
    if (butt[14]==1)//start
    {
        start ^= 1;
        butt[14] = 0;
    }
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

ISR(USART3_RX_vect)
{
    RX_raw=UDR3;
    receive();
}

void timer3_init(void)
{
    TCCR3A = 0x00;
    TCCR3B = (1<<CS30);
    TCNT3 = 0;
}

int main()
{
    init_ports();
    lcd_init();
    timer3_init();
    bluetooth_init();
    usart_init();
    DDRD = 0x00;
    DDRA = 0xFF;
    
    lcd_print(1,2,result,1);
    while(1)
    {
        PORTA = 0x02;
        _delay_ms(2000);
        PORTA = 0x00;
        timer3_init();
        while(!(PIND & (1<<0)))
        {
        }
        result = TCNT3;
        float time = result/F_CPU;
        time = time*1000000;
        lcd_print(1,2,time,4);
        _delay_ms(2000);
    }
}

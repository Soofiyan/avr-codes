#define F_CPU 16000000UL
#define red_min 13000
#define green_min 18000
#define blue_min 15000
#define red_max 18000
#define green_max 23000
#define blue_max 19000

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

int result1,result2,result3,result_left,result_right,result;
uint8_t prev_colour;
uint8_t colour;


void timer5_init(void)
{
    TCCR5A = 0x00;
    TCCR5B |= (1<<CS50);
    TCNT5 = 0;
}

void timer5_reset(void)
{
    TCCR5A = 0x00;
    TCCR5B = 0x00;
    TCNT5 = 0;
}

void tcs_init(void)
{
    DDRB |= 0xF0;//s0 s1 s3 s2
    DDRD &= ~(1<<0);//input pin
    PORTB |= (1<<5);
    PORTB &= ~(1<<4);
}

uint8_t tcs_sensor_value(void)
{
    PORTB &= ~((1<<6));
    PORTB |= (1<<7);//no filter
    if(!(PIND & (1<<0)))
    {
        while(!(PIND & (1<<0)));
    }
    while((PIND & (1<<0)));//Wait for falling edge
    timer5_init();
    while(!(PIND & (1<<0)));//Wait for rising edge
    result = TCNT5;
    timer5_reset();
    lcd_print(1,1,result,5);
    _delay_ms(100);
    if(result >= red_min && result <= red_max)
    {
        return 1;
    }
    else if(result >= green_min && result <= green_max)
    {
        return 2;
    }
    else if(result >= blue_min && result <= blue_max)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}

int main(void)
{
    timer5_init();
    tcs_init();
    lcd_port_config();
    lcd_init();
    DDRK = 0xFF;
    PORTK = 0xFF;
    while(1)
    {
        timer5_init();
//        uint8_t colour = tcs_sensor_value();
//        if(prev_colour == 0)
//        {
//            if(colour == 1)
//            {
//                PORTK |= 0x38;
//                PORTK &= ~(1<<3);
//            }
//            else if(colour == 2)
//            {
//                PORTK |= 0x38;
//                PORTK &= ~(1<<4);
//            }
//            else if(colour == 3)
//            {
//                PORTK |= 0x38;
//                PORTK &= ~(1<<5);
//            }
//            else
//            {
//                PORTK |= 0x38;
//            }
//        }
//        prev_colour = colour;
//        lcd_print(1,10,colour,3);
        PORTB &= ~((1<<6));
        PORTB &= ~(1<<7);//red
        if(!(PIND & (1<<0)))
        {
            while(!(PIND & (1<<0)));
        }
        while((PIND & (1<<0)));//Wait for falling edge
        timer5_init();
        while(!(PIND & (1<<0)));//Wait for rising edge
        result1 = TCNT5;
        lcd_print(1,1,result1,5);
        _delay_ms(100);
        timer5_reset();
        timer5_init();
        PORTB |= (1<<6)|(1<<7);//green
        if(!(PIND & (1<<0)))
        {
            while(!(PIND & (1<<0)));
        }
        while((PIND & (1<<0)));//Wait for falling edge
        timer5_init();
        while(!(PIND & (1<<0)));//Wait for rising edge
        result2 = TCNT5;
        lcd_print(1,8,result2,5);
        _delay_ms(100);
        timer5_reset();
        timer5_init();
//        PORTB &= ~((1<<6));
//        PORTB |= (1<<7);//no filter
//        if(!(PIND & (1<<0)))
//        {
//            while(!(PIND & (1<<0)));
//        }
//        while((PIND & (1<<0)));//Wait for falling edge
//        timer5_init();
//        while(!(PIND & (1<<0)));//Wait for rising edge
//        result = TCNT5;
//        lcd_print(2,1,result,5);
//        _delay_ms(100);
//        timer5_reset();
        timer5_init();
        PORTB &= ~((1<<7));
        PORTB |= (1<<6);//blue
        if(!(PIND & (1<<0)))
        {
            while(!(PIND & (1<<0)));
        }
        while((PIND & (1<<0)));//Wait for falling edge
        timer5_init();
        while(!(PIND & (1<<0)));//Wait for rising edge
        result3 = TCNT5;
        lcd_print(2,8,result3,5);
        _delay_ms(100);
        timer5_reset();
        if(result1 >= red_min && result1 <= red_max)
        {
            if(result2 >= 21000)
            colour = 1;
        }
        else if(result2 >= green_min && result2 <= green_max)
        {
            colour = 2;
        }
        else if(result3 >= blue_min && result3 <= blue_max)
        {
            colour = 3;
        }
        else
        {
            colour = 0;
        }
        
        if(colour == 1)
        {
            PORTK |= 0x38;
            PORTK &= ~(1<<3);
        }
        else if(colour == 2)
        {
            PORTK |= 0x38;
            PORTK &= ~(1<<4);
        }
        else if(colour == 3)
        {
            PORTK |= 0x38;
            PORTK &= ~(1<<5);
        }
        else
        {
            PORTK |= 0x38;
        }
}
}
//no nut 26 31 23
//red nut 15
//green nut 21000 22000 19000;
//blue nut 22000 24000 16000;24 26 18;23 25 17


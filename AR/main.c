//
//  main.c
//  AR
//
//  Created by Soofiyan Atar on 14/12/18.
//
#define F_CPU 14745600UL
#define BAUD 9600
#define BAUDRATE ((F_CPU/(BAUD*16UL)-1))
#define target1 1000
#define target2 1000

#define PS_L1 0
#define PS_R1 1
#define PS_L2 2
#define PS_R2 3
#define PS_L3 4
#define PS_R3 5
#define PS_TRIANGLE 6
#define PS_SQUARE 7
#define PS_CROSS 8
#define PS_CIRCLE 9
#define PS_UP 10
#define PS_LEFT 11
#define PS_DOWN 12
#define PS_RIGHT 13
#define PS_START 14
#define PS_SELECT 15

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
//#include "lcd.h"

uint8_t butt[16],RX_raw = 0,RX_ad1 = 0,RX[4],pwm_range = 255,RX_range = 200;
int xj1=0,yj1=0,xj2=0,yj2=0,x_vect=0,y_vect =0;
bool currstatea1  = 0,currstateb1 = 0,currstatea2  = 0,currstateb2 = 0;
int countercw2 = 1000,countercw1 = 0;
bool i=0,check = 1,counter = 1;
bool start = 0;
int speed1 = 150,speed2 = 150;

void port_init(void)
{
    //DDRD = 0x00;  //encoder as input
  //  PORTD = 0xFF; //internal pullup(necessary)
    DDRJ |= (1<<PJ0)|(1<<PJ1);
    PORTJ |= (1<<PJ1)|(1<<PJ0);//bluetooth
    DDRH = 0xFF;  //pwm of drive
    PORTH = 0xFF; //high pwm port
    DDRK = 0xFF;  //direction of drive
    //PORTK = 0xFF;
    DDRF = 0xFF;
}

//void exti_init(void)
//{
//    EIMSK |=(1<<INT0)|(1<<INT1)|(1<<INT2)|(1<<INT3);
//    EICRA |=(1<<ISC00)|(1<<ISC10)|(1<<ISC20)|(1<<ISC30);
//}

void timer_init_motor(void)
{
    TCCR4A |= (1<<COM4A1)|(1<<COM4B1)|(1<<COM4C1)|(1<<WGM40);
    TCCR4B |= (1<<WGM42)|(1<<CS41)|(1<<CS40);
}

//ISR(INT1_vect)
//{
//    if((PIND & 0x02) == 0x02)
//    {
//        currstatea1 = 1;
//        if(currstateb1 == 0)
//        {
//            countercw1 --;
//        }
//        else
//        {
//            countercw1 ++;
//        }
//    }
//    else
//    {
//        currstatea1 = 0;
//        if(currstateb1 == 0)
//        {
//            countercw1 ++;
//        }
//        else
//        {
//            countercw1 --;
//        }
//    }
//}
//
//ISR(INT0_vect)
//{
//    if((PIND & 0x01) == 0x01)
//    {
//        currstateb1 = 1;
//        if(currstatea1 == 0)
//        {
//            countercw1 ++;
//        }
//        else
//        {
//            countercw1 --;
//        }
//    }
//    else
//    {
//        currstateb1 = 0;
//        if(currstatea1 == 0)
//        {
//            countercw1 --;
//        }
//        else
//        {
//            countercw1 ++;
//        }
//    }
//}
//
//ISR(INT3_vect)
//{
//    if((PIND & 0x08) == 0x08)
//    {
//        currstatea2 = 1;
//        if(currstateb2 == 0)
//        {
//            countercw2 --;
//        }
//        else
//        {
//            countercw2 ++;
//        }
//    }
//    else
//    {
//        currstatea2 = 0;
//        if(currstateb2 == 0)
//        {
//            countercw2 ++;
//        }
//        else
//        {
//            countercw2 --;
//        }
//    }
//}
//
//ISR(INT2_vect)
//{
//    if((PIND & 0x04) == 0x04)
//    {
//        currstateb2 = 1;
//        if(currstatea2 == 0)
//        {
//            countercw2 ++;
//        }
//        else
//        {
//            countercw2 --;
//        }
//    }
//    else
//    {
//        currstateb2 = 0;
//        if(currstatea2 == 0)
//        {
//            countercw2 --;
//        }
//        else
//        {
//            countercw2 ++;
//        }
//    }
//}

long limit_var(long in_var, long l_limit, long h_limit)
{
    if (in_var>h_limit)
    {
        in_var=h_limit;
    }
    else if (in_var<l_limit)
    {
        in_var=l_limit;
    }
    return in_var;
}

void drivewheel_1(long sp_vect, long l_lim, long h_lim)//black uper red niche
{
    sp_vect = limit_var(sp_vect,l_lim,h_lim);
    if (sp_vect<(-5))
    {
        PORTK&=(~(1<<PK0));
        PORTK|=(1<<PK1);
        sp_vect=(-sp_vect);
    }
    else if (sp_vect>5)
    {
        PORTK&=(~(1<<PK1));
        PORTK|=(1<<PK0);
    }
    else
    {
        PORTK &= ~(1<<PK0);
        PORTK &= ~(1<<PK1);
        sp_vect=0;
    }
    OCR4A = sp_vect;
    
}

void drivewheel_2(long sp_vect, long l_lim, long h_lim)//red upar black niche
{
    sp_vect = limit_var(sp_vect,l_lim,h_lim);
    if (sp_vect<(-5))
    {
        PORTK &= (~(1<<PK2));
        PORTK |= (1<<PK3);
        sp_vect = (-sp_vect);
    }
    else if (sp_vect>5)
    {
        PORTK&=(~(1<<PK3));
        PORTK|=(1<<PK2);
    }
    else
    {
        PORTK &= ~(1<<PK2);
        PORTK &= ~(1<<PK3);
        sp_vect=0;
    }
    OCR4B = sp_vect;
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

void usart_init(void)
{
    UBRR3H=BAUDRATE>>8;
    UBRR3L=BAUDRATE;
    UCSR3B=0b10011000;//enable RXEN TXEN
    UCSR3C=0b00000110;// UCSZ1 UCSZ0
}

long map_value(long in_value, long in_min, long in_max, long out_min, long out_max)
{
    return (in_value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ps2_values(void)
{
//    xj1=map_value(RX[0],0,RX_range,(pwm_range),(-pwm_range));
//    yj1=map_value(RX[1],0,RX_range,(pwm_range),(-pwm_range));
//    xj2=map_value(RX[2],0,RX_range,(pwm_range),(-pwm_range));
//    yj2=map_value(RX[3],0,RX_range,(pwm_range),(-pwm_range));
    if (butt[0]==1)//l1
    {
        i ^=1;
        butt[0] = 0;
    }
    if (butt[1]==1)//r1
    {
        i ^=1;
        butt[1] = 0;
    }
    if (butt[2]==1)//l2
    {
        i ^=1;
        butt[2] = 0;
    }
    if (butt[3]==1)//r2
    {
        i ^=1;
        butt[3] = 0;
    }
    if (butt[4]==1)//l3
    {
        i ^=1;
        butt[4] = 0;
    }
    if (butt[5]==1)//r3
    {
        i ^=1;
        butt[5] = 0;
    }
    if (butt[6]==1)//triangle
    {
        i ^=1;
        butt[6] = 0;
    }
    if (butt[7]==1)//square
    {
        i ^=1;
        butt[7] = 0;
    }
    if (butt[8]==1)//cross
    {
        i ^=1;
        butt[8] = 0;
    }
    if (butt[9]==1)//circle
    {
        butt[9] = 0;
    }
    if (butt[10]==1)//up
    {
        speed1 += 10;
        butt[10] = 0;
    }
    if (butt[11]==1)//left
    {
        speed2 += 10;
        butt[11] = 0;
    }
    if (butt[12]==1)//down
    {
        speed1 -= 10;
        butt[12] = 0;
    }
    if (butt[13]==1)//right
    {
        speed2 -= 10;
        butt[13] = 0;
    }
    if (butt[14]==1)//start
    {
        start ^= 1;
        butt[14] = 0;
    }
    if (butt[15]==1)//select
    {
        butt[15] = 0;
    }
}

void walking(void)
{
    if(i)
    {
        if(counter)
        {
                if(check)
                {
                    drivewheel_1(0,-255,255);
                    drivewheel_2(0,-255,255);
                    check = 0;
                }
                drivewheel_1(-120,-255,255);
                drivewheel_2(120,-255,255);
            if(countercw2 <=0 || countercw1 >=1000)
            {
                counter = 0;
            }
        }
        else if(!counter)
        {
                if(!check)
                {
                    drivewheel_1(0,-255,255);
                    drivewheel_2(0,-255,255);
                    check = 1;
                }
                drivewheel_1(120,-255,255);
                drivewheel_2(-120,-255,255);
            if(countercw2 >=1000 || countercw1 <=0)
            {
                counter = 1;
            }
        }
    }
    else
    {
        drivewheel_1(0,-255,255);
        drivewheel_2(0,-255,255);
    }
}

int main(void)
{
    port_init();
//    exti_init();
    timer_init_motor();
    usart_init();
    
    sei();
    while(1)
    {
        PORTF = RX_raw;
        ps2_values();
        if(i == 1)
        {
            drivewheel_1(speed1,-255,255);
            drivewheel_2(speed2,-255,255);
        }
        else
        {
            drivewheel_1(0,-255,255);
            drivewheel_2(0,-255,255);
        }
    }
    return 0;
}

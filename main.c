/*
 * manuel_counter.c
 *
 * Created: 5/7/2022 12:21:43 AM
 * Author : LENOVO
 */ 

#define F_CPU 1000000UL
#define bit_is_clear(sfr, bit) //controls if pin is 5V (set) or  pin < 1V (clear)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile int the_count = 0; 
volatile int i;
volatile int k;
volatile uint32_t tick_millisec = 0 ;
int digit[] = {0b000000101, 0b10111101, 0b00100110, 0b10100100, 0b10011100, 0b11000100, 0b01000100, 0b10101101, 0b00000100, 0b10001100};

int dig1 = 0b00000110;  //PortD first digit
int dig2 = 0b00000101;  //PortD second digit


void countdown(){
	int a,b;                    //firstly we find how many 10s we have and what is left from 10s 
	a = the_count/10;
	b = the_count%10;
	
	
	while(a+b != 0){           //while a+b is not 0 this while will continue, if it  both is zero then it means "00" break
		for(int j=300; j>0; j--){     //for 300 miliseconds it will show a digit I didn't do 500 ms or 1000ms because it takes so long to countdown from 99
			PORTB = digit[a];         
			PORTD = dig1;
			_delay_ms(1);
			PORTD = dig2;
			PORTB = digit[b];
			_delay_ms(1);
		}
		b--;        //our digit is "ab" when we show digit for 300ms, then we make b one down 
		if (b==0){  //we check from "ab" if b is 0
			a=a;   //if b is 0 we dont change anything to show numbers like 50,60,70,80  
			b=b;   //if we do like this  a--; b=b  it skips numbers 10,20,30 
		}
		else if (b==-1){  //after b becomes 0 it will go to for once more then with b--; it will be -1 so it means we showed number 20,30,40..
			a--;         //now from "ab" we should make "a" down  and start b from 9 again
			b=9;
		}
		else{        //if b is not 0 or -1 it means we shouldn't touch a or b it should continue b-- till it comes to 0 or -1
			a=a;
		}
	}  //we reached 00 break while
	the_count = 0;   
	k = 0;
	i = 0;			//now we reset all values to 0 then finish function
	a = 0;
	b = 0;
}

void timer0_ovf(){              //resets time and count of overflows to 0 when function is called
	TCCR0 |= (1 << CS02);       //our register zone, here we decide which presecaler mode our timer will work I load timer with 256 prescaler here
	TCNT0 = 0;                   //reset timer to 0
	TIMSK |= (1 << TOIE0);		//enable overflow interrupt  if TOIE0 is 1 it allows to interrupt for timer0 when it overflows
	tick_millisec = 0;			//resets overflow counts to 0
}

ISR(TIMER0_OVF_vect){          //when timer0 overflows this ISR interrupt is called and adds tick_millisec +1
	tick_millisec++;
}
	

void countup(){	
		if (the_count < 99){
			the_count++;
			i = the_count % 10;
			k = the_count / 10;
		}
		else{
			the_count = 0;
			i = the_count % 10;
			k = the_count / 10;
		}	
}
	


void init(){
	DDRD = 0b00000111;
	DDRB = 0b11111111;
	PORTB = 255;
	_delay_ms(2000);
	PORTB = 0;
}



int main(void){
	
    init();
	timer0_ovf();        //timer already starts counting right after microcontroller opens and never stops
    sei();               //enables global interrupts
	
	
    while (1) {
		
			if (bit_is_clear(PIND,PD2)){          //PD2 will be clear when button is pressed
				TCNT0=0;                          //reset time to 0
				tick_millisec=0;                  // reset overflows to 0 after this code timer will continue it will just start from 0,0 again
				
				while(bit_is_clear(PIND,PD2)){    //while button is pressed program will stuck inside this while
					if (tick_millisec>=44){        //tick milisec reaches 44 (aprx 3sec) it calls count down and breaks
						countdown();
						break;
					}
				}
				if (tick_millisec<44)          //if user leaves pressing in while loop, before tick millisec reaches 44
				{								// bit is clear will turn to FALSE cause now it will be set and program will break while before tick=44
					countup();					//we catch tick_millisec<44 here and we call countup function adds +1 to number
				}
			}
			
			if (k < 1){                                             //if the_count/10 is lower than 1 it means display will show 0-9
				PORTB = digit[0];
				PORTD = dig1;
				_delay_ms(1);
				PORTD = dig2;
				PORTB = digit[i];
				_delay_ms(1);
			}
		
			else if (k>=1 && i==0 && the_count<100){                 //display will show if the count is 10, 20, 30, 40, 50... etc 
				PORTB = digit[k];
				PORTD = dig1;
				_delay_ms(1);
				PORTD = dig2;
				PORTB = digit[0];
				_delay_ms(1);
			}
			else if (k>=1 && i!=0 && the_count<100){                   //if the count is 12 or 24 or 53 or 82 etc it will show
				PORTB = digit[k];
				PORTD = dig1;
				_delay_ms(1);
				PORTD = dig2;
				PORTB = digit[i];
				_delay_ms(1);
			}
			else {                                                       //if the count is none of these program will turn to 00
				the_count = 0;
			}
	}
}


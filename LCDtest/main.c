// S/W Environment : AVR Studio + WINAVR Compiler
// Target : M128
// Crystal: 16MHz
//
// Made by NEWTC Co., Ltd. in Korea.
// DAEWOO RYU   
// Email : davidryu@newtc.co.kr
// +82-2-704-4774(TEL), +82-2-704-4733(FAX)
// http://www.newtc.co.kr
// example : Serial LCD module in Terminal mode test from ATMEGA128 to SLCD


#include <avr/io.h>
#include <avr/interrupt.h>
//#include "c:/WinAVR-20100110/avr/include/avr/iom128.h"
#include <stdio.h>
#include <stdlib.h>

char buff[30];


static int Putchar(char c, FILE *stream);
void tx0Char(char message);
void tx1Char(char message);



static int Putchar(char c, FILE *stream)
{
	tx0Char(c);
    tx1Char(c);
	return 0;
      
}

// UART0 을 이용한 출력
void tx0Char(char message)
{
	while (((UCSR0A>>UDRE0)&0x01) == 0) ;  // UDRE, data register empty        
    UDR0 = message;
}

// UART1 을 이용한 출력
void tx1Char(char message)
{
	while (((UCSR1A>>UDRE1)&0x01) == 0) ;  // UDRE, data register empty        
    UDR1 = message;
}


void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0x00;
 PORTB = 0x00;
 DDRB  = 0x00;
 PORTC = 0x00; //m103 output only
 DDRC  = 0x00;
 PORTD = 0x00;
 DDRD  = 0x00;
 PORTE = 0x00;
 DDRE  = 0x00;
 PORTF = 0x00;
 DDRF  = 0x00;
 PORTG = 0x00;
 DDRG  = 0x00;
}

//UART0 initialize
// desired baud rate: 9600
// actual: baud rate:9615 (0.2%)
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSR0B = 0x00; //disable while setting baud rate
 UCSR0A = 0x00;
 UCSR0C = 0x06;
 UBRR0L = 0x67; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 UCSR0B = 0x18;
}

//UART1 initialize
// desired baud rate:9600
// actual baud rate:9615 (0.2%)
// char size: 8 bit
// parity: Disabled
void uart1_init(void)
{
 UCSR1B = 0x00; //disable while setting baud rate
 UCSR1A = 0x00;
 UCSR1C = 0x06;
// UBRR1L = 0x2F; //set baud rate lo 7.3728 MHz
// UBRR1L = 0x47; //set baud rate lo 11.0592 Mhz
 UBRR1L = 0x67; //set baud rate lo 16Mhz
 UBRR1H = 0x00; //set baud rate hi
 UCSR1B = 0x18;
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 XMCRB = 0x00; //external memory
 port_init();
 uart0_init();
 uart1_init();
 fdevopen(Putchar,0);

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources

 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

// 시간 지연 함수
void delay_us(int time_us)
{
   register int i;
   for(i=0; i<time_us; i++){   // 4 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
     /* asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle   = 16 cycle = 1us for 16MHz*/
   }
}

void delay_ms(int time_ms)
{
   register int i;
   for(i=0;i<time_ms;i++) delay_us(1000);
}


//
int main(void)
{
 int i=0;
 init_devices();
 //insert your functional code here...
 while(1){
 	i++;
	itoa(i, buff, 10);
	printf("%s\r\n",buff);
	delay_ms(1200);

	sprintf(buff,"%d",i);
	printf("%s\r\n",buff);
	delay_ms(1200);
    printf("testing...");
	delay_ms(1200);
	printf("$$CS\r");		// 화면 클리어
	delay_ms(1200);	
	printf("$$L0\r");		// 백라이트 off
	delay_ms(1200);	
	printf("$$L1\r");		// 백라이트 on
	delay_ms(1200);
	printf("$$BB\r");		// 커서 Blink
	delay_ms(1200);
	
	/*
	터미널 창에서 아래 명령어를 텍스트로 전송하고 ‘\r’ 을 전송하면 아래
	의 해당 동작을 한다.
	$$CS - 화면을 지우는 명령
	$$B0 - 커서를 OFF 시키는 명령
	$$B1 - 커서를 ON 시키는 명령
	$$BB - 커서를 Blink 시키는 명령
	$$L0 - 백라이트를 OFF 시키는 명령
	$$L1 - 백라이트를 ON 시키는 명령
	*/
 }
}

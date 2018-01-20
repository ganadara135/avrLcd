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

// 내 모듈은 16MHz 크리스탈사용, 컴파일러에게 안알려주면 1MHz 인식
// _delay_ms()의 최대시간 16.38ms, _delay_us() 최대 시간 48us
#define F_CPU 16000000UL          //UL unsigned long

/*
i >> x  : i의 비트열을 오른쪽으로 x만큼 이동
i << x  : i의 비트열을 왼쪽으로 x만큼 이동
*/

char buff[30];
char rx_flag=0;

static int Putchar(char c, FILE *stream);
void tx0Char(char message);
void tx1Char(char message);

static int Putchar(char c, FILE *stream)
{
	// UART 두 개에 다 메시지를 출력함
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
		/*
		PINx  = Port x Input Pins Address : Read만 가능
		PORTx = Port x Data Register  : x포트에서 값을 내보내거나 받을 것인지
		DDRx  = Port x Data Direction Register : x포트를 입력으로 할 지, 출력으로 할 지
		
		DDRx  =  0x00 포트X 를 입력으로 설정
		PORTx = 0x00;   PORTx 로 0 을 입력 받음
		PORTx = 0xFF;   PORTx 로 1 을 입력 받음
		
		DDRx  =  0xFF  포트X 를  출력   설정
		PORTx = 0x00;   PORTx 로 0 을 출력함
		PORTx = 0xFF;   PORTx 로 1 을 출력함
	*/
		
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
 UCSR0C = 0x06;   // 0000_0110
 UBRR0L = 0x67; //set baud rate lo
 UBRR0H = 0x00; //set baud rate hi
 //UCSR0B = 0x18;  // 수신가능
  UCSR0B= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
 //UCSR0B = (1<<TXEN0);
 //UCSR0B = (1<<RXEN0);
}

// UART1 initialize
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
 //UCSR0B= (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
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
 ETIMSK = 0x00; //SREG 직접 설정 대신 모듈화 호출
 
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


//  CLCD 용 명령어 쓰기 함수  // 내건 SLCD 이다
void lcd_cmd_write(char cmd){
	PORTF = 0x00;   // RS=0, R/W=0, E=0
	_delay_us(1);
	PORTF^=0x04;   // E=1
	PORTA = cmd;    // 명령어 데이터 출력
	PORTF^= 0x04;   //  E=0
	_delay_ms(5);	
}

//  CLCD 용 데이터 쓰기 함수  // 내건 SLCD 이다
void lcd_data_write(char cmd){
	PORTF = 0x01;   // RS=1, R/W=0, E=0
	_delay_us(1);
	PORTF^=0x04;   // E=1
	PORTA = cmd;    // 명령어 데이터 출력
	PORTF^= 0x04;   //  E=0
	_delay_ms(5);
}
// CLCD 용으로 문자열 입력 함수와 위치 지정 함수, 초기화함수 추가 필요


// UART0 이용한 수신
unsigned char rx0Char(void)
{
	while(!(UCSR0A&(1<<RXC0))); // 데이터 수신이 완료때까지 대기ㅣ
	rx_flag= 1;    // 수신 체크를 위한 flag
	return UDR0;   // 수신된 데이터 리턴
}

int main(void)
{
 int i=0;
 init_devices();
 //insert your functional code here...
 unsigned char rx_buf;  // 수신 데이터를 받을 변수
 
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
	
	// 수신 부분 처리
/*
	rx_buf= rx0Char();
	if(rx_flag){
		rx_flag= 0;
			 
		//	sprintf(buff,"%d",i);
		printf("%s\r\n",rx_buf);
		delay_ms(2400);
	}
	*/
 }
}

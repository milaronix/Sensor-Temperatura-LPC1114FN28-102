//Sensor de Temperatura
//LPC1114FN28/102
//Universidad Galileo, Guatemala 2014
//Erick Braul, Sergio Saenz y Milton Godinez

//CREDITOS
// funcion delay : http://www.eevblog.com/forum/microcontrollers/lpc1114-28dip-tutorials/
// informacion calculo baud rate: csserver.evansville.edu/~blandfor/EE354/ARMLecture/ARMLect7.pdf

#include "LPC11xx.h"

#define U0THR (*((volatile unsigned long *) 0x40008000)) //Transmit Buffer
#define U0DLL (*((volatile unsigned long *) 0x40008000)) //Divisor Latch LSByte
#define U0FCR (*((volatile unsigned long *) 0x40008008)) //FIFO control
#define U0LCR (*((volatile unsigned long *) 0x4000800C)) //Line control
//Autobaud control
#define U0FDR (*((volatile unsigned long *) 0x40008028)) //Fractional divide register
#define U0TER (*((volatile unsigned long *) 0x40008030)) //Transmit enable
//Enable TxD to output
#define IOCON_PIO1_7 (*((volatile unsigned long*) 0x400440A8)) //Pin control register
#define SYSAHBCLKCTRL (*((volatile unsigned long *) 0x40048080)) //System AHB clock control
#define UARTCLKDIV (*((volatile unsigned long *) 0x40048098)) //UART clock divider

void delay_us( int);
void delay_ms( int);
void desliza_texto(int posiciones);

int main(){
	int i;
	IOCON_PIO1_7 |= 0x01;			//TxD enable to output
	//LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);
	SYSAHBCLKCTRL |= (1 << 12); 	//Clock to UART
	UARTCLKDIV |= 4; 				//48000000/4 = 12 MHz for UARTPCLK
	U0FCR |= 0x01; 					//Enable FIFO buffer
	U0LCR |= 1 << 7; 				//DLAB must be 1 to write to U0DLL, and U0FDR
	//divAdd=7 mulVal=10 dll=23 //Set up baud rate
	U0FDR = 0xA7; 					//DIVADDVAL is in bits 0-3 and MULVAL is in bits 4-7
	U0DLL = 46;
	U0LCR &= ~(1 << 7); 			//DLAB back to 0 to transmit
	U0LCR |= 0x03; 					//8-bit data width
	
	delay_ms(2000);					//espera lo suficiente para el arranque del display LCD
		
	U0THR = 0xfe;						//comando escape para poder correr el cursor
	U0THR = 130;						//corrimiento cursor para centrar la frase
	U0THR = 70;delay_ms( 100);
	U0THR = 101;delay_ms( 100);
	U0THR = 108;delay_ms( 100);
	U0THR = 105;delay_ms( 100);
	U0THR = 99;delay_ms( 100);
	U0THR = 105;delay_ms( 100);
	U0THR = 100;delay_ms( 100);
	U0THR = 97;delay_ms( 100);
	U0THR = 100;delay_ms( 100);
	U0THR = 101;delay_ms( 100);
	U0THR = 115;delay_ms( 100);
	
	U0THR = 0xfe;						//comando escape para poder correr el cursor
	U0THR = 197;						//corrimiento cursor para centrar la frase
	
	U0THR = 73;delay_ms( 100);
	U0THR = 110;delay_ms( 100);
	U0THR = 103;delay_ms( 100);
	U0THR = 101;delay_ms( 100);
	U0THR = 33;delay_ms( 100);
	U0THR = 33;delay_ms( 100);
	U0THR = 33;delay_ms( 100);
	
	desliza_texto(26);
	
	while(1){
	}
}

void desliza_texto(int posiciones){
	int i;
	for(i=0; i <= posiciones; ++i){
			U0THR = 0xfe;						//comando escape para poder correr el cursor
			U0THR = 0X18;						//corrimiento cursor para centrar la frase
			delay_ms(900);
	}
}
void splash_screen (){
	U0THR = 0x7c;						//comando escape 
	U0THR = 9;							//habilita splash screen
}

void delay_us(int delay)
{
  while(delay--)
  {
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
      __asm volatile (" NOP");
  }
}

void delay_ms(int delay)
{
  char i;
  while(delay--)
  {
    for(i=0; i<4; i++)
    {
      delay_us(250);
    }
  }
}
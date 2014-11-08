//Sensor de Temperatura
//LPC1114FN28/102
//Universidad Galileo, Guatemala 2014
//Erick Braul, Sergio Saenz y Milton Godinez

//CREDITOS
// funcion delay : http://www.eevblog.com/forum/microcontrollers/lpc1114-28dip-tutorials/
// informacion calculo baud rate: csserver.evansville.edu/~blandfor/EE354/ARMLecture/ARMLect7.pdf

#include <LPC11xx.h>			/* LPC11xx Peripheral Registers */
#include "system_LPC11xx.h"

#define U0THR (*((volatile unsigned long *) 0x40008000)) //Transmit Buffer
#define U0DLL (*((volatile unsigned long *) 0x40008000)) //Divisor Latch LSByte
#define U0FCR (*((volatile unsigned long *) 0x40008008)) //FIFO control
#define U0LCR (*((volatile unsigned long *) 0x4000800C)) //Line control
//Autobaud control
#define U0FDR (*((volatile unsigned long *) 0x40008028)) //Fractional divide register
#define U0TER (*((volatile unsigned long *) 0x40008030)) //Transmit enable


/*void delay_us( int);
void delay_ms( int);
void desliza_texto(int posiciones);
void splash_screen();
void set_splash_screen();
void imprime();
void limpia_pantalla();*/

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

void desliza_texto(int posiciones){
	int i;
	for(i=0; i <= posiciones; ++i){
			U0THR = 0xfe;						//comando escape para poder correr el cursor
			U0THR = 0X18;						//corrimiento cursor para centrar la frase
			delay_ms(900);
	}
}

void limpia_pantalla(){
	U0THR = 0xfe;						//comando escape 
	U0THR = 0x01;						//habilita splash screen
}

void splash_screen(){
	U0THR = 0x7c;						//comando escape 
	U0THR = 9;							//habilita splash screen
}

void set_splash_screen(){
	U0THR = 0x7c;						//comando escape 
	U0THR = 10;						//comando escape 
}

void imprime(char *texto, int delay, int desliza){
	int posicion = 0;
	int largo = 0;
	
	if(desliza == 1){
		while(texto[posicion++]){
			largo++;
		}
	}
	
	posicion = 0;
	while(texto[posicion]){		
		if(posicion == 16){
			if(desliza == 1 || desliza == 2){
				U0THR = 0xfe;						//comando escape para poder correr el cursor
				U0THR = 148;						//cambia a la primer posicion de la segunda linea
				delay_ms(100);
			}else{
				U0THR = 0xfe;						//comando escape para poder correr el cursor
				U0THR = 192;						//cambia a la primer posicion de la segunda linea
				delay_ms(50);
			}
		}
		U0THR = texto[posicion];delay_ms(delay);  //imprime
		posicion++;
	}
	
	if(desliza == 1){
		desliza_texto(largo);
	}
	if(desliza == 2){
		desliza_texto(0xFFFFF);
	}
}

void buzzer(){
	int m;
	int i;
	int n = 0;
	
	while(n < 999){
		while (m < 10){//ciclo uno
			LPC_TMR16B1 ->MR0 = LPC_TMR16B1 ->MR0 + 1;
				while (i < 99999){
				i = i + 1;
				};
			i = 0;
			m = m+1;
		};
		m=0;i=0;
		while (m < 10){//ciclo dos
			LPC_TMR16B1 ->MR0 = LPC_TMR16B1 ->MR0 - 1;
				while (i < 99999){
				i = i + 1;
				};
			i = 0;
			m = m+1;
		};
		m=0;i=0;
	}
	
}

int main(){
	LPC_IOCON ->PIO1_7 |= 0x01;			//TxD enable to output
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);		//Clock to UART
	LPC_SYSCON->UARTCLKDIV |= 4; 				//48000000/4 = 12 MHz for UARTPCLK
	U0FCR |= 0x01; 					//Enable FIFO buffer
	U0LCR |= 1 << 7; 				//DLAB must be 1 to write to U0DLL, and U0FDR
	//divAdd=7 mulVal=10 dll=23 //Set up baud rate
	U0FDR = 0xA7; 					//DIVADDVAL is in bits 0-3 and MULVAL is in bits 4-7
	U0DLL = 46;
	U0LCR &= ~(1 << 7); 			//DLAB back to 0 to transmit
	U0LCR |= 0x03; 					//8-bit data width
	
	// configuracion buzzer
	LPC_SYSCON ->SYSAHBCLKCTRL |= (1 << 8); // Enable Clock for TMR1
	LPC_IOCON ->PIO1_9 |= (1 << 0); // PIN1_9 = CT16B1_MAT0
	LPC_TMR16B1 ->MR0 = 0;
	LPC_TMR16B1 ->PR = 12000;
	LPC_TMR16B1 ->MR3 = 10; // Cycle Length
	LPC_TMR16B1 ->MCR |= (1 << 10); // TC Reset on MR3 Match
	LPC_TMR16B1 ->PWMC |= (1 << 0); // PWM Mode
	LPC_TMR16B1 ->TCR |= (1 << 0); // GO
	
	buzzer();
	
	delay_ms(2500);					//espera lo suficiente para que arranque el display LCD
	
	imprime("esto es vida... lo demas son",100,0);	
	delay_ms(100);
	limpia_pantalla();
	delay_ms(100);
	imprime("licenciados...",100,0);
	delay_ms(1000);
	limpia_pantalla();
	
}

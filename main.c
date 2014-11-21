//Sensor de Temperatura
//LPC1114FN28/102
//Universidad Galileo, Guatemala 2014
//Erick Braul, Sergio Saenz y Milton Godinez

//CREDITOS
// funcion delay : http://www.eevblog.com/forum/microcontrollers/lpc1114-28dip-tutorials/
// informacion calculo baud rate: csserver.evansville.edu/~blandfor/EE354/ARMLecture/ARMLect7.pdf

#include <LPC11xx.h>			/* LPC11xx Peripheral Registers */
#include "system_LPC11xx.h"
#include <stdio.h>


#define MASK(x) (1UL << (x))

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

void enciende_pantalla(){
	U0THR = 0xfe;						//comando escape 
	U0THR = 0x0c;						//habilita splash screen
}

void apaga_pantalla(){
	U0THR = 0xfe;						//comando escape 
	U0THR = 0x08;						//habilita splash screen
}

void enciende_cursor(){
	U0THR = 0xfe;						//comando escape 
	U0THR = 0x0d;						//habilita splash screen
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

void Init_ADC(void){
	LPC_SYSCON->PDRUNCFG &= ~(MASK(4));//Encender ADC
	LPC_SYSCON->SYSAHBCLKCTRL |= MASK(13);//Clock para ADC
	LPC_IOCON->R_PIO0_11 &= ~0x8F;//Resetear configuración de IO
	LPC_IOCON->R_PIO0_11 |= 0x0A;//Selección de modo análogo
	LPC_ADC->CR = (0x01<<0) | (0x0A<<8);//ADC0 con preescala
	LPC_ADC->CR &= (~MASK(16)) & (~MASK(17)) & (~MASK(24));//11 ciclos para el clock
	LPC_ADC->CR &= 0xF8FFFFFF;//Parar ADC
}

uint32_t readADC(){
   uint32_t regVal,ADC_data;
   LPC_ADC->CR |= (1<<24);//ADC listo para leer datos
   while(!(LPC_ADC->GDR & MASK(31)));//Espera a que el dato este listo
   regVal = LPC_ADC->DR[0];//Lee el dato del ADC
   LPC_ADC->CR &= 0xF8FFFFFF;//Paro al ADC
   ADC_data = (regVal>>6) & 0x3FF;
   return ADC_data;
}

void init_uart(){
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
}

void init_buzzer(){
	// configuracion buzzer
	LPC_SYSCON ->SYSAHBCLKCTRL |= (1 << 8); // Enable Clock for TMR1
	LPC_IOCON ->PIO1_9 |= (1 << 0); // PIN1_9 = CT16B1_MAT0
	LPC_TMR16B1 ->MR0 = 0;
	LPC_TMR16B1 ->PR = 12000;
	LPC_TMR16B1 ->MR3 = 10; // Cycle Length
	LPC_TMR16B1 ->MCR |= (1 << 10); // TC Reset on MR3 Match
	LPC_TMR16B1 ->PWMC |= (1 << 0); // PWM Mode
	LPC_TMR16B1 ->TCR |= (1 << 0); // GO
}

int main(){
	
	int mi_variable = 0;
	
	init_uart();
	//init_buzzer();	
	Init_ADC();
	
	delay_ms(2500);					//espera lo suficiente para que arranque el display LCD		
	
	imprime("si enciende...",100,0);
	delay_ms(500);	
	limpia_pantalla();
	
	while(1){
		
		mi_variable = readADC();
		
		imprime("entro al while",100,0);
		delay_ms(100);	
		limpia_pantalla();
		
		if((mi_variable < 500)){
			imprime("menor a 500",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 500) && (mi_variable < 510)){
			imprime("de 500 a 510",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 510) && (mi_variable < 520)){
			imprime("de 510 a 520",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 520) && (mi_variable < 530)){
			imprime("de 520 a 530",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 530) && (mi_variable < 540)){
			imprime("de 530 a 540",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 540) && (mi_variable < 550)){
			imprime("de 540 a 550",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 550) && (mi_variable < 560)){
			imprime("de 550 a 560",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 560) && (mi_variable < 570)){
			imprime("de 560 a 570",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 570) && (mi_variable < 580)){
			imprime("de 570 a 580",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 580) && (mi_variable < 590)){
			imprime("de 580 a 590",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 590) && (mi_variable < 600)){
			imprime("de 590 a 600",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 600) && (mi_variable < 700)){
			imprime("de 600 a 700",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 700) && (mi_variable < 800)){
			imprime("de 700 a 800",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 800) && (mi_variable < 900)){
			imprime("de 800 a 900",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 900) && (mi_variable < 1000)){
			imprime("de 900 a 1000",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 1000) && (mi_variable < 1100)){
			imprime("de 1000 a 1100",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 1100) && (mi_variable < 1200)){
			imprime("de 1100 a 1200",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 1200) && (mi_variable < 1300)){
			imprime("de 1200 a 1300",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 1300) && (mi_variable < 1400)){
			imprime("de 1300 a 1400",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
		
		if((mi_variable >= 1400)){
			imprime("mayor a 1400",100,0);
			delay_ms(2500);	
			limpia_pantalla();
		}
	}
	
}

#include "led.h"
#include <LPC21xx.H>

#define LED0_bm (1<<16)
#define LED1_bm (1<<17)
#define LED2_bm (1<<18)
#define LED3_bm (1<<19)

unsigned int uiLedReg = 0x0000, uiBitmask = 0x0000;

enum StepDirection {Left, Right};

void Led_Init(){
	IO1DIR = (IO1DIR | LED0_bm | LED1_bm | LED2_bm | LED3_bm);
	//IO1SET = LED0_bm;
}

void Led_On(unsigned char ucLedIndeks){
	IO1CLR = (LED0_bm | LED1_bm | LED2_bm | LED3_bm);
	switch(ucLedIndeks){
		case 0:
			IO1SET = LED0_bm;
			break;
		case 1:
			IO1SET = LED1_bm;
			break;
		case 2:
			IO1SET = LED2_bm;
			break;
		case 3:
			IO1SET = LED3_bm;
			break;
		default:
			break;
	}
}

void Led_Step(enum StepDirection Direction){
	static unsigned int uiPozycjaPunktu;
	switch(Direction){
		case Left:
			uiPozycjaPunktu = (++uiPozycjaPunktu) % 4;
			break;
		case Right:
			uiPozycjaPunktu = (--uiPozycjaPunktu) % 4;
			break;
		default:
			break;
	}
	Led_On(uiPozycjaPunktu);
}

void Led_StepLeft(){
	Led_Step(Left);
}

void Led_StepRight(){
	Led_Step(Right);
}

void Led_Toggle(unsigned char ucLedIndex){
	switch(ucLedIndex){
		case 0:
			uiBitmask = LED0_bm;
			break;
		case 1:
			uiBitmask = LED1_bm;
			break;
		case 2:
			uiBitmask = LED2_bm;
			break;
		case 3:
			uiBitmask = LED3_bm;
			break;
	}
	uiLedReg = uiLedReg ^ uiBitmask;
	IO1SET = uiLedReg;
	IO1CLR = ~uiLedReg;
}

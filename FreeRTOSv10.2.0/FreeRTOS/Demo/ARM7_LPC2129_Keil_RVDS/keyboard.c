#include "keyboard.h"
#include <LPC21xx.H>

#define BUTTON1_bm 1<<4
#define BUTTON2_bm 1<<6
#define BUTTON3_bm 1<<5
#define BUTTON4_bm 1<<7

void ButtonInit(void){
	IO0DIR=IO0DIR&(~(BUTTON1_bm|BUTTON2_bm|BUTTON3_bm|BUTTON4_bm));
}

enum KeyboardState eKeyboard_Read(void){
	
	if((IO0PIN&BUTTON1_bm) == 0){
		return(BUTTON_1);
	}
	if((IO0PIN&BUTTON2_bm) == 0){
		return(BUTTON_2);
	}
	if((IO0PIN&BUTTON3_bm) == 0){
		return(BUTTON_3);
	}
	if((IO0PIN&BUTTON4_bm) == 0){
		return(BUTTON_4);
	}
	return(RELASED);
}

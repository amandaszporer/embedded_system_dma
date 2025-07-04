#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430.h>  // MSP430x4xx
#include <stdint.h>

#define   debounceVal      250


// LCDs abstraction
// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL
#define LCD_CTL_SEL        P2SEL



//// PushButton 3 abstraction for Main Lab
//#define PB3sArrPort         P2IN
//#define PB3sArrIntPend      P2IFG
//#define PB3sArrIntEn        P2IE
//#define PB3sArrIntEdgeSel   P2IES
//#define PB3sArrPortSel      P2SEL
//#define PB3sArrPortDir      P2DIR
//#define PB3sArrPortOut      P2OUT

// PushButtons abstraction
#define PBsArrPort         P1IN
#define PBsArrIntPend      P1IFG
#define PBsArrIntEn        P1IE
#define PBsArrIntEdgeSel   P1IES
#define PBsArrPortSel      P1SEL
#define PBsArrPortDir      P1DIR
#define PBsArrPortOut      P1OUT
#define PB0                0x01   // P1.0
#define PB1                0x02  // P1.1
#define PB2                0x04  // P1.2
#define PB3                0x08  // P1.3

// Keypad abstraction
#define KeypadPortSel      P10SEL
#define KeypadPortDir      P10DIR
#define KeypadPortOut      P10OUT
#define KeypadPortIn       P10IN
#define KeypadIntPort      P2IN
#define KeypadIntPend      P2IFG
#define KeypadIntEn        P2IE
#define KeypadIntEdgeSel   P2IES
#define KeypadIntPortSel   P2SEL
#define KeypadIntPortDir   P2DIR
#define KeypadIntPortOut   P2OUT

extern void GPIOconfig(void);
extern void TIMERB_config_500ms(void);
extern void TIMERA_config_1s(void);
extern void ADCconfig(void);
//extern void DMA_config(void);
//extern void DMA_BLOCK_config(char* source_str, char* dest_str, int str_length);

#endif




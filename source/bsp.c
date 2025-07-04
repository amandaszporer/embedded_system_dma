#include  "header/bsp.h"    // private library - BSP layer
#include "header/api.h"

//-----------------------------------------------------------------------------  
//           GPIO configuration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;     // Stop WDT
   
  // LCD configuration
  LCD_DATA_WRITE &= ~0xFF; // clears all 8 bits of the data port, ensuring it starts from 0.
  LCD_DATA_DIR |= 0xF0;    //sets p1.4-p1.7 as outputs, because we'll send data to the lcd on those pins
  LCD_DATA_SEL &= ~0xF0;   // Bit clear P2.4-P2.7 - this assures this pins are on GPIO mode
  LCD_CTL_SEL  &= ~0xE0;   // Bit clear P2.5-P2.7 - this assures this pins are on GPIO mode

  // PushButtons Setup
  PBsArrPortSel &= ~0x07;           //
  PBsArrPortOut &= ~0x07;            // Set P1Out to '0'
  PBsArrPortDir &= ~0x07;            // P1.0-2 - Input ('0')
  PBsArrIntEdgeSel |= 0x03;          // pull-up mode   P1.0-P1.1 - '1'
  PBsArrIntEdgeSel &= ~0x0C;         // pull-down mode  P1.2 - '0'
  PBsArrIntEn |= 0x07;               // P1.0-2 - '1'
  PBsArrIntPend &= ~0xFF;            // clear pending interrupts P1.0-P1.3 all P1
  
     //PushButton 3 Setup For Main Lab
     PBsArrPortSel &= ~BIT3;
     PBsArrPortOut &= ~BIT3;
     PBsArrPortDir &= ~BIT3;
     PBsArrIntEn |= BIT3;
     PBsArrIntPend &= ~BIT3;


    _BIS_SR(GIE);                     // enable interrupts globally

  //Keypad Setup
  KeypadPortSel &= ~0xFF;
  KeypadPortDir = 0x0F;         // Set P10.0-P10.3 as inputs, P10.4-P10.7 as outputs
  KeypadPortOut = 0x00;         // Initialize outputs to 0

  //Keypad Setup
  KeypadIntPortSel &= ~BIT1;
  KeypadIntPortDir &= ~BIT1;
  KeypadIntEdgeSel |= BIT1;
  KeypadIntEn |= BIT1;
  KeypadIntPend &= ~0xFF;

  // PORT9 Setup - LEDs
  P9SEL &= ~0xFF;              // P9.0-P9.7 as GPIO
  P9DIR |= 0xFF;               // P9.0-P9.7 as outputs (LEDs)
  P9OUT = 0x00;

  _BIS_SR(GIE);                     // enable interrupts globally
}


//-------------------------------------------------------------------------------------
//            ADC configuration
//-------------------------------------------------------------------------------------
//void ADCconfig(void){
 //     ADC12CTL0 = ADC12SHT_2 + ADC12ON+ SREF_0 + ADC12IE;  // 16*ADCLK+ Turn on, set ref to Vcc and Gnd, and Enable Interrupt
      //ADC10SHT_2: Sets sample and hold time to 16 ADC10 clock cycles
      //ADC10ON: Turns on the ADC
      //SREF_0: Chooses V+ = Vcc and V- = GND as reference voltages
      //ADC10IE: Enables interrupt when the conversion finishes (ADC will trigger an ISR)
      //for summary- this line is powering the adc and telling it to interrupt us when it finishes converting.

//      ADC12CTL1 = INCH_3 + ADC12SSEL_3;           // Input channel A3 (P1.3), SMCLK source
      //INCH_3: Selects input channel A3, which maps to P1.3 Ãwhere the potentiometer is connected
      //ADC10SSEL_3: Selects SMCLK as the clock source for the ADC

//      ADC12AE0 |= BIT3;                           // Enable analog input on P1.3 (A3)
//}

//-------------------------------------------------------------------------------------
//            DMA Configuration for State3 - LED Pattern Transfer
//-------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------
//            Timer B configuration
//-------------------------------------------------------------------------------------

void TIMERB_config_500ms(void)
{
    TB0CTL   = TBSSEL_2 | ID_3 | MC_0 | TBCLR;  /* SMCLK /8, stop+clear */
    TB0CCR0  = 62500 - 1;                       /* 0.5 s period         */
    TB0CCR2  = 62500 - 1;                       /* CCR2 compare         */
    TB0CCTL2 = 0;                               /* compare mode         */
    TB0CTL  |= MC_1;                            /* Up-mode start        */
}

 void TIMERA_config_1s(void){
     TACCR0 = 4096 - 1;  // = 4095
     TACCTL0 = CCIE; // Enable CCR0 interrupt
     TA0CTL = TASSEL_1 + MC_1 + ID_2; // Up mode, ACLK, divide 4
     __bis_SR_register(LPM0_bits | GIE);
 }
//-------------------------------------------------------------------------------------

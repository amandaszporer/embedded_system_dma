#include  "header/halGPIO.h"     // private library - HAL layer
#include <string.h>

volatile unsigned char KB = '#';
int key_identifier_array[16] = {0};

volatile int current_key_index =-1;
volatile int current_key_press_count = 0;
volatile int key_index;

#define MAX_KEY_INDEX 15
#define MIN_KEY_INDEX 0;

const KeyConfig keypad[16] = {
    {{'F', 'Y', 'Z'}, 3},  // Key 0
    {{'B', 'W', 'X'}, 3},  // Key 1
    {{'0', 'U', 'V'}, 3},  // Key 2
    {{'A', 'S', 'T'}, 3},  // Key 3
    {{'E', 'R', '\0'}, 2}, // Key 4
    {{'9', 'Q', '\0'}, 2}, // Key 5
    {{'8', 'P', '\0'}, 2}, // Key 6
    {{'7', 'O', '\0'}, 2}, // Key 7
    {{'D', 'N', '\0'}, 2}, // Key 8
    {{'6', 'M', '\0'}, 2}, // Key 9
    {{'5', 'L', '\0'}, 2}, // Key 10
    {{'4', 'K', '\0'}, 2}, // Key 11
    {{'C', 'J', '\0'}, 2}, // Key 12
    {{'3', 'I', '\0'}, 2}, // Key 13
    {{'2', 'H', '\0'}, 2}, // Key 14
    {{'1', 'G', '\0'}, 2}  // Key 15
};

volatile unsigned long last_key_press_time = 0;


//--------------------------------------------------------------------
//             System Configuration
//--------------------------------------------------------------------
void sysConfig(void){
    GPIOconfig();
    lcd_init();
    lcd_clear();
}

//---------------------------------------------------------------------
//            Polling based Delay functions
//---------------------------------------------------------------------
void delay(unsigned int t){  //
    volatile unsigned int i;
    for(i=t; i>0; i--);
}


void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) __no_operation(); // the command asm("nop") takes roughly 1usec

}

void DelayMs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--)
        DelayUs(1000); // the command asm("nop") takes roughly 1usec

}

//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
    if (LPM_level == 0x00)
      _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01)
      _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02)
      _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
    else if(LPM_level == 0x03)
      _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04)
      _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}

//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  __no_operation();
  __no_operation();
  LCD_EN(0);
}

//---------------------------------------------------------------------
//              Print Mirrored String Driver
//---------------------------------------------------------------------
void print_reversed(char reversed[]){
    lcd_clear();
    lcd_home();
    int i = 0;
    while (reversed[i] != '\0'){
        if (i == 16 || i == 48){
           lcd_new_line;
        }
        else if (i == 32){
            TIMERA_config_1s();
            lcd_clear();
            lcd_home();
        }
        lcd_data(reversed[i]);
        i++;
    }
}

//---------------------------------------------------------------------
//              DMA Mirror String Driver
//---------------------------------------------------------------------
void dma_strMirror(char *const string, char *const reversed, unsigned int str_len){

    DMACTL0 |= DMA1TSEL_0;       // Software trigger for DMA1

    DMA0CTL = DMADT_1 +          // Block transfer mode
                  DMASRCINCR_2 +     // Source address decrement (for reversal)
                  DMADSTINCR_3 +     // Destination address increment
                  DMASRCBYTE +       // Source byte access
                  DMADSTBYTE +       // Destination byte access
                  DMAIE;

    DMA0CTL |= DMAEN;

    DMA0SA = (void(*))(string + str_len - 1); //Source: end of string (reverse)
    DMA0DA = (void(*))reversed;                      // Destination: start of output string
    DMA0SZ = str_len;                    // Transfer size: string length

    DMA0CTL |= DMAREQ;              // Software trigger for block transfer


    // Null terminate the result string
    reversed[str_len] = '\0';

}


//---------------------------------------------------------------------
//            Keypad Drivers
//---------------------------------------------------------------------

char get_key_character(int key_index, int press_count) {
    const KeyConfig* key = &keypad[key_index];
    //For keys with 2 options:
    if (key->num_options ==2) {
        return key->characters[press_count % 2];
    }
    //For keys with three options
    return key->characters[press_count % 3];
}

void reset_key_identifier_array(int k) {
    int i = 0;
    while (i < 16){
        if (i==k){
            i++;
        }
    current_key_index = -1;
    current_key_press_count = 0;
    }
}

void keypad_input(char *p){
    if (KB == '#') {
        __bis_SR_register(LPM0_bits + GIE); // Sleep until interrupt fires
    }
    *p = KB;
    KB = '#';
}

//---------------------------------------------------------------------
//            Port1 Interrupt Service Routine
//*********************************************************************//---------------------------------------------------------------------#pragma vector=PORT1_VECTOR
#pragma vector=PORT1_VECTOR  // For Push Buttons
  __interrupt void PBs_handler(void){
    delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
    if(PBsArrIntPend & PB0){
      state = state1;
      PBsArrIntPend &= ~PB0;
        }
        else if(PBsArrIntPend & PB1){
      state = state2;
      PBsArrIntPend &= ~PB1;
        }
    else if(PBsArrIntPend & PB2){
      state = state3;
      PBsArrIntPend &= ~PB2;
        }

    else if(PBsArrIntPend & PB3){
      state = state4;
      PBsArrIntPend &= ~PB3;
        }

//---------------------------------------------------------------------
//            Exit from a given LPM
//---------------------------------------------------------------------
        switch(lpm_mode){
        case mode0:
         LPM0_EXIT; // must be called from ISR only
         break;

        case mode1:
         LPM1_EXIT; // must be called from ISR only
         break;

        case mode2:
         LPM2_EXIT; // must be called from ISR only
         break;

        case mode3:
         LPM3_EXIT; // must be called from ISR only
         break;

        case mode4:
         LPM4_EXIT; // must be called from ISR only
         break;
    }
}

//*********************************************************************
//            Port2 Interrupt Service Routine
//*********************************************************************
#pragma vector = PORT2_VECTOR
__interrupt void kp_handler_P2(void)
{
    delay(debounceVal);
    int row, col;
    const uint8_t row_drive[4] = { 0x0E, 0x0D, 0x0B, 0x07 };
    const uint8_t col_mask [4] = { 0x10, 0x20, 0x40, 0x80 };
    if (KeypadIntPend & BIT1) {

        // Scan each row
        for (row = 0; row < 4; row++) {
            KeypadPortOut = row_drive[row];
            for (col = 0; col < 4; col++) {
                if ((KeypadPortIn & col_mask[col]) == 0) {
                    key_index = row * 4 + col;

                    if (state == state2) {
                        KB = get_key_character(key_index, 0);
                    }

                    else if (state == state1) {

                        if (current_key_index == -1) {
                            //First Press of a new key
                            current_key_index = key_index;
                            current_key_press_count = 1;
                            KB = get_key_character(key_index, 0);
                        }

                        else if (current_key_index == key_index){
                            current_key_press_count++;
                            KB = get_key_character(key_index, current_key_press_count-1);
                        }
                    }
                }
            }

        }

        KeypadIntPend &= ~0x02;
        KeypadPortOut &= ~0x0f;  // Reset rows
    }
    delay(debounceVal);
    // Exit from LPM based on current mode
    switch (lpm_mode) {
        case mode0: LPM0_EXIT; break;
        case mode1: LPM1_EXIT; break;
        case mode2: LPM2_EXIT; break;
        case mode3: LPM3_EXIT; break;
        case mode4: LPM4_EXIT; break;
    }
}


//---------------------------------------------------------------------
//            Timer A 1s ISR
//---------------------------------------------------------------------
#pragma vector = TIMERA0_VECTOR
__interrupt void TimerA_ISR(void){
    TA0CTL = MC_0;
    __bic_SR_register_on_exit(LPM0_bits); //leave LPM
}


//---------------------------------------------------------------------
//            Timer B 500ms ISR for DMA Trigger
//---------------------------------------------------------------------
#pragma vector = TIMERB1_VECTOR
__interrupt void TimerB_ISR(void) {
    __bic_SR_register_on_exit(LPM0_bits); // Wake up CPU after DMA completes
}
//---------------------------------------------------------------------
//            DMA ISR
//---------------------------------------------------------------------
#pragma vector = DMA_VECTOR
__interrupt void DMA_ISR(void)
{
    switch(DMAIV)
    {
        case DMAIV_DMA0IFG:
            DMA0CTL &= ~DMAIFG;
            break;

        case DMAIV_DMA1IFG:
            DMA1CTL &= ~DMAIFG;
            TB0CTL = 0;
            dma1_done = 1; // Signal that LED DMA sequence is done
            __bic_SR_register_on_exit(LPM0_bits);
            break;
    }
}


//--------------------------------------3-------------------------------
//            state3 configurations
//---------------------------------------------------------------------
#define LED_PORT_OUT P9OUT
#define LED_PORT_DIR P9DIR

unsigned char led_data[8] = {128,224,32,126,83,44,253,113};

void state_3_config(void) {
    P9DIR = 0xFF;
    P9OUT = 0x00; //clear leds
    P9SEL = 0x00;

    DMA1CTL &= ~DMAEN; // Disable before config
    DMA1SA = (unsigned int)led_data; //pointer to the source
    DMA1DA = (unsigned int)&LED_PORT_OUT; //
    DMA1SZ = 8;

    DMA1CTL = DMADT_0          // Single transfer mode
            | DMASRCINCR_3     // Increment source
            | DMADSTINCR_0     // Fixed destination
            | DMADSTBYTE
            | DMASRCBYTE;
            //| DMALEVEL;

    DMA1CTL |= DMAIE; // Enable DMA interrupt
    TB0CCTL2 &= ~CCIFG;
    // Trigger source = Timer_B CCR2’ value 30
    DMACTL0 = (DMACTL0 & ~0x00F0) | (2 << 4); // Manual assignment for DMA1TSEL_30

    DMA1CTL |= DMAEN; // Enable DMA
}



//---------------------------------------------------------------------
//            state4 configurations
//---------------------------------------------------------------------
char str1[]="An apple a day keeps the doctor away ";
char str2[]="Strike while the iron's hot";
volatile char str1_dest[];



void state_4_config(void) {


    DMA1CTL &= ~DMAEN; // Disable before config
    DMA1SA = (unsigned int)str1; //pointer to the source
    DMA1DA = (unsigned int) str1_dest; //
    DMA1SZ = 38;

    DMA2CTL &= ~DMAEN; // Disable before config
    DMA2SA =(void(*))str2; //pointer to the source
    DMA2DA = (void(*))(str1_dest + 36); //
    DMA2SZ = 27;

    DMA1CTL = DMADT_1 +          // Block transfer mode
                  DMASRCINCR_3 +     // Source address increment
                  DMADSTINCR_3 +     // Destination address increment
                  DMASRCBYTE +       // Source byte access
                  DMADSTBYTE +       // Destination byte access
                  DMAIE;

    DMA2CTL = DMADT_1 +          // Block transfer mode
                  DMASRCINCR_3 +     // Source address increment
                  DMADSTINCR_3 +     // Destination address increment
                  DMASRCBYTE +       // Source byte access
                  DMADSTBYTE +       // Destination byte access
                  DMAIE;


    DMA1CTL |= DMAIE; // Enable DMA interrupt
    DMA2CTL |= DMAIE; // Enable DMA interrupt


    // Trigger source = Timer_B CCR2’ value 30
 /*   DMACTL0 = (DMACTL0 & ~0X00F0) | (0<<4);     // Software trigger for DMA1
    DMACTL1 = (DMACTL1 & ~0X0F00) | (0XE<<8);
    DMA2CTL |= DMAEN;
    DMA1CTL |= DMAEN | DMAREQ; // Enable DMA */

    DMACTL0 |= DMA1TSEL_0 + DMA2TSEL_14;
    DMA2CTL |= DMAEN;
    DMA1CTL |= DMAEN | DMAREQ; // Enable DMA


}




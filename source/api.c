#include  "header/api.h"            // private library - API layer
#include  "header/halGPIO.h"     // private library - HAL layer
#include <stdint.h>
#include <stdio.h>


//-------------------------------------------------------------
//             String Mirroring using DMA Block Transfer
//-------------------------------------------------------------

void idiom_record(char* string) {
    char user_input;
    int k = 0;
    lcd_clear();
    lcd_home();
    lcd_puts("Type Idiom:"); //immidiately cleared
    TIMERA_config_1s();
    lcd_clear();
    lcd_home();

    while (state == state1) {
        char final = '#';
        enable_interrupts();
        keypad_input(&user_input);
        final = user_input;

        //Print first character immediately
        if (user_input != '#') {
            string[k] = final;
            if (k == 16){
                lcd_new_line;
            }
            lcd_data(final);
            k++;
            }
        // Open a 1 s window for more presses of the same key

        TIMERA_config_1s();           // sleep until 1 s is up

        keypad_input(&user_input);

        //IF there was a another press of the same

        if (user_input != '#'){
            if (current_key_press_count > 1) {
                final = user_input;
                string[k-1] = final;
                lcd_cursor_left();      //move cursor back
                lcd_data(final);
                }
            else {
                final = user_input;
                string[k] = final;
                lcd_data(final);
                k++;

            }
        }
        current_key_index = -1;
        current_key_press_count = 0;

        if (k >= 32) {
            lcd_clear();
            lcd_puts("Max length exceeded!");
            lcd_new_line;
            lcd_puts("Please try again!");
            TIMERA_config_1s();
            lcd_clear();
            break;
        }
    }
    string[k] = '\0';
    __bis_SR_register(LPM0_bits | GIE);

}



//-------------------------------------------------------------
//             String Mirroring using DMA Block Transfer
//-------------------------------------------------------------
void mirror_string(char const data_base[13][65], char reversed[65]){
    char string[65];
    char key;
    int index;

    while (1){
        enable_interrupts();
        lcd_clear();
        lcd_home();
        lcd_puts("Select Index:");
        keypad_input(&key);
        if ((key >= '0' && key <= '9') || key == 'A')
        {
            lcd_clear();
            break;
        }
        else if (key != '#')
        {
            lcd_clear();
            lcd_puts("Invalid Input");
            TIMERA_config_1s();
            lcd_clear();
         }
    }

    if (key >= '0' && key <= '9'){
        index = key - '0';          //key is string so we subtract the ascii value of 0 to get index number
        }

    else if (key == 'A'){
         index = 10;
        }

    strcpy(string, data_base[index]);
    int str_len = strlen(string);
    dma_strMirror(string, reversed, str_len);
    print_reversed(reversed);
    __bis_SR_register(LPM0_bits | GIE);

}

//-------------------------------------------------------------
//              print array
volatile unsigned char dma1_done = 0;

void print_array(void)
{
    dma1_done = 0;
    state_3_config();        /* Port-9 + DMA1 */
    TIMERB_config_500ms();   /* start Timer-B */

    while (!dma1_done)
        __bis_SR_register(LPM0_bits | GIE);

    TB0CTL = 0;              /* stop timer   */
    DMA1CTL &= ~DMAEN;

    state = state0;          /* back to idle */
}

//-------------------------------------------------------------
//              merge
//-------------------------------------------------------------

void string_merging (void)
{
    lcd_clear();
    lcd_home();
    state_4_config(); //result in str1_dest[]
    print_reversed(str1_dest);
    __bis_SR_register(LPM0_bits | GIE);

}



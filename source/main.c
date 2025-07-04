#include "header/api.h"  // private library - API layer
#include "header/app.h"  // private library - APP layer

enum FSMstate state;
enum SYSmode lpm_mode;
void main(void) {
    const char data_base[13][65] = {
        "An apple a day keeps the doctor away",
        "Give someone the benefit of the doubt",
        "Time flies when you're having fun",
        "He who pays the piper calls the tune",
        "The pen is mightier than the sword",
        "The pot calling the kettle black",
        "Wrap your head around something",
        "We'll cross that bridge when we come to it",
        "A bird in the hand is worth two in the bush",
        "Do unto others as you would have them do unto you",
        "You can catch more flies with honey than you can with vinegar",
        "eod"
    };
    char reversed[65];
    char idiom_recorder[32];
    state = state0;    // start in idle state on RESET
    lpm_mode = mode0;  // start in idle state on RESET
    sysConfig();
    lcd_init();
    lcd_clear();

    while (1) {
        switch (state) {
            case state0:
                enable_interrupts();
                enterLPM(lpm_mode);
                break;

            case state1:
                enable_interrupts();
                idiom_record(idiom_recorder);
                break;

            case state2:
                //clrLEDs();
                lcd_clear();
                mirror_string(data_base,reversed);
                break;

            case state3:
                print_array();
                break;

            case state4:
                string_merging();
                break;
        }
    }
}

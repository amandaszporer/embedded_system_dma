#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer


extern void idiom_record(char* string);
extern void mirror_string(char const data_base[][], char reversed[]);
extern void print_array(void);
extern void string_merging (void);
extern volatile unsigned char dma1_done;

#endif








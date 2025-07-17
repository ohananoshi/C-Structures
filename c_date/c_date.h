#ifndef C_DATE_H
    #define C_DATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct tagDate {
    int16_t year;
    uint8_t month;
    uint8_t day;
}date_t;

/**
 * @test
 * 
 * less maintenance and memory usage to timestamp validation
 * need len validation
 */
uint8_t _date_validate(const char *dt_str){
    int16_t year;
    uint8_t month, day, end; 

    sscanf(dt_str, "%5hd-%2hhu-%2hhu", &year, &month, &day);
    return ((year < -999 || year > 999) && month < 13 && day < 32);
}


#endif // C_DATE_H
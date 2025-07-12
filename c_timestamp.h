#ifndef C_TIMESTAMP_H
    #define C_TIMESTAMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

/**
 * @file c_timestamp.h
 * @brief Provides functions and types for timestamp manipulation and retrieval.
 *
 * This header defines the interface for working with timestamps, including
 * obtaining the current time, formatting timestamps, and performing time
 * calculations. It is designed to offer a portable and efficient way to
 * handle time-related operations in C programs.
 *
 * Typical functionalities may include:
 *   - Retrieving the current system time as a timestamp.
 *   - Formatting timestamps into human-readable strings.
 *   - Parsing strings into timestamp representations.
 *   - Performing arithmetic and comparison operations on timestamps.
 *
 * Usage:
 *   #include "c_timestamp.h"
 *
 * @author Guilherme Arruda - https://github.com/ohananoshi
 * @date 12 - July - 2025 [last modified:  12 - July - 2025]
 * @version 0.1
 * @attention no value limits validation yet.
 * @
 */

/**
 * @struct timestamp_t
 * @brief Represents a timestamp with separate fields for date and time components.
 *
 * This structure is used to store and manipulate date and time information.
 *
 * Members:
 *   - year: The year component of the timestamp.
 *   - month: The month component (1-12).
 *   - day: The day of the month (1-31).
 *   - hour: The hour component (0-23).
 *   - minute: The minute component (0-59).
 *   - second: The second component (0-59).
 *   - millisecond/microsecond: Sub-second precision.
 *   - tz_id: [UTC | GMT | UT | <area>/<city>]
 *   - tz_hour: Time Zone hour component (0-23).
 *   - tz_min: Time Zone minute component (0-59).
 *   - tz_sec: Time Zone The second component (0-59).
 */
typedef struct tagTimestamp{   
    int16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint32_t fractional_second;
    char *tz_id;
    int8_t tz_hour;    
    uint8_t tz_min;    
    uint8_t tz_sec;
}timestamp_t;

/**
 * Validates a timezone string and extracts its components.
 * @param tz_str The timezone string to validate.
 * @param dest_fmt Destination format string to store the result.
 * @return 1 if valid, 0 otherwise.
 */
uint8_t _timestamp_timezone_validate(const char* tz_str, char* dest_fmt){
   if(tz_str == NULL || strlen(tz_str) == 0){
        fprintf(stderr, "Error: Source string is NULL.\n");
        exit(1);
    }

    if(dest_fmt == NULL){
        fprintf(stderr, "Error: Destination format string is NULL.\n");
        exit(1);
    }

    if(!isupper(tz_str[0])){
        fprintf(stderr, "Error: Source string does not start with a valid character.\n");
        return 0;
    }

    //printf("start fmt: %s\n", dest_fmt);

    char tz_id[64], tz_fmt[20];
    uint8_t tz_blocks = 0;

    sscanf(tz_str, "%[a-zA-Z/]", tz_id);
    if(strcmp(tz_id, "UTC") == 0 || strcmp(tz_id, "GMT") == 0 || strcmp(tz_id, "UT") == 0){
        //printf("Timezone ID: %s\n", tz_id);

        if(strlen(tz_str + strlen(tz_id)) == 0){
            printf("error: No timezone offset provided.\n");
            return 0;
        }

        strcat(tz_fmt, "%[UTCGM]");
        tz_blocks++;
        
        char sig[2];
        sscanf(tz_str + strlen(tz_id), "%[+-]", sig);
        //printf("Timezone sign: %s\n", sig);

        char tz_offset[9];
        strcpy(tz_offset,tz_str + strlen(tz_id) + 1);
        
        uint8_t tz_offset_len = strlen(tz_offset);
        
        //printf("offset: %s offset len: %d\n", tz_offset, tz_offset_len);
        if(tz_offset_len == 0){
            printf("error: No timezone offset provided.\n");
            return 0;
        }

        char sep[2];
        if(sscanf(tz_offset, "%*[^:]%[:]", sep) > 0){
            //printf("Timezone separator: %s\n", sep);
            
            if(tz_offset_len == 5) strcat(tz_fmt, "%3hhd:%2hhu"); //printf("Timezone format: ±hh:mm\n");
            else if(tz_offset_len == 8) strcat(tz_fmt, "%3hhd:%2hhu:%2hhu"); //printf("Timezone format: ±hh:mm:ss\n");
            else{
                printf("Unknown timezone format.\n");
                return 0;
            }
        }
        else{
            //printf("Timezone separator: no separator\n");
            if(tz_offset_len <= 2) strcat(tz_fmt, "%3hhd"); //printf("Timezone format: ±hh\n");
            else if(tz_offset_len == 4) strcat(tz_fmt, "%3hhd%2hhu"); //printf("Timezone format: ±hhmm\n");
            else if(tz_offset_len == 6) strcat(tz_fmt, "%3hhd%2hhu%2hhu"); //printf("Timezone format: ±hhmmss\n");
            else printf("Unknown timezone format.\n");
        
        }
        
    }
    else if(strcmp(tz_id, "Z") == 0){ 
        if(strlen(tz_str + 1) > 0){
            printf("error: Not valid timezone offset.\n");
            return 0;
        }
        sprintf(tz_fmt, "Z");
    }
    else{
        sprintf(tz_fmt, "%%s");
        tz_blocks++;
    }

    strcat(dest_fmt, tz_fmt);

}

/**
 * @brief Validates the given timestamp.
 *
 * This function checks whether the provided timestamp value is valid
 * according to the expected format or range. It returns a boolean
 * indicating the validity of the timestamp.
 *
 * @param timestamp The timestamp value to validate.
 * @param dest_fmt Destination format string to store the result.
 * @return true if the timestamp is valid, false otherwise.
 */
uint8_t _timestamp_validate(const char *ts_str, char* dest_fmt){
    if(ts_str == NULL || strlen(ts_str) == 0){
        fprintf(stderr, "Error: Source string is NULL.\n");
        exit(1);
    }

    if(ts_str[0] != '-' && ts_str[0] != '+' && !isdigit(ts_str[0])){
        fprintf(stderr, "Error: Source string does not start with a valid character.\n");
        exit(1);
    }

    if(dest_fmt == NULL){
        fprintf(stderr, "Error: Destination format string is NULL.\n");
        exit(1);
    }

    uint8_t d_ver[] = {5,2,2,2,2,2,6,255}, i = 0, j = 0, k = 0, d_count = 0, len = strlen(ts_str), ts_zoneid_pos = 0;
    char sep_v[] = {'-','-',':',':','.'}, sep, ts_fmt[64];;

    //printf("len: %d\n", len);

    while(i < len){
        //printf("i: %d %c \n", i, ts_str[i]);
        if(isdigit(ts_str[i])){
            d_count++;
        }
        else if(ispunct(ts_str[i]) || ts_str[i] == 'T' || ts_str[i] == ' '){
            if(ts_str[i] == 'T' || ts_str[i] == ' ') sep = ts_str[i];
            if(i == 0);
            else if(i == len-1){
                printf("erro de formato\n");
                return 0;
            }
            else if(i > 0 && j <= 6){
                //printf("bloco %d -> d_count: %d, d_ver[%d]: %d ==> ", j, d_count, j, d_ver[j]);
                if(d_count <= d_ver[j]){
                    //printf("ok\n");
                    if(ts_str[i] != sep_v[k] && j != 2 && j != 6){
                        printf("erro: sep esperado: %c, encontrado: %c\n", sep_v[k], ts_str[i]);
                        exit(1);
                    }
                    j++;
                    k = j-1;
                    d_count = 0;
                    if(j > 6) i--;
                }
                else{
                    printf("erro: d_count: %d, d_ver[%d]: %d\n", d_count, j, d_ver[j]);
                    return 0;
                }
            }
        }
        else{
                //i--; // Decrement to re-evaluate the character
                //printf("zoneID: %s\n", ts_str + i);
                ts_zoneid_pos = i;
                break;
            }
        i++;
    }
    
    //printf("j: %d k: %d\n", j, k);

    if(j == 0 && i >= 4) sprintf(ts_fmt,"%%%dhd", d_ver[j]);
    else if(j > 0 && j < 3){
        sprintf(ts_fmt,"%%%dhd", d_ver[0]);
        for(i = 1; i <= j; i++) sprintf(ts_fmt+strlen(ts_fmt),"%c%%%dhhu", sep_v[i-1], d_ver[i]);
    }
    if(j >= 3 && j < 6){
        sprintf(ts_fmt,"%%%dhd", d_ver[0]);
        for(i = 1; i < 3; i++) sprintf(ts_fmt+strlen(ts_fmt),"%c%%%dhhu", sep_v[i-1], d_ver[i]);
        sprintf(ts_fmt+strlen(ts_fmt),"%c%%%dhhd", sep, d_ver[3]);
        for(i = 4; i < j; i++) sprintf(ts_fmt + strlen(ts_fmt),"%c%%%dhhu", sep_v[i-2], d_ver[i]);
    }
    else{
       sprintf(ts_fmt,"%%%dhd", d_ver[0]);
        for(i = 1; i < 3; i++) sprintf(ts_fmt+strlen(ts_fmt),"%c%%%dhhu", sep_v[i-1], d_ver[i]);
        sprintf(ts_fmt+strlen(ts_fmt),"%c%%%dhhd", sep, d_ver[3]);
        for(i = 4; i < j; i++) sprintf(ts_fmt + strlen(ts_fmt),"%c%%%dhhu", sep_v[i-2], d_ver[i]);
        sprintf(ts_fmt + strlen(ts_fmt),".%%06u");
    }
    //printf("tz: %s\n", ts_str + ts_zoneid_pos);
    char tz_fmt[64] = {""};
    if(ts_zoneid_pos > 0) if(_timestamp_timezone_validate(ts_str + ts_zoneid_pos, tz_fmt) == 0){
        printf("erro: timezone inválido\n");
        exit(1);
    }
    if(strlen(tz_fmt) > 0) strcat(ts_fmt, tz_fmt);
    strcpy(dest_fmt, ts_fmt);
    //printf("ts_fmt: %s\n", ts_fmt);

    return 1;
}

/**
 * @brief Parses a timestamp string and sets the fields of a not null timestamp_t structure.
 *
 * This function parses the given timestamp string according to supported formats.
 * '[+|-]yyyy[...]' |
 * '[+|-]yyyy[...]-[m]m' |
 * '[+|-]yyyy[...]-[m]m-[d]d' |
 * '[+|-]yyyy[...]-[m]m-[d]d ' |
 * '[+|-]yyyy[...]-[m]m-[d]d[T][h]h[:]' |
 * '[+|-]yyyy[...]-[m]m-[d]d[T][h]h:[m]m[:]' |
 * '[+|-]yyyy[...]-[m]m-[d]d[T][h]h:[m]m:[s]s[.]' |
 * '[+|-]yyyy[...]-[m]m-[d]d[T][h]h:[m]m:[s]s.[ms][ms][ms][us][us][us][zoneId]'
 *
 * @param ts Pointer to the timestamp_t structure to populate.
 * @param ts_str The input timestamp string to parse.
 */
void set_timestamp(timestamp_t *ts, const char *ts_str){

    if(ts == NULL || ts_str == NULL || strlen(ts_str) == 0){
        fprintf(stderr, "Error: Timestamp or source string is NULL.\n");
        exit(1);
    }

    char fmt[64], zone_id[64] = "";
    int16_t ts_year = 0;
    uint32_t ts_fraction = 0;
    uint8_t ts_month = 0, ts_day = 0, ts_minute = 0, ts_second = 0, tz_minute = 0, tz_second = 0;
    int8_t ts_hour = 0, tz_hour = 0;
    
    if(_timestamp_validate(ts_str, fmt)){
        sscanf(ts_str, fmt, &ts_year, &ts_month, &ts_day, &ts_hour, &ts_minute, &ts_second, &ts_fraction, zone_id, &tz_hour, &tz_minute, &tz_second);
        //printf("--> %+5d-%02d-%02dT%02d:%02d:%02d.%06d%s%+02d:%02d:%02d\n", ts_year, ts_month, ts_day, ts_hour, ts_minute, ts_second, ts_fraction, zone_id, tz_hour, tz_minute, tz_second);
        
        ts->year = ts_year;
        ts->month = ts_month;
        ts->day = ts_day;
        ts->hour = ts_hour;
        ts->minute = ts_minute;
        ts->second = ts_second;
        ts->fractional_second = ts_fraction;
        ts->tz_hour = tz_hour;
        ts->tz_min = tz_minute;
        ts->tz_sec = tz_second;

        ts->tz_id = (char*)calloc(strlen(zone_id), sizeof(char));
        strcpy(ts->tz_id,zone_id);
    }
    else{
        printf("erro\n");
        exit(1);
    }
}

/**
 * @brief Frees the memory allocated for a timestamp structure.
 *
 * This function releases all resources associated with the given timestamp pointer.
 * After calling this function, the pointer should not be used unless it is reassigned.
 *
 * @param ts Pointer to the timestamp structure to be freed. If NULL, no action is taken.
 */
uint8_t free_timestamp(timestamp_t *ts){
    if(ts != NULL){
        if(ts->tz_id != NULL) free(ts->tz_id);
        free(ts);
    }
}

/**
 * @brief Returns the current timestamp as a formatted string.
 *
 * This function retrieves the current system time and formats it as a string
 * in the "YYYY-MM-DD HH:MM:SS" format. The returned string is dynamically
 * allocated and should be freed by the caller to avoid memory leaks.
 *
 * @return A pointer to a null-terminated string containing the formatted timestamp,
 *         or NULL if memory allocation fails.
 */
char* get_timestamp_str(const timestamp_t ts){
    char *ts_str = (char*)calloc(64, sizeof(char));
    sprintf(ts_str, "%+d-%02d-%02dT%02d:%02d:%02d.%06d%s%+02d:%02d:%02d\n",
                    ts.year,
                    ts.month,
                    ts.day,
                    ts.hour,
                    ts.minute,
                    ts.second,
                    ts.fractional_second,
                    ts.tz_id ? ts.tz_id : "",
                    ts.tz_hour,
                    ts.tz_min,
                    ts.tz_sec
                );

    return ts_str;
}

#endif // C_TIMESTAMP_H
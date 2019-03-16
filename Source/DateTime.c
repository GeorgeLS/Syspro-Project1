#include <time.h>
#include "../Include/DateTime.h"
#include "../Include/Macros.h"

DateTime DateTime_Create_New(void) {
    time_t current_time = time(NULL);
    struct tm tm = *localtime(&current_time);
    return (DateTime) {
            .day = tm.tm_mday,
            .month = tm.tm_mon + 1,
            .year = tm.tm_year + 1900,
            .hour = tm.tm_hour,
            .minute = tm.tm_min
    };
}

static __INLINE__
DateTime DateTime_Create(int day, int month, int year, int hour, int minute) {
    return (DateTime) {
            .day = day,
            .month = month,
            .year = year,
            .hour = hour,
            .minute = minute
    };
}

DateTime DateTime_Create_From_String(const char *restrict date_str, const char *restrict time_str) {
    int day = 0;
    int month = 0;
    int year = 0;
    int hour = 0;
    int minute = 0;
    char *end;
    char date_delim = '-';
    char time_delim = ':';
    size_t i = 0U;
    if (date_str) {
        char *_date_str = SAFE_STR_COPY(date_str);
        while (_date_str[i] != '\0') {
            if (_date_str[i] == date_delim) _date_str[i] = ' ';
            ++i;
        }
        day = (int) strtol(_date_str, &end, 10);
        month = (int) strtol(end, &end, 10);
        year = (int) strtol(end, &end, 10);
        free(_date_str);
    }
    if (time_str) {
        char *_time_str = SAFE_STR_COPY(time_str);
        i = 0U;
        while (_time_str[i] != '\0') {
            if (_time_str[i] == time_delim) _time_str[i] = ' ';
            ++i;
        }
        hour = (int) strtol(_time_str, &end, 10);
        minute = (int) strtol(end, NULL, 10);
        free(_time_str);
    }
    return DateTime_Create(day, month, year, hour, minute);
}

static time_t DateTime_To_Seconds(const DateTime *dateTime) {
    struct tm storage = {0};
    storage.tm_year = dateTime->year + 1970;
    storage.tm_mon = dateTime->month - 1;
    storage.tm_mday = dateTime->day;
    storage.tm_hour = dateTime->hour;
    storage.tm_min = dateTime->minute;
    time_t res = mktime(&storage);
    return (res < 0) ? 0 : res;
}

int8_t DateTime_Compare(const DateTime *dt1, const DateTime *dt2) {
    time_t d1 = DateTime_To_Seconds(dt1);
    time_t d2 = DateTime_To_Seconds(dt2);
    return (int8_t) ((d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0));
}
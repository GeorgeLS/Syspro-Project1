#ifndef EXERCISE_I_DATETIME_H
#define EXERCISE_I_DATETIME_H

#include <stdint.h>
#include "Attributes.h"

typedef struct DateTime {
    int day;
    int month;
    int year;
    int hour;
    int minute;
} DateTime;

DateTime DateTime_Create_New(void);

DateTime DateTime_Create_From_String(const char *restrict date_str, const char *restrict time_str);

int8_t DateTime_Compare(const DateTime *dt1, const DateTime *dt2) __NON_NULL__(1, 2);

#endif //EXERCISE_I_DATETIME_H

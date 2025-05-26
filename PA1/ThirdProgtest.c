#ifndef __PROGTEST__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct
{
    int m_TotalDays;
    int m_WorkDays;
} TResult;
#endif /* __PROGTEST__ */
#include <stdbool.h>

bool is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0 && year % 4000 != 0);
}

int day_of_week(int y, int m, int d) {
    int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

bool check_data( int y, int m, int d ){
    if (y < 2000 || m < 1 || m > 12 || d < 1 || d > 31) {
        return false;
    }

    if (m == 2 && d > 29) {
        return false;
    }

    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) {
        return false;
    }

    if (m == 2 && d == 29 && !is_leap_year(y)) {
        return false;
    }

    return true;
}

bool isWorkDay( int y, int m, int d )
{
    if(!check_data(y, m, d))
        return false;
    if(m==1&&d==1)
        return false;
    if(m==5&&(d==8||d==1))
        return false;
    if(m==7&&(d==5||d==6))
        return false;
    if(m==9&&(d==28))
        return false;
    if(m==10&&d==28)
        return false;
    if(m==11&&d==17)
        return false;
    if(m==12&&(d==24||d==25||d==26))
        return false;


    int day = day_of_week(y, m, d);

    if(day <= 5 && day >=1)
        return true;

    return false;
}

int count_days_in_month(int y, int m){
    int days = 0;
    if(m == 4 || m == 6 || m == 9 || m == 11)
        days = 30;
    else if(m == 2 && is_leap_year(y))
        days = 29;
    else if(m == 2 && !is_leap_year(y))
        days = 28;
    else
        days = 31;
    return days;
}

int count_days_to_plus(int y, int m, int max_m){
    int days_total = 0;
    for(int i = m; i <= max_m; i++){
        days_total += count_days_in_month(y,i);
    }
    return days_total;
}

int countValidHolidays(int year1, int month1, int day1, int year2, int month2, int day2, int holidaysCount) {
    int count = 0;
    int min = day1;
    for(int i = 1; i < month1; i++){
        min+= count_days_in_month(year1, i);
    }

    int max = day2;
    for(int i = 1; i < month2; i++){
        max += count_days_in_month(year1, i);
    }
    int month_of_holidays[11]={1,5,5,7,7,9,10,11,12,12,12};
    int day_of_holidays[11]={1,1,8,5,6,28,28,17,24,25,26};
    if(!is_leap_year(year1)){
        int holidays[11]={1, 121, 128, 186, 187, 271, 301, 321, 358, 359, 360};
        for(int i = 0; i < 11; i++){
            if(min <= holidays[i] && max >= holidays[i] && day_of_week(year1, month_of_holidays[i], day_of_holidays[i])!=0 && day_of_week(year1, month_of_holidays[i], day_of_holidays[i])!=6){
                count++;
            }
        }
    }
    else{
        int holidays[11]={1, 122, 129, 187, 188, 272, 302, 322, 359, 360, 361};
        for(int i = 0; i < 11; i++){
            if(min <= holidays[i] && max >= holidays[i] && day_of_week(year1, month_of_holidays[i], day_of_holidays[i])!=0 && day_of_week(year1, month_of_holidays[i], day_of_holidays[i])!=6){
                count++;
            }
        }
    }

    return count;
}

int count_weekend_days(int startDay, int year1, int month1, int day1, int year2, int month2, int day2) {
    int totalDays = 0;
    int count = 0;
    int day = startDay;

    while (year1 < year2 || (year1 == year2 && month1 < month2) || (year1 == year2 && month1 == month2 && day1 <= day2)) {
        totalDays++;
        if (day == 0 || day == 6) {
            count++;
        }
        if (day == 6)
            day = 0;
        else
            day++;

        int daysInMonth = count_days_in_month(year1, month1);
        if (day1 < daysInMonth) {
            day1++;
        } else {
            day1 = 1;
            if (month1 < 12) {
                month1++;
            } else {
                month1 = 1;
                year1++;
            }
        }
    }
    return count;
}
TResult countDays ( int y1, int m1, int d1,
                    int y2, int m2, int d2 )
{
    TResult res;
    int days_total = 0;
    int days_work = 0;
    if(!check_data(y1, m1, d1) || !check_data(y2, m2, d2) || (y1 > y2) || ((y1 == y2) && (m1 > m2)) || ((y1 == y2) && (m1 == m2) && (d1 > d2))){
        res.m_TotalDays = -1;
        res.m_WorkDays = -1;
        return res;
    }

    if(m1 == 4 || m1 == 6 || m1 == 9 || m1 == 11)
        days_total += (30 - (d1-1));
    else if(m1 == 2 && is_leap_year(y1))
        days_total += (29 - (d1-1));
    else if(m1 == 2 && !is_leap_year(y1))
        days_total += (28 - (d1-1));
    else
        days_total += (31 - (d1-1));

    if(y1 == y2){
        days_total += count_days_to_plus(y1, m1 + 1, m2 - 1);
        if(m1!=m2)
            days_total += d2;
        else
            days_total -= (count_days_in_month(y2, m2) - d2);
        int weekend_count = 0;
        weekend_count += count_weekend_days(day_of_week(y1,m1,d1),y1, m1, d1, y1, m2, d2);
        weekend_count += countValidHolidays(y1, m1, d1, y1, m2, d2, 11);
        days_work = days_total - weekend_count;
    }
    else{
        days_total += count_days_to_plus(y1, m1 + 1, 12);
        days_total += count_days_to_plus(y2, 1 , m2 - 1);

        days_total += d2;

        int weekend_count = 0;

        weekend_count += count_weekend_days(day_of_week(y1,m1,d1),y1, m1, d1, y1, 12, 31);
        weekend_count += countValidHolidays(y1, m1, d1, y1, 12, 31, 11);

        weekend_count += count_weekend_days(day_of_week(y2,1,1),y2, 1, 1, y2, m2, d2);
        weekend_count += countValidHolidays(y2, 1, 1, y2, m2, d2, 11);

        for(int i = y1 + 1;  i < y2; i++){
            if(day_of_week(i,1,1) == 1 && day_of_week(i, 12, 31) == 1){
                days_total+=365;
                weekend_count += (365-252);
            }
            else if(day_of_week(i,1,1) == 2 && day_of_week(i, 12, 31) == 2){
                days_total+=365;
                weekend_count += (365-253);
            }
            else if(day_of_week(i,1,1) == 3 && day_of_week(i, 12, 31) == 3){
                days_total+=365;
                weekend_count += (365-253);
            }
            else if(day_of_week(i,1,1) == 4 && day_of_week(i, 12, 31) == 4){
                days_total+=365;
                weekend_count += (365-252);
            }
            else if(day_of_week(i,1,1) == 5 && day_of_week(i, 12, 31) == 5){
                days_total+=365;
                weekend_count += (365-254);
            }
            else if(day_of_week(i,1,1) == 6 && day_of_week(i, 12, 31) == 6){
                days_total+=365;
                weekend_count += (365-254);
            }
            else if(day_of_week(i,1,1) == 0 && day_of_week(i, 12, 31) == 0){
                days_total+=365;
                weekend_count += (365-252);
            }
            else if(day_of_week(i,1,1) == 1 && day_of_week(i, 12, 31) == 2){
                days_total+=366;
                weekend_count += (366-254);
            }
            else if(day_of_week(i,1,1) == 2 && day_of_week(i, 12, 31) == 3){
                days_total+=366;
                weekend_count += (366-254);
            }
            else if(day_of_week(i,1,1) == 3 && day_of_week(i, 12, 31) == 4){
                days_total+=366;
                weekend_count += (366-253);
            }
            else if(day_of_week(i,1,1) == 4 && day_of_week(i, 12, 31) == 5){
                days_total+=366;
                weekend_count += (366-255);
            }
            else if(day_of_week(i,1,1) == 5 && day_of_week(i, 12, 31) == 6){
                days_total+=366;
                weekend_count += (366-254);
            }
            else if(day_of_week(i,1,1) == 6 && day_of_week(i, 12, 31) == 0){
                days_total+=366;
                weekend_count += (366-252);
            }
            else if(day_of_week(i,1,1) == 0 && day_of_week(i, 12, 31) == 1){
                days_total+=366;
                weekend_count += (366-253);
            }
        }

        days_work = days_total - weekend_count;
    }
    res.m_TotalDays = days_total;
    res.m_WorkDays = days_work;
    return res;
}

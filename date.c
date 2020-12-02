#include "date.h"
#include <stdbool.h>
#include <stdlib.h>

struct Date_t{
    int day;
    int month;
    int year;
};

static bool is_date_legal(int day, int month){
    bool day_legal=(day>=1)&&(day<=30);
    bool month_legal=(month>=1)&&(month<=12);
    return (month_legal&&day_legal);
}

Date dateCreate(int day, int month, int year){
    if(!is_date_legal(day,month)){
        return NULL;
    }
    Date date=malloc(sizeof(Date));
    if(date==NULL){
        return NULL;
    }
    date->day=day;
    date->month=month;
    date->year=year;
    return date;
}

void dateDestroy(Date date){
    free(date);
}

Date dateCopy(Date date){
    if(date==NULL){
        return NULL;
    }
    return dateCreate(date->day,date->month,date->year);
}
bool dateGet(Date date, int* day, int* month, int* year){
    if(date==NULL||day==NULL||month==NULL||year==NULL){
        return false;
    }
    *day=date->day;
    *month=date->month;
    *year=date->year;
    return true;
}

int dateCompare(Date date1, Date date2){
    if (date1->year<date2->year){
        return -1;
    }
    else if (date1->year>date2->year){
        return 1;
    }
    else{
        if (date1->month<date2->month){
            return -1;
        }
        else if (date1->month>date2->month){
            return 1;
        }
        else{
            if (date1->day<date2->day){
                return -1;
            }
            else if (date1->day>date2->day){
                return 1;
            }
            else{
                return 0;
            }
        }
    }
}

void dateTick(Date date){
    date->day+=1;
    if(date->day>30){
        date->month+=1;
        date->day=1;
        if(date->month>12){
            date->year+=1;
            date->month=1;
        }
    }
}





#include "date.h"
#include <stdbool.h>
#include <stdlib.h>
#define POS_NUM 1
#define NEG_NUM -1 
#define MAX_DAY 30
#define MIN_DAY 1
#define MIN_MONTH 1
#define MAX_MONTH 12
struct Date_t{
    int day;
    int month;
    int year;
};

static bool isDateLegal(int day, int month){
    bool day_legal=(day>=MIN_DAY)&&(day<=MAX_DAY);
    bool month_legal=(month>=MIN_MONTH)&&(month<=MAX_MONTH);
    return (month_legal&&day_legal);
}

Date dateCreate(int day, int month, int year){ 
    if(!isDateLegal(day,month)){
        return NULL;
    }
    Date date=malloc(sizeof(struct Date_t));
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
    if(date1==NULL||date2==NULL){
        return 0;
    }
    if (date1->year<date2->year){
        return NEG_NUM;
    }
    else if (date1->year>date2->year){
        return POS_NUM;
    }
    else{
        if (date1->month<date2->month){
            return NEG_NUM;
        }
        
        else if (date1->month>date2->month){
            return POS_NUM;
        }
        else{
            if (date1->day<date2->day){
                return NEG_NUM;
            }
            else if (date1->day>date2->day){
                return POS_NUM;
            }
            else{
                return 0;
            }
        }
    }
}

void dateTick(Date date){
    date->day+=1;
    if(date->day>MAX_DAY){
        date->month+=1;
        date->day=1;
        if(date->month>MAX_MONTH){
            date->year+=1;
            date->month=1;
        }
    }
}





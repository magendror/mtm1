typedef enum Months_t {
    MONTHS,JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC,MONTHS_NUM
} Months;

struct Date{
    int date;
    Months month;
    int year;
};
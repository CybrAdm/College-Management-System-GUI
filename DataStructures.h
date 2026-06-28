#pragma once
#include <QString>

const int SIZE      = 100;
const int NOTIF_SIZE= 1000;

extern long long int stuIDCounter;
extern long long int stfIDCounter;
extern long long int admIDCounter;
extern int           notifTimestamp;

struct information {
    long long int id = 0;
    QString name;
    QString mobileNumber;
    QString password;
    QString address;
};

struct gradeScheme {
    float total       = 0;
    float finalMax    = 0;
    float practicalMax= 0;
    float yearWorkMax = 0;
    float quizMax     = 0;
};

struct grade {
    float total       = 0;
    float final_      = 0;
    float practical   = 0;
    float yearWork    = 0;
    float quiz        = 0;
    float percentage  = 0;
    QString letter;
    bool gradeAssigned= false;
};

struct enrollment {
    int studentIndex[SIZE] = {};
    grade g[SIZE];
};

struct courseSchedule {
    int  weekDay = 0;
    int  hour    = 0;
    int  minute  = 0;
    bool isSet   = false;
};

struct course {
    QString name;
    int creditHours  = 0;
    int academicYear = 0;
    gradeScheme gr;
    QString academicMemberName[5];
    int studentCount = 0;
    enrollment enr;
    courseSchedule schedule;
};

struct student {
    information inf;
    QString department;
    int academicYear = 0;
    QString studentCourses[10];
    float couTotalMarks = 0;
    float totalMarks    = 0;
    float percentage    = 0;
    float GPA           = 0;
    QString letter;
};

struct staff {
    information inf;
    QString department;
    QString position;
    QString teachingCourses[10];
    int stfNumOfCou = 0;
};

struct administration {
    information inf;
    QString position;
};

struct schedule {
    int day    = 0;
    int month  = 0;
    int year   = 0;
    int hour   = 0;
    int minute = 0;
};

struct exam {
    schedule sch;
    course   co;
};

struct appointment {
    long long int studentID = 0;
    QString studentName;
    int     stfIndex        = -1;
    QString stfName;
    QString status          = "Pending";
    schedule sch;
};

struct notification {
    long long int userId   = 0;
    int           userType = -1;
    QString       text;
    QString       icon;
    QString       color;
    bool          isRead   = false;
    int           timestamp= 0;
};

struct AppData {
    student        stu[SIZE];
    staff          stf[SIZE];
    administration adm[SIZE];
    course         cou[SIZE];
    exam           exm[SIZE];
    appointment    app[SIZE];

    notification   notif[NOTIF_SIZE];

    int numOfStu  = 0;
    int numOfStf  = 0;
    int numOfAdm  = 0;
    int numOfCou  = 0;
    int numOfApp  = 0;
    int numOfExm  = 0;
    int numOfNotif= 0;

    int loggedInIndex = -1;
    int loggedInType  = -1;
};

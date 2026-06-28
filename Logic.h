#pragma once
#include "DataStructures.h"

void initData(AppData& d);
void loadCourses(AppData& d);

bool isMobileTaken(const QString& phone, AppData& d);

void calculateGrade(int couIdx, grade& g, AppData& d);
float getGradePoint(const QString& letter);
void getGPAletter(int stuIndex, AppData& d);
void getTotalMarks(int stuIndex, AppData& d);
void calculateGPA(int stuIndex, AppData& d, bool showOutput = false);

QString getDayName(int weekDay);

void addNotification(long long int userId, int userType,
                     const QString& text,
                     const QString& icon,
                     const QString& color,
                     AppData& d);

void saveData(const AppData& d);
bool loadData(AppData& d);

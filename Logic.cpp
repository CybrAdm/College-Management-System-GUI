#include "Logic.h"

#include <QFile>
#include <QDataStream>

long long int stuIDCounter  = 100012;
long long int stfIDCounter  = 200006;
long long int admIDCounter  = 300002;
int           notifTimestamp= 0;

void initData(AppData& d) {
    d.stu[0]  = {{100000,"Adam Ahmed Abdelbaset Ahmed","01116622100","123","Tawheed Street, Helwan"},"SWE",1,{"Calculus I","Calculus II"}};
    d.stu[1]  = {{100001,"Yousef Allam Abdullah Tayel","01103193182","123","Gamal Abdelnasser Axis, 5th Settlement"},"SWE",1,{"Calculus I","Calculus II"}};
    d.stu[2]  = {{100002,"Yousef Ehab Makram Armeia","01206747635","123","Ahmed Esmat, Ain Shams"},"SWE",1};
    d.stu[3]  = {{100003,"Salah Eldin Ibrahim Salah Eldin","01003083350","123","5th Settlement"},"AI",2,{"Linear Algebra","Introduction to CS"}};
    d.stu[4]  = {{100004,"Malek Mohammed Abdullah Elshazly","01227247771","123","Tanta"},"AI",2,{"Linear Algebra","Introduction to CS"}};
    d.stu[5]  = {{100005,"Philopater Farag Nabih Farag","01097113849","123","Zayton"},"AI",2};
    d.stu[6]  = {{100006,"Marwan Mohammed Abed","01012112768","123","AlNakheel, 1st Settlement"},"CSEC",3,{"Data Structures","Discrete Math"}};
    d.stu[7]  = {{100007,"Karim Mohammed Saleh","01234567890","123","Rehab"},"CSEC",3,{"Data Structures","Discrete Math"}};
    d.stu[8]  = {{100008,"Ahmed Karim Reda","01227727654","123","Obour"},"CSEC",3};
    d.stu[9]  = {{100009,"Micheal George Maged","01221723032","123","Korba"},"Robotics",4,{"AI","SWE"}};
    d.stu[10] = {{100010,"Mohammed Salah Abdullah","01027823966","123","Shebin ElKom"},"Robotics",4,{"AI","SWE"}};
    d.stu[11] = {{100011,"Raneem Ahmed Mostafa","01275553141","123","Benha"},"Robotics",4};
    d.numOfStu = 12;

    d.stf[0] = {{200000,"Hassan Ramadan","01098765432","256"},"Information Systems","Professor"};
    d.stf[1] = {{200001,"Ghada Hamed","01087654321","256"},"Computer Science","Lecturer"};
    d.stf[2] = {{200002,"Naglaa Fathy","01098765433","256"},"Information Systems","Lecturer"};
    d.stf[3] = {{200003,"Doaa Ezzat","01098765431","256"},"Information Systems","Lecturer"};
    d.stf[4] = {{200004,"Eslam Sharshar","01087654328","256"},"Computer Science","Teaching Assistant"};
    d.stf[5] = {{200005,"Habiba Hegazy","01087654329","256"},"Computer Science","Teaching Assistant"};
    d.numOfStf = 6;

    d.adm[0] = {{300000,"Mohammed Ayad","01122334455","256"},"Supervisor"};
    d.adm[1] = {{300001,"Hala Moushir","01133445566","256"},"Supervisor"};
    d.numOfAdm = 2;

    d.cou[0]  = {"Calculus I",3,1,{100,60,0,30,10}};
    d.cou[1]  = {"Calculus II",3,1,{100,60,0,30,10}};
    d.cou[2]  = {"Physics I",3,1,{100,50,15,25,10}};
    d.cou[3]  = {"Introduction to CS",3,2,{100,60,0,30,10}};
    d.cou[4]  = {"Linear Algebra",3,2,{100,60,0,30,10}};
    d.cou[5]  = {"English I",2,2,{100,60,0,30,10}};
    d.cou[6]  = {"Data Structures",3,3,{100,60,10,20,10}};
    d.cou[7]  = {"Discrete Math",3,3,{100,60,0,30,10}};
    d.cou[8]  = {"Database Systems",3,3,{100,55,15,20,10}};
    d.cou[9]  = {"OOP",3,4,{100,60,10,20,10}};
    d.cou[10] = {"SWE",3,4,{100,60,0,30,10}};
    d.cou[11] = {"AI",3,4,{100,55,15,20,10}};
    d.numOfCou = 12;

    d.cou[0].enr.studentIndex[0]=0; d.cou[0].enr.g[0]={87,52,0,25,10,87,"B+",true};
    d.cou[0].enr.studentIndex[1]=1; d.cou[0].enr.g[1]={73,43,0,22,8,73,"C+",true};
    d.cou[0].studentCount=2;
    d.cou[1].enr.studentIndex[0]=0; d.cou[1].enr.g[0]={91,55,0,27,9,91,"A-",true};
    d.cou[1].enr.studentIndex[1]=1; d.cou[1].enr.g[1]={65,38,0,19,8,65,"D",true};
    d.cou[1].studentCount=2;
    d.cou[4].enr.studentIndex[0]=3; d.cou[4].enr.g[0]={95,58,0,27,10,95,"A+",true};
    d.cou[4].enr.studentIndex[1]=4; d.cou[4].enr.g[1]={78,46,0,23,9,78,"B-",true};
    d.cou[4].studentCount=2;
    d.cou[3].enr.studentIndex[0]=3; d.cou[3].enr.g[0]={82,50,0,24,8,82,"B",true};
    d.cou[3].enr.studentIndex[1]=4; d.cou[3].enr.g[1]={61,36,0,17,8,61,"D",true};
    d.cou[3].studentCount=2;
    d.cou[6].enr.studentIndex[0]=6; d.cou[6].enr.g[0]={89,54,8,18,9,89,"B+",true};
    d.cou[6].enr.studentIndex[1]=7; d.cou[6].enr.g[1]={70,41,6,15,8,70,"C",true};
    d.cou[6].studentCount=2;
    d.cou[7].enr.studentIndex[0]=6; d.cou[7].enr.g[0]={94,57,0,27,10,94,"A",true};
    d.cou[7].enr.studentIndex[1]=7; d.cou[7].enr.g[1]={76,45,0,22,9,76,"B-",true};
    d.cou[7].studentCount=2;
    d.cou[11].enr.studentIndex[0]=9;  d.cou[11].enr.g[0]={88,51,13,16,8,88,"B+",true};
    d.cou[11].enr.studentIndex[1]=10; d.cou[11].enr.g[1]={67,39,9,12,7,67,"C-",true};
    d.cou[11].studentCount=2;
    d.cou[10].enr.studentIndex[0]=9;  d.cou[10].enr.g[0]={97,59,0,28,10,97,"A+",true};
    d.cou[10].enr.studentIndex[1]=10; d.cou[10].enr.g[1]={80,48,0,23,9,80,"B",true};
    d.cou[10].studentCount=2;

    loadCourses(d);

    int preGradedStudents[] = {0,1,3,4,6,7,9,10};
    for (int idx : preGradedStudents)
        calculateGPA(idx, d, false);
}

void loadCourses(AppData& d) {
    for (int i = 0; i < d.numOfStu; i++) {
        for (int j = 0; j < 10; j++) {
            if (d.stu[i].studentCourses[j].isEmpty()) continue;
            for (int k = 0; k < d.numOfCou; k++) {
                if (d.cou[k].name == d.stu[i].studentCourses[j]) {
                    bool already = false;
                    for (int m = 0; m < d.cou[k].studentCount; m++)
                        if (d.cou[k].enr.studentIndex[m] == i) { already = true; break; }
                    if (!already) {
                        d.cou[k].enr.studentIndex[d.cou[k].studentCount] = i;
                        d.cou[k].studentCount++;
                    }
                    break;
                }
            }
        }
    }
}

bool isMobileTaken(const QString& phone, AppData& d) {
    for (int i = 0; i < d.numOfStu; i++) if (d.stu[i].inf.mobileNumber == phone) return true;
    for (int i = 0; i < d.numOfStf; i++) if (d.stf[i].inf.mobileNumber == phone) return true;
    for (int i = 0; i < d.numOfAdm; i++) if (d.adm[i].inf.mobileNumber == phone) return true;
    return false;
}

void calculateGrade(int couIdx, grade& g, AppData& d) {
    g.total = g.final_ + g.quiz + g.practical + g.yearWork;
    float pct = (g.total / d.cou[couIdx].gr.total) * 100.0f;
    g.percentage = pct;
    if      (pct >= 97) g.letter = "A+";
    else if (pct >= 93) g.letter = "A";
    else if (pct >= 89) g.letter = "A-";
    else if (pct >= 84) g.letter = "B+";
    else if (pct >= 80) g.letter = "B";
    else if (pct >= 76) g.letter = "B-";
    else if (pct >= 73) g.letter = "C+";
    else if (pct >= 70) g.letter = "C";
    else if (pct >= 67) g.letter = "C-";
    else if (pct >= 64) g.letter = "D+";
    else if (pct >= 60) g.letter = "D";
    else                g.letter = "F";
}

float getGradePoint(const QString& letter) {
    if (letter == "A+" || letter == "A") return 4.0f;
    if (letter == "A-") return 3.7f;
    if (letter == "B+") return 3.3f;
    if (letter == "B")  return 3.0f;
    if (letter == "B-") return 2.7f;
    if (letter == "C+") return 2.3f;
    if (letter == "C")  return 2.0f;
    if (letter == "C-") return 1.7f;
    if (letter == "D+") return 1.3f;
    if (letter == "D")  return 1.0f;
    return 0.0f;
}

void getGPAletter(int stuIndex, AppData& d) {
    float gpa = d.stu[stuIndex].GPA;
    float pct = d.stu[stuIndex].percentage;
    if      (gpa >= 4.0 && pct >= 97) d.stu[stuIndex].letter = "A+";
    else if (gpa >= 4.0)               d.stu[stuIndex].letter = "A";
    else if (gpa >= 3.7)               d.stu[stuIndex].letter = "A-";
    else if (gpa >= 3.3)               d.stu[stuIndex].letter = "B+";
    else if (gpa >= 3.0)               d.stu[stuIndex].letter = "B";
    else if (gpa >= 2.7)               d.stu[stuIndex].letter = "B-";
    else if (gpa >= 2.3)               d.stu[stuIndex].letter = "C+";
    else if (gpa >= 2.0)               d.stu[stuIndex].letter = "C";
    else if (gpa >= 1.7)               d.stu[stuIndex].letter = "C-";
    else if (gpa >= 1.3)               d.stu[stuIndex].letter = "D+";
    else if (gpa >= 1.0)               d.stu[stuIndex].letter = "D";
    else                               d.stu[stuIndex].letter = "F";
}

void getTotalMarks(int stuIndex, AppData& d) {
    float total = 0, couTotal = 0;
    for (int i = 0; i < 10; i++) {
        if (d.stu[stuIndex].studentCourses[i].isEmpty()) continue;
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name == d.stu[stuIndex].studentCourses[i]) {
                for (int k = 0; k < d.cou[j].studentCount; k++) {
                    if (d.cou[j].enr.studentIndex[k] == stuIndex) {
                        if (d.cou[j].enr.g[k].letter.isEmpty()) break;
                        total    += d.cou[j].enr.g[k].total;
                        couTotal += d.cou[j].gr.total;
                        break;
                    }
                }
                break;
            }
        }
    }
    d.stu[stuIndex].totalMarks    = total;
    d.stu[stuIndex].couTotalMarks = couTotal;
    d.stu[stuIndex].percentage    = (couTotal > 0) ? (total / couTotal) * 100 : 0;
}

void calculateGPA(int stuIndex, AppData& d, bool showOutput) {
    float totalPoints = 0, totalCredits = 0;
    for (int i = 0; i < d.numOfCou; i++) {
        for (int j = 0; j < d.cou[i].studentCount; j++) {
            if (d.cou[i].enr.studentIndex[j] == stuIndex) {
                if (d.cou[i].enr.g[j].letter.isEmpty()) break;
                totalPoints  += getGradePoint(d.cou[i].enr.g[j].letter) * d.cou[i].creditHours;
                totalCredits += d.cou[i].creditHours;
            }
        }
    }
    if (totalCredits == 0) { d.stu[stuIndex].GPA = 0; return; }
    d.stu[stuIndex].GPA = totalPoints / totalCredits;
    getTotalMarks(stuIndex, d);
    getGPAletter(stuIndex, d);
    (void)showOutput;
}

QString getDayName(int weekDay) {
    switch (weekDay) {
    case 1: return "Saturday";
    case 2: return "Sunday";
    case 3: return "Monday";
    case 4: return "Tuesday";
    case 5: return "Wednesday";
    case 6: return "Thursday";
    default: return "Unknown";
    }
}

void addNotification(long long int userId, int userType,
                     const QString& text,
                     const QString& icon,
                     const QString& color,
                     AppData& d)
{
    if (d.numOfNotif >= NOTIF_SIZE) return;
    notification& n = d.notif[d.numOfNotif++];
    n.userId    = userId;
    n.userType  = userType;
    n.text      = text;
    n.icon      = icon;
    n.color     = color;
    n.isRead    = false;
    n.timestamp = ++notifTimestamp;
}

// ─────────────────────────────────────────────────────────────────────────────
// DATA PERSISTENCE
// ─────────────────────────────────────────────────────────────────────────────
void saveData(const AppData& d)
{
    QFile f("college_data.dat");
    if (!f.open(QIODevice::WriteOnly)) return;
    QDataStream s(&f);
    s.setVersion(QDataStream::Qt_5_0);

    s << stuIDCounter << stfIDCounter << admIDCounter << notifTimestamp;

    s << d.numOfStu;
    for (int i = 0; i < d.numOfStu; i++) {
        const student& st = d.stu[i];
        s << st.inf.id << st.inf.name << st.inf.mobileNumber << st.inf.password << st.inf.address;
        s << st.department << st.academicYear;
        for (int j = 0; j < 10; j++) s << st.studentCourses[j];
        s << st.couTotalMarks << st.totalMarks << st.percentage << st.GPA << st.letter;
    }

    s << d.numOfStf;
    for (int i = 0; i < d.numOfStf; i++) {
        const staff& sf = d.stf[i];
        s << sf.inf.id << sf.inf.name << sf.inf.mobileNumber << sf.inf.password << sf.inf.address;
        s << sf.department << sf.position << sf.stfNumOfCou;
        for (int j = 0; j < 10; j++) s << sf.teachingCourses[j];
    }

    s << d.numOfAdm;
    for (int i = 0; i < d.numOfAdm; i++) {
        const administration& ad = d.adm[i];
        s << ad.inf.id << ad.inf.name << ad.inf.mobileNumber << ad.inf.password << ad.inf.address;
        s << ad.position;
    }

    s << d.numOfCou;
    for (int i = 0; i < d.numOfCou; i++) {
        const course& co = d.cou[i];
        s << co.name << co.creditHours << co.academicYear;
        s << co.gr.total << co.gr.finalMax << co.gr.practicalMax << co.gr.yearWorkMax << co.gr.quizMax;
        for (int j = 0; j < 5; j++) s << co.academicMemberName[j];
        s << co.studentCount;
        for (int j = 0; j < co.studentCount; j++) {
            s << co.enr.studentIndex[j];
            s << co.enr.g[j].total << co.enr.g[j].final_ << co.enr.g[j].practical
              << co.enr.g[j].yearWork << co.enr.g[j].quiz << co.enr.g[j].percentage
              << co.enr.g[j].letter << co.enr.g[j].gradeAssigned;
        }
        s << co.schedule.weekDay << co.schedule.hour << co.schedule.minute << co.schedule.isSet;
    }

    s << d.numOfExm;
    for (int i = 0; i < d.numOfExm; i++) {
        const exam& ex = d.exm[i];
        s << ex.sch.day << ex.sch.month << ex.sch.year << ex.sch.hour << ex.sch.minute;
        s << ex.co.name << ex.co.creditHours << ex.co.academicYear;
        s << ex.co.gr.total << ex.co.gr.finalMax << ex.co.gr.practicalMax << ex.co.gr.yearWorkMax << ex.co.gr.quizMax;
        for (int j = 0; j < 5; j++) s << ex.co.academicMemberName[j];
        s << ex.co.studentCount;
        for (int j = 0; j < ex.co.studentCount; j++) {
            s << ex.co.enr.studentIndex[j];
            s << ex.co.enr.g[j].total << ex.co.enr.g[j].final_ << ex.co.enr.g[j].practical
              << ex.co.enr.g[j].yearWork << ex.co.enr.g[j].quiz << ex.co.enr.g[j].percentage
              << ex.co.enr.g[j].letter << ex.co.enr.g[j].gradeAssigned;
        }
        s << ex.co.schedule.weekDay << ex.co.schedule.hour << ex.co.schedule.minute << ex.co.schedule.isSet;
    }

    s << d.numOfApp;
    for (int i = 0; i < d.numOfApp; i++) {
        const appointment& ap = d.app[i];
        s << ap.studentID << ap.studentName << ap.stfIndex << ap.stfName << ap.status;
        s << ap.sch.day << ap.sch.month << ap.sch.year << ap.sch.hour << ap.sch.minute;
    }

    s << d.numOfNotif;
    for (int i = 0; i < d.numOfNotif; i++) {
        const notification& n = d.notif[i];
        s << n.userId << n.userType << n.text << n.icon << n.color << n.isRead << n.timestamp;
    }
}

bool loadData(AppData& d)
{
    QFile f("college_data.dat");
    if (!f.exists() || !f.open(QIODevice::ReadOnly)) return false;
    QDataStream s(&f);
    s.setVersion(QDataStream::Qt_5_0);

    s >> stuIDCounter >> stfIDCounter >> admIDCounter >> notifTimestamp;
    if (s.status() != QDataStream::Ok) return false;

    s >> d.numOfStu;
    for (int i = 0; i < d.numOfStu; i++) {
        student& st = d.stu[i];
        s >> st.inf.id >> st.inf.name >> st.inf.mobileNumber >> st.inf.password >> st.inf.address;
        s >> st.department >> st.academicYear;
        for (int j = 0; j < 10; j++) s >> st.studentCourses[j];
        s >> st.couTotalMarks >> st.totalMarks >> st.percentage >> st.GPA >> st.letter;
    }

    s >> d.numOfStf;
    for (int i = 0; i < d.numOfStf; i++) {
        staff& sf = d.stf[i];
        s >> sf.inf.id >> sf.inf.name >> sf.inf.mobileNumber >> sf.inf.password >> sf.inf.address;
        s >> sf.department >> sf.position >> sf.stfNumOfCou;
        for (int j = 0; j < 10; j++) s >> sf.teachingCourses[j];
    }

    s >> d.numOfAdm;
    for (int i = 0; i < d.numOfAdm; i++) {
        administration& ad = d.adm[i];
        s >> ad.inf.id >> ad.inf.name >> ad.inf.mobileNumber >> ad.inf.password >> ad.inf.address;
        s >> ad.position;
    }

    s >> d.numOfCou;
    for (int i = 0; i < d.numOfCou; i++) {
        course& co = d.cou[i];
        s >> co.name >> co.creditHours >> co.academicYear;
        s >> co.gr.total >> co.gr.finalMax >> co.gr.practicalMax >> co.gr.yearWorkMax >> co.gr.quizMax;
        for (int j = 0; j < 5; j++) s >> co.academicMemberName[j];
        s >> co.studentCount;
        for (int j = 0; j < co.studentCount; j++) {
            s >> co.enr.studentIndex[j];
            s >> co.enr.g[j].total >> co.enr.g[j].final_ >> co.enr.g[j].practical
              >> co.enr.g[j].yearWork >> co.enr.g[j].quiz >> co.enr.g[j].percentage
              >> co.enr.g[j].letter >> co.enr.g[j].gradeAssigned;
        }
        s >> co.schedule.weekDay >> co.schedule.hour >> co.schedule.minute >> co.schedule.isSet;
    }

    s >> d.numOfExm;
    for (int i = 0; i < d.numOfExm; i++) {
        exam& ex = d.exm[i];
        s >> ex.sch.day >> ex.sch.month >> ex.sch.year >> ex.sch.hour >> ex.sch.minute;
        s >> ex.co.name >> ex.co.creditHours >> ex.co.academicYear;
        s >> ex.co.gr.total >> ex.co.gr.finalMax >> ex.co.gr.practicalMax >> ex.co.gr.yearWorkMax >> ex.co.gr.quizMax;
        for (int j = 0; j < 5; j++) s >> ex.co.academicMemberName[j];
        s >> ex.co.studentCount;
        for (int j = 0; j < ex.co.studentCount; j++) {
            s >> ex.co.enr.studentIndex[j];
            s >> ex.co.enr.g[j].total >> ex.co.enr.g[j].final_ >> ex.co.enr.g[j].practical
              >> ex.co.enr.g[j].yearWork >> ex.co.enr.g[j].quiz >> ex.co.enr.g[j].percentage
              >> ex.co.enr.g[j].letter >> ex.co.enr.g[j].gradeAssigned;
        }
        s >> ex.co.schedule.weekDay >> ex.co.schedule.hour >> ex.co.schedule.minute >> ex.co.schedule.isSet;
    }

    s >> d.numOfApp;
    for (int i = 0; i < d.numOfApp; i++) {
        appointment& ap = d.app[i];
        s >> ap.studentID >> ap.studentName >> ap.stfIndex >> ap.stfName >> ap.status;
        s >> ap.sch.day >> ap.sch.month >> ap.sch.year >> ap.sch.hour >> ap.sch.minute;
    }

    s >> d.numOfNotif;
    for (int i = 0; i < d.numOfNotif; i++) {
        notification& n = d.notif[i];
        s >> n.userId >> n.userType >> n.text >> n.icon >> n.color >> n.isRead >> n.timestamp;
    }

    return s.status() == QDataStream::Ok;
}

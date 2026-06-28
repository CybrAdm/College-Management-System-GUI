#include "MainWindow.h"
#include "StyleHelpers.h"

static QStringList admSidebarItems() {
    return { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DA  Manage Courses",
             "  \U0001F550  Course Schedule", "  \U0001F4CB  Exam Schedule",
             "  \U0001F514  Notifications" };
}
static QList<int> admSidebarPages() { return {17,18,19,20,21,24}; }

// ══════════════════════════════════════════════════════════════════════════════
// ADMIN DASHBOARD
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::adminDashboard()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.adm[i].inf.id,2);
    QStringList items = admSidebarItems();
    if (unreadN>0) items[5]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, admSidebarPages(), PAGE_ADM_DASH, DS::ADM_ACCENT);

    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);

    auto* banner = new QWidget;
    banner->setStyleSheet(g_lightMode
        ? "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #FFFFFF,stop:1 #F5F1EA);"
          "border-radius:16px; border:1px solid #E0D8CC;"
        : "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #001A16,stop:1 #000C10);"
          "border-radius:16px; border:1px solid #00FFD4;");
    addShadow(banner, 28, 6, 30);
    auto* bannerL = new QHBoxLayout(banner); bannerL->setContentsMargins(28,22,28,22);
    auto* bannerText = new QVBoxLayout;
    auto* greetLbl = new QLabel(QString("\U0001F3DB  Welcome, %1!").arg(d.adm[i].inf.name));
    greetLbl->setStyleSheet(QString("color:%1; font-size:22px; font-weight:800; background:transparent;").arg(DS::TEXT_PRIMARY()));
    auto* subLbl = new QLabel(QString("%1  \u2022  ID: %2").arg(d.adm[i].position).arg(d.adm[i].inf.id));
    subLbl->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));
    bannerText->addWidget(greetLbl); bannerText->addWidget(subLbl);
    auto* roleBadge = new QLabel("ADM");
    roleBadge->setFixedSize(80,80); roleBadge->setAlignment(Qt::AlignCenter);
    roleBadge->setStyleSheet(QString("background:#00FFD4; color:%1; font-size:13px; font-weight:800; border-radius:40px;").arg(g_lightMode ? "#1A1A2E" : "#000A14"));
    bannerL->addLayout(bannerText,1); bannerL->addWidget(roleBadge);
    mainL->addWidget(banner);

    auto* statsRow = new QHBoxLayout; statsRow->setSpacing(16);
    statsRow->addWidget(makeStatCard(QString::number(d.numOfStu), "Students", "\U0001F393", DS::STU_ACCENT));
    statsRow->addWidget(makeStatCard(QString::number(d.numOfStf), "Staff Members", "\U0001F468\u200D\U0001F3EB", DS::STF_ACCENT));
    statsRow->addWidget(makeStatCard(QString::number(d.numOfCou), "Courses", "\U0001F4DA", DS::ADM_ACCENT));
    statsRow->addWidget(makeStatCard(QString::number(d.numOfExm), "Exam Slots", "\U0001F4CB", DS::WARNING));
    mainL->addLayout(statsRow);

    auto* rowL2 = new QHBoxLayout; rowL2->setSpacing(20);
    auto* stuL = new QVBoxLayout; stuL->setContentsMargins(16,16,16,16);
    auto* stuTbl = new QTableWidget(0,3);
    stuTbl->setHorizontalHeaderLabels({"Name","Dept","GPA"});
    stuTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(stuTbl);
    for (int j = 0; j < d.numOfStu; j++) {
        int row=stuTbl->rowCount(); stuTbl->insertRow(row);
        stuTbl->setItem(row,0,new QTableWidgetItem(d.stu[j].inf.name));
        stuTbl->setItem(row,1,new QTableWidgetItem(d.stu[j].department));
        auto* gItm=new QTableWidgetItem(QString("%1").arg(d.stu[j].GPA,0,'f',2));
        QString gc=(d.stu[j].GPA>=3.5f)?DS::SUCCESS:(d.stu[j].GPA>=2.5f)?DS::STU_ACCENT:DS::WARNING;
        gItm->setForeground(QColor(gc)); stuTbl->setItem(row,2,gItm);
    }
    stuL->addWidget(stuTbl);
    rowL2->addWidget(makeCard("\U0001F393  All Students", stuL, DS::STU_ACCENT), 1);

    auto* stfL = new QVBoxLayout; stfL->setContentsMargins(16,16,16,16);
    auto* stfTbl = new QTableWidget(0,3);
    stfTbl->setHorizontalHeaderLabels({"Name","Position","Courses"});
    stfTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(stfTbl);
    for (int j = 0; j < d.numOfStf; j++) {
        int row=stfTbl->rowCount(); stfTbl->insertRow(row);
        stfTbl->setItem(row,0,new QTableWidgetItem(d.stf[j].inf.name));
        stfTbl->setItem(row,1,new QTableWidgetItem(d.stf[j].position));
        stfTbl->setItem(row,2,new QTableWidgetItem(QString::number(d.stf[j].stfNumOfCou)));
    }
    stfL->addWidget(stfTbl);
    rowL2->addWidget(makeCard("\U0001F468\u200D\U0001F3EB  All Staff", stfL, DS::STF_ACCENT), 1);
    mainL->addLayout(rowL2); mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// ADMIN PROFILE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::admProfilePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.adm[i].inf.id,2);
    QStringList items = admSidebarItems();
    if (unreadN>0) items[5]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, admSidebarPages(), PAGE_ADM_PROFILE, DS::ADM_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F464  My Profile");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);

    const QString valSS = QString("color:%1; font-size:15px; font-weight:600; background:transparent;").arg(DS::TEXT_PRIMARY());
    const QString keyS = QString("color:%1; font-size:12px; font-weight:700; background:transparent;").arg(DS::TEXT_MUTED());
    auto* infoL = new QVBoxLayout; infoL->setContentsMargins(28,24,28,24); infoL->setSpacing(14);
    auto mkRow = [&](const QString& key, const QString& val) {
        auto* row = new QHBoxLayout;
        auto* k = new QLabel(key); k->setStyleSheet(keyS);
        auto* v = new QLabel(val); v->setStyleSheet(valSS);
        row->addWidget(k,1); row->addWidget(v,2); infoL->addLayout(row);
        auto* sep = new QFrame; sep->setFrameShape(QFrame::HLine); sep->setFrameShadow(QFrame::Plain);
        sep->setStyleSheet(QString("QFrame{border:none; border-top:1px solid %1; background:transparent; max-height:1px;}").arg(DS::BORDER())); infoL->addWidget(sep);
    };
    mkRow("Admin ID",  QString::number(d.adm[i].inf.id));
    mkRow("Full Name", d.adm[i].inf.name);
    mkRow("Mobile",    d.adm[i].inf.mobileNumber);
    mkRow("Address",   d.adm[i].inf.address);
    mkRow("Position",  d.adm[i].position);
    mainL->addWidget(makeCard("\U0001F4CB  Admin Information", infoL, DS::ADM_ACCENT));

    const QString fs = inputLineStyle(DS::ADM_ACCENT);
    const QString ls = labelSS();
    auto mkL3 = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* editLayout = new QVBoxLayout; editLayout->setContentsMargins(28,24,28,24); editLayout->setSpacing(10);
    auto* oldPw = new QLineEdit; oldPw->setEchoMode(QLineEdit::Password); oldPw->setPlaceholderText("Current password\u2026"); oldPw->setStyleSheet(fs);
    auto* nameE = new QLineEdit; nameE->setPlaceholderText("New name (optional)\u2026"); nameE->setStyleSheet(fs);
    auto* mobE  = new QLineEdit; mobE->setText(d.adm[i].inf.mobileNumber); mobE->setStyleSheet(fs);
    auto* addrE = new QLineEdit; addrE->setText(d.adm[i].inf.address); addrE->setStyleSheet(fs);
    auto* newPw = new QLineEdit; newPw->setEchoMode(QLineEdit::Password); newPw->setPlaceholderText("New password (optional)\u2026"); newPw->setStyleSheet(fs);
    editLayout->addWidget(mkL3("Current Password:")); editLayout->addWidget(oldPw);
    editLayout->addWidget(mkL3("Full Name (optional):")); editLayout->addWidget(nameE);
    editLayout->addWidget(mkL3("Mobile Number:")); editLayout->addWidget(mobE);
    editLayout->addWidget(mkL3("Address:")); editLayout->addWidget(addrE);
    editLayout->addWidget(mkL3("New Password (optional):")); editLayout->addWidget(newPw);
    auto* saveBtn = makeBtn("\U0001F4BE  Save Changes", DS::ADM_ACCENT);
    editLayout->addSpacing(8); editLayout->addWidget(saveBtn);
    mainL->addWidget(makeCard("\u270F\uFE0F  Edit Profile", editLayout, DS::ADM_ACCENT));
    mainL->addStretch();
    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        if (oldPw->text() != d.adm[i].inf.password) { showMsg("Wrong current password!", false); return; }
        if (!nameE->text().trimmed().isEmpty()) d.adm[i].inf.name = nameE->text().trimmed();
        d.adm[i].inf.mobileNumber = mobE->text().trimmed();
        d.adm[i].inf.address = addrE->text().trimmed();
        if (!newPw->text().isEmpty()) d.adm[i].inf.password = newPw->text();
        showMsg("Profile updated!");
        refreshPage(PAGE_ADM_PROFILE); stack->setCurrentIndex(PAGE_ADM_PROFILE);
    });
    animateFadeIn(content, 250);
    auto* scroll = new QScrollArea; scroll->setWidget(content); scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame); scroll->setStyleSheet(QString("background:%1; border:none;").arg(DS::BG_PAGE()));
    return wrapWithSidebar(sidebar, scroll);
}

// ══════════════════════════════════════════════════════════════════════════════
// ADMIN MANAGE COURSES
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::admCoursesPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.adm[i].inf.id,2);
    QStringList items = admSidebarItems();
    if (unreadN>0) items[5]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, admSidebarPages(), PAGE_ADM_COURSES, DS::ADM_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4DA  Manage Courses");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* rowL = new QHBoxLayout; rowL->setSpacing(20);
    const QString fs = inputMixedStyle(DS::ADM_ACCENT);
    const QString ls = labelSmSS();
    auto mkLbl = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* addLayout2 = new QVBoxLayout; addLayout2->setContentsMargins(20,20,20,20); addLayout2->setSpacing(8);
    auto* cNameE = new QLineEdit; cNameE->setPlaceholderText("e.g. Data Structures"); cNameE->setStyleSheet(fs);
    auto* chSpin = new QSpinBox; chSpin->setRange(1,10); chSpin->setStyleSheet(fs);
    auto* yrSpin = new QSpinBox; yrSpin->setRange(1,4);  yrSpin->setStyleSheet(fs);
    auto* finSpin= new QDoubleSpinBox; finSpin->setRange(0,500); finSpin->setDecimals(0); finSpin->setStyleSheet(fs);
    auto* praSpin= new QDoubleSpinBox; praSpin->setRange(0,200); praSpin->setDecimals(0); praSpin->setStyleSheet(fs);
    auto* ywSpin = new QDoubleSpinBox; ywSpin->setRange(0,200);  ywSpin->setDecimals(0);  ywSpin->setStyleSheet(fs);
    auto* qSpin  = new QDoubleSpinBox; qSpin->setRange(0,100);   qSpin->setDecimals(0);   qSpin->setStyleSheet(fs);
    addLayout2->addWidget(mkLbl("Course Name:"));    addLayout2->addWidget(cNameE);
    addLayout2->addWidget(mkLbl("Credit Hours:"));   addLayout2->addWidget(chSpin);
    addLayout2->addWidget(mkLbl("Academic Year:"));  addLayout2->addWidget(yrSpin);
    addLayout2->addWidget(mkLbl("Final Max:"));      addLayout2->addWidget(finSpin);
    addLayout2->addWidget(mkLbl("Practical Max:"));  addLayout2->addWidget(praSpin);
    addLayout2->addWidget(mkLbl("Year Work Max:"));  addLayout2->addWidget(ywSpin);
    addLayout2->addWidget(mkLbl("Quiz Max:"));       addLayout2->addWidget(qSpin);
    auto* addCouBtn = makeBtn("\u2795  Add Course", DS::ADM_ACCENT);
    addLayout2->addSpacing(4); addLayout2->addWidget(addCouBtn); addLayout2->addStretch();
    rowL->addWidget(makeCard("\u2795  Add New Course", addLayout2, DS::ADM_ACCENT), 1);

    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16); tblLayout->setSpacing(8);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Course Name","Year","Credits","Students","Total Mark"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(tbl);
    auto doFillCou = [=]() {
        tbl->setRowCount(0);
        for (int j = 0; j < d.numOfCou; j++) {
            int row=tbl->rowCount(); tbl->insertRow(row);
            tbl->setItem(row,0,new QTableWidgetItem(d.cou[j].name));
            tbl->setItem(row,1,new QTableWidgetItem(QString("Year %1").arg(d.cou[j].academicYear)));
            tbl->setItem(row,2,new QTableWidgetItem(QString("%1 hrs").arg(d.cou[j].creditHours)));
            tbl->setItem(row,3,new QTableWidgetItem(QString("%1 students").arg(d.cou[j].studentCount)));
            tbl->setItem(row,4,new QTableWidgetItem(QString("%1").arg(d.cou[j].gr.total,0,'f',0)));
        }
    };
    doFillCou();
    auto* remCouBtn = makeBtn("\U0001F5D1  Remove Selected Course", DS::DANGER);
    tblLayout->addWidget(tbl); tblLayout->addWidget(remCouBtn);
    rowL->addWidget(makeCard("\U0001F4CB  All Courses", tblLayout, DS::ADM_ACCENT), 2);
    mainL->addLayout(rowL); mainL->addStretch();
    connect(addCouBtn, &QPushButton::clicked, this, [=]() {
        if (cNameE->text().trimmed().isEmpty()) { showMsg("Enter a course name!",false); return; }
        if (d.numOfCou>=SIZE) { showMsg("Course limit reached!",false); return; }
        float fin=(float)finSpin->value(), pra=(float)praSpin->value(), yw=(float)ywSpin->value(), q=(float)qSpin->value();
        float total2=fin+pra+yw+q;
        d.cou[d.numOfCou]={cNameE->text().trimmed(),(int)chSpin->value(),(int)yrSpin->value(),{total2,fin,pra,yw,q},{},0,{},{}};
        d.numOfCou++; cNameE->clear(); doFillCou(); showMsg("Course added!");
    });
    connect(remCouBtn, &QPushButton::clicked, this, [=]() {
        int row=tbl->currentRow(); if (row<0) { showMsg("Select a course!",false); return; }
        QString couName = d.cou[row].name;
        if (!confirmDialog("Remove Course",
                QString("Are you sure you want to remove \"%1\"?\n"
                        "This cannot be undone.").arg(couName),
                "Remove", DS::DANGER)) return;
        for (int j=row;j<d.numOfCou-1;j++) d.cou[j]=d.cou[j+1];
        d.numOfCou--; doFillCou(); showMsg("Course removed!");
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// ADMIN COURSE SCHEDULE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::admCourseSchedulePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.adm[i].inf.id,2);
    QStringList items = admSidebarItems();
    if (unreadN>0) items[5]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, admSidebarPages(), PAGE_ADM_COSCH, DS::ADM_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F550  Manage Course Schedule");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* rowL = new QHBoxLayout; rowL->setSpacing(20);
    const QString fs = inputComboStyle(DS::ADM_ACCENT);
    const QString ls = labelSmSS();
    auto mkLbl = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* addLayout2 = new QVBoxLayout; addLayout2->setContentsMargins(20,20,20,20); addLayout2->setSpacing(8);
    auto* couCombo4 = new QComboBox; couCombo4->setStyleSheet(fs);
    for (int j = 0; j < d.numOfCou; j++) couCombo4->addItem(d.cou[j].name);
    applyComboDelegate(couCombo4);
    auto* dayCombo2 = new QComboBox; dayCombo2->setStyleSheet(fs);
    for (int day2=1;day2<=6;day2++) dayCombo2->addItem(getDayName(day2));
    applyComboDelegate(dayCombo2);
    auto* hourSp3 = new QSpinBox; hourSp3->setRange(8,18); hourSp3->setStyleSheet(fs);
    auto* minCom3 = new QComboBox; minCom3->setStyleSheet(fs); minCom3->addItems({"00","15","30","45"});
    applyComboDelegate(minCom3);
    addLayout2->addWidget(mkLbl("Course:")); addLayout2->addWidget(couCombo4);
    addLayout2->addWidget(mkLbl("Day of Week:")); addLayout2->addWidget(dayCombo2);
    addLayout2->addWidget(mkLbl("Hour (8-18):")); addLayout2->addWidget(hourSp3);
    addLayout2->addWidget(mkLbl("Minute:")); addLayout2->addWidget(minCom3);
    auto* setSchBtn = makeBtn("\U0001F550  Set Schedule", DS::ADM_ACCENT);
    addLayout2->addSpacing(4); addLayout2->addWidget(setSchBtn); addLayout2->addStretch();
    rowL->addWidget(makeCard("\u270F\uFE0F  Set Course Schedule", addLayout2, DS::ADM_ACCENT), 1);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Course","Day","Time","Year","Clear"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tbl->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    tbl->setColumnWidth(4, 80);
    styleTable(tbl);
    auto doFill = [=]() {
        tbl->setRowCount(0);
        // Check if any course has a schedule set
        int scheduledCount = 0;
        for (int j = 0; j < d.numOfCou; j++) if (d.cou[j].schedule.isSet) scheduledCount++;
        if (scheduledCount == 0) {
            tbl->setRowCount(1);
            tbl->setSpan(0, 0, 1, 5);
            auto* emptyLbl = new QLabel("📭  No course schedules set yet");
            emptyLbl->setAlignment(Qt::AlignCenter);
            emptyLbl->setStyleSheet(QString("color:%1; font-size:14px; font-style:italic; background:transparent; padding:28px;").arg(DS::TEXT_MUTED()));
            tbl->setCellWidget(0, 0, emptyLbl);
            return;
        }
        for (int j = 0; j < d.numOfCou; j++) {
            if (!d.cou[j].schedule.isSet) continue;
            int row=tbl->rowCount(); tbl->insertRow(row); tbl->setRowHeight(row,52);
            tbl->setItem(row,0,new QTableWidgetItem(d.cou[j].name));
            tbl->setItem(row,1,new QTableWidgetItem(getDayName(d.cou[j].schedule.weekDay)));
            tbl->setItem(row,2,new QTableWidgetItem(QString("%1:%2").arg(d.cou[j].schedule.hour,2,10,QChar('0')).arg(d.cou[j].schedule.minute,2,10,QChar('0'))));
            tbl->setItem(row,3,new QTableWidgetItem(QString("Year %1").arg(d.cou[j].academicYear)));
            int couIdx2 = j;
            auto* actW = new QWidget; auto* actL = new QHBoxLayout(actW);
            actL->setContentsMargins(0,0,0,0); actL->setAlignment(Qt::AlignCenter);
            auto* clrBtn = new QPushButton("🗑"); clrBtn->setFixedSize(44,36); clrBtn->setCursor(Qt::PointingHandCursor);
            clrBtn->setStyleSheet(QString(
                "QPushButton{ background:transparent; color:%1; border:1px solid %1;"
                "  border-radius:8px; font-size:16px; padding:0; }"
                "QPushButton:hover{ background:%1; color:white; }").arg(DS::DANGER));
            connect(clrBtn, &QPushButton::clicked, this, [this, couIdx2]() {
                if (!confirmDialog("Clear Schedule",
                        QString("Remove the schedule for \"%1\"?\n"
                                "Students will NOT receive a notification.")
                                .arg(d.cou[couIdx2].name),
                        "Clear", DS::DANGER)) return;
                d.cou[couIdx2].schedule.isSet = false;
                refreshPage(PAGE_ADM_COSCH); stack->setCurrentIndex(PAGE_ADM_COSCH);
                showMsg("Schedule cleared!");
            });
            actL->addWidget(clrBtn); tbl->setCellWidget(row,4,actW);
        }
    };
    doFill();
    tblLayout->addWidget(tbl);
    rowL->addWidget(makeCard("\U0001F4CB  Course Timetable", tblLayout, DS::ADM_ACCENT), 2);
    mainL->addLayout(rowL); mainL->addStretch();
    connect(setSchBtn, &QPushButton::clicked, this, [=]() {
        int couIdx = couCombo4->currentIndex(); if (couIdx < 0) return;
        int newDay = dayCombo2->currentIndex()+1;
        int newHr  = (int)hourSp3->value();
        int newMin = minCom3->currentText().toInt();
        // Check for time conflicts with other courses
        for (int j = 0; j < d.numOfCou; j++) {
            if (j == couIdx) continue;
            if (!d.cou[j].schedule.isSet) continue;
            if (d.cou[j].schedule.weekDay == newDay &&
                d.cou[j].schedule.hour   == newHr  &&
                d.cou[j].schedule.minute == newMin) {
                showMsg(QString("\u26A0\uFE0F  Conflict! \"%1\" is already scheduled at the same time (%2, %3). Please choose a different slot.")
                    .arg(d.cou[j].name)
                    .arg(getDayName(newDay))
                    .arg(QString("%1:%2").arg(newHr,2,10,QChar('0')).arg(newMin,2,10,QChar('0'))), false);
                return;
            }
        }
        d.cou[couIdx].schedule = { newDay, newHr, newMin, true };
        // Notify all students enrolled in this course
        QString couName = d.cou[couIdx].name;
        QString dayName = getDayName(d.cou[couIdx].schedule.weekDay);
        QString timeStr = QString("%1:%2")
            .arg(d.cou[couIdx].schedule.hour, 2, 10, QChar('0'))
            .arg(d.cou[couIdx].schedule.minute, 2, 10, QChar('0'));
        for (int si = 0; si < d.cou[couIdx].studentCount; si++) {
            int stuIdx = d.cou[couIdx].enr.studentIndex[si];
            addNotification(d.stu[stuIdx].inf.id, 0,
                QString("\U0001F4C5  Schedule updated for \"%1\": %2 at %3. Check your Schedule tab!")
                    .arg(couName).arg(dayName).arg(timeStr),
                "\U0001F550", DS::ADM_ACCENT, d);
        }
        doFill(); showMsg("Schedule set!");
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// ADMIN EXAM SCHEDULE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::admExamSchedulePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.adm[i].inf.id,2);
    QStringList items = admSidebarItems();
    if (unreadN>0) items[5]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, admSidebarPages(), PAGE_ADM_EXSCH, DS::ADM_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4CB  Manage Exam Schedule");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* rowL = new QHBoxLayout; rowL->setSpacing(20);
    const QString fs = inputComboStyle(DS::ADM_ACCENT);
    const QString ls = labelSmSS();
    auto mkLbl = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* addLayout2 = new QVBoxLayout; addLayout2->setContentsMargins(20,20,20,20); addLayout2->setSpacing(8);
    auto* couCombo5 = new QComboBox; couCombo5->setStyleSheet(fs);
    for (int j = 0; j < d.numOfCou; j++) couCombo5->addItem(d.cou[j].name);
    applyComboDelegate(couCombo5);
    auto* daySp5  = new QSpinBox;  daySp5->setRange(1,31); daySp5->setStyleSheet(fs);
    auto* monSp5  = new QSpinBox;  monSp5->setRange(1,12); monSp5->setStyleSheet(fs);
    auto* yrSp5   = new QSpinBox;  yrSp5->setRange(2024,2030); yrSp5->setValue(2025); yrSp5->setStyleSheet(fs);
    auto* hrSp5   = new QSpinBox;  hrSp5->setRange(8,18); hrSp5->setStyleSheet(fs);
    auto* minCom5 = new QComboBox; minCom5->setStyleSheet(fs); minCom5->addItems({"00","15","30","45"});
    applyComboDelegate(minCom5);
    addLayout2->addWidget(mkLbl("Course:")); addLayout2->addWidget(couCombo5);
    auto* dateRow = new QHBoxLayout; dateRow->setSpacing(8);
    auto* dw2 = new QVBoxLayout; dw2->setSpacing(4); dw2->addWidget(mkLbl("Day:")); dw2->addWidget(daySp5);
    auto* mw2 = new QVBoxLayout; mw2->setSpacing(4); mw2->addWidget(mkLbl("Month:")); mw2->addWidget(monSp5);
    auto* yw2 = new QVBoxLayout; yw2->setSpacing(4); yw2->addWidget(mkLbl("Year:")); yw2->addWidget(yrSp5);
    dateRow->addLayout(dw2); dateRow->addLayout(mw2); dateRow->addLayout(yw2);
    addLayout2->addLayout(dateRow);
    auto* timeRow = new QHBoxLayout; timeRow->setSpacing(8);
    auto* hw2  = new QVBoxLayout; hw2->setSpacing(4);  hw2->addWidget(mkLbl("Hour:")); hw2->addWidget(hrSp5);
    auto* mw3  = new QVBoxLayout; mw3->setSpacing(4); mw3->addWidget(mkLbl("Minute:")); mw3->addWidget(minCom5);
    timeRow->addLayout(hw2); timeRow->addLayout(mw3);
    addLayout2->addLayout(timeRow);
    auto* addExBtn = makeBtn("\U0001F4CB  Add Exam Slot", DS::ADM_ACCENT);
    addLayout2->addSpacing(4); addLayout2->addWidget(addExBtn); addLayout2->addStretch();
    rowL->addWidget(makeCard("\u2795  Schedule Exam", addLayout2, DS::ADM_ACCENT), 1);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16); tblLayout->setSpacing(8);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Course","Year","Date","Time","Actions"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tbl->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tbl->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tbl->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    tbl->setColumnWidth(4, 80);
    styleTable(tbl);
    auto doFill = [=]() {
        tbl->setRowCount(0);
        if (d.numOfExm == 0) {
            tbl->setRowCount(1);
            tbl->setSpan(0, 0, 1, 5);
            auto* emptyLbl = new QLabel("📭  No exams scheduled yet");
            emptyLbl->setAlignment(Qt::AlignCenter);
            emptyLbl->setStyleSheet(QString("color:%1; font-size:14px; font-style:italic; background:transparent; padding:28px;").arg(DS::TEXT_MUTED()));
            tbl->setCellWidget(0, 0, emptyLbl);
            return;
        }
        for (int j = 0; j < d.numOfExm; j++) {
            int row=tbl->rowCount(); tbl->insertRow(row); tbl->setRowHeight(row,52);
            tbl->setItem(row,0,new QTableWidgetItem(d.exm[j].co.name));
            tbl->setItem(row,1,new QTableWidgetItem(QString("Year %1").arg(d.exm[j].co.academicYear)));
            tbl->setItem(row,2,new QTableWidgetItem(QString("%1/%2/%3").arg(d.exm[j].sch.day).arg(d.exm[j].sch.month).arg(d.exm[j].sch.year)));
            tbl->setItem(row,3,new QTableWidgetItem(QString("%1:%2").arg(d.exm[j].sch.hour,2,10,QChar('0')).arg(d.exm[j].sch.minute,2,10,QChar('0'))));
            auto* actW = new QWidget; auto* actL = new QHBoxLayout(actW);
            actL->setContentsMargins(0,0,0,0); actL->setAlignment(Qt::AlignCenter);
            auto* remBtn3 = new QPushButton("🗑"); remBtn3->setFixedSize(44,36); remBtn3->setCursor(Qt::PointingHandCursor);
            remBtn3->setStyleSheet(QString(
                "QPushButton{ background:transparent; color:%1; border:1px solid %1;"
                "  border-radius:8px; font-size:16px; padding:0; }"
                "QPushButton:hover{ background:%1; color:white; }").arg(DS::DANGER));
            int exIdx=j;
            connect(remBtn3, &QPushButton::clicked, this, [this, exIdx]() {
                QString exCouName = d.exm[exIdx].co.name;
                if (!confirmDialog("Remove Exam",
                        QString("Remove the exam slot for \"%1\"?\n"
                                "Related student notifications will also be removed.")
                                .arg(exCouName),
                        "Remove", DS::DANGER)) return;
                // Remove notifications that were sent for this exam so they
                // don't keep showing in students' notification counts.
                for (int ni = 0; ni < d.numOfNotif; ni++) {
                    if (d.notif[ni].text.contains(exCouName) &&
                        d.notif[ni].text.contains("Exam scheduled")) {
                        // Shift notifications left to delete this entry
                        for (int nj = ni; nj < d.numOfNotif-1; nj++)
                            d.notif[nj] = d.notif[nj+1];
                        d.numOfNotif--; ni--;
                    }
                }
                for (int k=exIdx;k<d.numOfExm-1;k++) d.exm[k]=d.exm[k+1]; d.numOfExm--;
                refreshPage(PAGE_ADM_EXSCH); stack->setCurrentIndex(PAGE_ADM_EXSCH);
                showMsg("Exam removed and notifications cleaned up.");
            });
            actL->addWidget(remBtn3); tbl->setCellWidget(row,4,actW);
        }
    };
    doFill();
    tblLayout->addWidget(tbl);
    rowL->addWidget(makeCard("\U0001F4CB  Exam Timetable", tblLayout, DS::ADM_ACCENT), 2);
    mainL->addLayout(rowL); mainL->addStretch();
    connect(addExBtn, &QPushButton::clicked, this, [=]() {
        if (d.numOfExm>=SIZE) { showMsg("Exam limit reached!",false); return; }
        int couIdx=couCombo5->currentIndex(); if (couIdx<0) return;
        exam& ex=d.exm[d.numOfExm++];
        ex.co=d.cou[couIdx];
        ex.sch={daySp5->value(),monSp5->value(),yrSp5->value(),hrSp5->value(),minCom5->currentText().toInt()};
        // Notify all students enrolled in this course
        QString couName = d.cou[couIdx].name;
        QString dateStr = QString("%1/%2/%3")
            .arg(ex.sch.day).arg(ex.sch.month).arg(ex.sch.year);
        QString timeStr = QString("%1:%2")
            .arg(ex.sch.hour, 2, 10, QChar('0'))
            .arg(ex.sch.minute, 2, 10, QChar('0'));
        for (int si = 0; si < d.cou[couIdx].studentCount; si++) {
            int stuIdx = d.cou[couIdx].enr.studentIndex[si];
            addNotification(d.stu[stuIdx].inf.id, 0,
                QString("\U0001F4CB  Exam scheduled for \"%1\" on %2 at %3. Check your Exam Schedule tab!")
                    .arg(couName).arg(dateStr).arg(timeStr),
                "\U0001F4CB", DS::WARNING, d);
        }
        doFill(); showMsg("Exam scheduled!");
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

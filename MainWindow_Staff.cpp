#include "MainWindow.h"
#include "StyleHelpers.h"

static QStringList stfSidebarItems() {
    return { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DD  Set Grades",
             "  \U0001F4DA  My Courses", "  \U0001F5D3  Appointments", "  \U0001F3C6  Top Students",
             "  \U0001F514  Notifications" };
}
static QList<int> stfSidebarPages() { return {11,12,13,14,15,16,23}; }

// ══════════════════════════════════════════════════════════════════════════════
// STAFF DASHBOARD
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::staffDashboard()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id,1);
    QStringList items = stfSidebarItems();
    if (unreadN>0) items[6]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSidebarPages(), PAGE_STF_DASH, DS::STF_ACCENT);

    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);

    auto* banner = new QWidget;
    banner->setStyleSheet(g_lightMode
        ? "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #FFFFFF,stop:1 #F5F1EA);"
          "border-radius:16px; border:1px solid #E0D8CC;"
        : "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #120025,stop:1 #08001A);"
          "border-radius:16px; border:1px solid #C040FF;");
    addShadow(banner, 28, 6, 30);
    auto* bannerL = new QHBoxLayout(banner); bannerL->setContentsMargins(28,22,28,22);
    auto* bannerText = new QVBoxLayout;
    auto* greetLbl = new QLabel(QString("\U0001F44B  Welcome, %1!").arg(d.stf[i].inf.name));
    greetLbl->setStyleSheet(QString("color:%1; font-size:22px; font-weight:800; background:transparent;").arg(DS::TEXT_PRIMARY()));
    auto* subLbl = new QLabel(QString("Department: %1  \u2022  %2  \u2022  ID: %3")
        .arg(d.stf[i].department).arg(d.stf[i].position).arg(d.stf[i].inf.id));
    subLbl->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));
    bannerText->addWidget(greetLbl); bannerText->addWidget(subLbl);
    auto* roleBadge = new QLabel(d.stf[i].position.left(3).toUpper());
    roleBadge->setFixedSize(80,80); roleBadge->setAlignment(Qt::AlignCenter);
    roleBadge->setStyleSheet(QString("background:#C040FF; color:%1; font-size:13px; font-weight:800; border-radius:40px;").arg(g_lightMode ? "#FFFFFF" : "#000A14"));
    bannerL->addLayout(bannerText,1); bannerL->addWidget(roleBadge);
    mainL->addWidget(banner);

    auto* statsRow = new QHBoxLayout; statsRow->setSpacing(16);
    statsRow->addWidget(makeStatCard(QString::number(d.stf[i].stfNumOfCou), "Teaching Courses", "\U0001F4DA", DS::STF_ACCENT));
    int pendingAppt = 0;
    for (int j=0;j<d.numOfApp;j++) if (d.app[j].stfIndex==i && d.app[j].status=="Pending") pendingAppt++;
    statsRow->addWidget(makeStatCard(QString::number(pendingAppt), "Pending Appointments", "\U0001F5D3", DS::WARNING));
    statsRow->addWidget(makeStatCard(QString::number(unreadN), "Notifications", "\U0001F514", DS::DANGER));
    mainL->addLayout(statsRow);

    auto* coursesL = new QVBoxLayout; coursesL->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,3);
    tbl->setHorizontalHeaderLabels({"Course","Year","Students"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(tbl);
    for (int j = 0; j < d.stf[i].stfNumOfCou; j++) {
        int row = tbl->rowCount(); tbl->insertRow(row);
        tbl->setItem(row,0,new QTableWidgetItem(d.stf[i].teachingCourses[j]));
        int yr=0, cnt=0;
        for (int k=0;k<d.numOfCou;k++) if (d.cou[k].name==d.stf[i].teachingCourses[j]) { yr=d.cou[k].academicYear; cnt=d.cou[k].studentCount; break; }
        tbl->setItem(row,1,new QTableWidgetItem(QString("Year %1").arg(yr)));
        tbl->setItem(row,2,new QTableWidgetItem(QString("%1 students").arg(cnt)));
    }
    coursesL->addWidget(tbl);
    mainL->addWidget(makeCard("\U0001F4CB  My Teaching Courses", coursesL, DS::STF_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// STAFF PROFILE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stfProfilePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id,1);
    QStringList items = stfSidebarItems();
    if (unreadN>0) items[6]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSidebarPages(), PAGE_STF_PROFILE, DS::STF_ACCENT);
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
    mkRow("Staff ID",     QString::number(d.stf[i].inf.id));
    mkRow("Full Name",    d.stf[i].inf.name);
    mkRow("Mobile",       d.stf[i].inf.mobileNumber);
    mkRow("Address",      d.stf[i].inf.address);
    mkRow("Department",   d.stf[i].department);
    mkRow("Position",     d.stf[i].position);
    mainL->addWidget(makeCard("\U0001F4CB  Staff Information", infoL, DS::STF_ACCENT));

    const QString fs = inputLineStyle(DS::STF_ACCENT);
    const QString ls = labelSS();
    auto mkL3 = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* editLayout = new QVBoxLayout; editLayout->setContentsMargins(28,24,28,24); editLayout->setSpacing(10);
    auto* oldPw = new QLineEdit; oldPw->setEchoMode(QLineEdit::Password); oldPw->setPlaceholderText("Current password\u2026"); oldPw->setStyleSheet(fs);
    auto* nameE = new QLineEdit; nameE->setPlaceholderText("New name (optional)\u2026"); nameE->setStyleSheet(fs);
    auto* mobE  = new QLineEdit; mobE->setText(d.stf[i].inf.mobileNumber); mobE->setStyleSheet(fs);
    auto* addrE = new QLineEdit; addrE->setText(d.stf[i].inf.address); addrE->setStyleSheet(fs);
    auto* newPw = new QLineEdit; newPw->setEchoMode(QLineEdit::Password); newPw->setPlaceholderText("New password (optional)\u2026"); newPw->setStyleSheet(fs);
    editLayout->addWidget(mkL3("Current Password:")); editLayout->addWidget(oldPw);
    editLayout->addWidget(mkL3("Full Name (optional):")); editLayout->addWidget(nameE);
    editLayout->addWidget(mkL3("Mobile Number:")); editLayout->addWidget(mobE);
    editLayout->addWidget(mkL3("Address:")); editLayout->addWidget(addrE);
    editLayout->addWidget(mkL3("New Password (optional):")); editLayout->addWidget(newPw);
    auto* saveBtn = makeBtn("\U0001F4BE  Save Changes", DS::STF_ACCENT);
    editLayout->addSpacing(8); editLayout->addWidget(saveBtn);
    mainL->addWidget(makeCard("\u270F\uFE0F  Edit Profile", editLayout, DS::STF_ACCENT));
    mainL->addStretch();
    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        if (oldPw->text() != d.stf[i].inf.password) { showMsg("Wrong current password!", false); return; }
        if (!nameE->text().trimmed().isEmpty()) d.stf[i].inf.name = nameE->text().trimmed();
        d.stf[i].inf.mobileNumber = mobE->text().trimmed();
        d.stf[i].inf.address = addrE->text().trimmed();
        if (!newPw->text().isEmpty()) d.stf[i].inf.password = newPw->text();
        showMsg("Profile updated!");
        refreshPage(PAGE_STF_PROFILE); stack->setCurrentIndex(PAGE_STF_PROFILE);
    });
    animateFadeIn(content, 250);
    auto* scroll = new QScrollArea; scroll->setWidget(content); scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame); scroll->setStyleSheet(QString("background:%1; border:none;").arg(DS::BG_PAGE()));
    return wrapWithSidebar(sidebar, scroll);
}

// ══════════════════════════════════════════════════════════════════════════════
// STAFF SET GRADES
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stfSetGradesPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id,1);
    QStringList items = stfSidebarItems();
    if (unreadN>0) items[6]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSidebarPages(), PAGE_STF_GRADES, DS::STF_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4DD  Set Student Grades");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* rowL = new QHBoxLayout; rowL->setSpacing(20);

    const QString comboSS = QString(
        "QComboBox{ border:2px solid %1; border-radius:10px; padding:9px 12px; font-size:13px; color:%2; background:%3; }"
        "QComboBox:hover{ border-color:%4; }"
        "QComboBox:focus{ border-color:%4; }"
        "QComboBox::drop-down{ border:none; padding-right:12px; }"
        "QComboBox::down-arrow{ width:10px; height:10px; }"
        "QComboBox QAbstractItemView{ border:1px solid %1; border-radius:8px; outline:none;"
        "  background:%3; color:%2; selection-background-color:%5; selection-color:%2; padding:4px; }"
        "QComboBox QAbstractItemView::item{ padding:6px 10px; min-height:28px; background:%3; color:%2; }"
        "QComboBox QAbstractItemView::item:hover{ background:%5; }"
        "QComboBox QAbstractItemView::item:selected{ background:%5; }"
    ).arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT()).arg(DS::STF_ACCENT).arg(DS::SEL_BG());

    const QString spinActiveSS = QString(
        "QDoubleSpinBox{ border:2px solid %1; border-radius:10px; padding:9px 12px; min-height:36px;"
        "  font-size:13px; font-weight:600; color:%2; background:%3; }"
        "QDoubleSpinBox:focus{ border-color:%4; background:%5; }"
        "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button{ width:20px; border:none; background:transparent; }"
    ).arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT()).arg(DS::STF_ACCENT).arg(DS::BG_INPUT_FOCUS());

    const QString spinDisabledSS = QString(
        "QDoubleSpinBox{ border:2px solid %1; border-radius:10px; padding:9px 12px; min-height:36px;"
        "  font-size:13px; color:%2; background:%3; }"
        "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button{ width:20px; border:none; background:transparent; }"
        "QDoubleSpinBox:disabled{ color:%2; }")
        .arg(DS::BORDER()).arg(DS::TEXT_MUTED()).arg(g_lightMode ? "#F0F4FF" : "#080B18");

    auto* selL = new QVBoxLayout; selL->setContentsMargins(20,20,20,20); selL->setSpacing(8);
    auto* couCombo = new QComboBox; couCombo->setStyleSheet(comboSS);
    for (int j = 0; j < d.stf[i].stfNumOfCou; j++) couCombo->addItem(d.stf[i].teachingCourses[j]);
    applyComboDelegate(couCombo);
    auto* stuCombo = new QComboBox; stuCombo->setStyleSheet(comboSS);
    applyComboDelegate(stuCombo);
    auto* finalSp  = new QDoubleSpinBox; finalSp->setRange(0,500);  finalSp->setDecimals(1);
    auto* pracSp   = new QDoubleSpinBox; pracSp->setRange(0,200);   pracSp->setDecimals(1);
    auto* ywSp     = new QDoubleSpinBox; ywSp->setRange(0,200);     ywSp->setDecimals(1);
    auto* quizSp   = new QDoubleSpinBox; quizSp->setRange(0,100);   quizSp->setDecimals(1);

    const QString maxBaseSS = "font-size:11px; font-weight:700; background:transparent; letter-spacing:0.3px; padding:0;";
    auto mkMaxLbl = [&](const QString& c){ auto* l = new QLabel("max: \u2014"); l->setStyleSheet(QString("color:%1;").arg(c)+maxBaseSS); return l; };
    auto* finalMaxLbl = mkMaxLbl(DS::STF_ACCENT);
    auto* pracMaxLbl  = mkMaxLbl(DS::STU_ACCENT);
    auto* ywMaxLbl    = mkMaxLbl(DS::ADM_ACCENT);
    auto* quizMaxLbl  = mkMaxLbl(DS::WARNING);
    auto* pracNaLbl   = new QLabel("No practical component for this course");
    pracNaLbl->setStyleSheet(QString("color:%1; font-size:12px; font-style:italic; background:transparent;").arg(DS::TEXT_MUTED()));

    auto addGradeRow = [&](const QString& fieldName, const QString& accentClr, QDoubleSpinBox* sp, QLabel* maxLbl) {
        auto* rowHdr = new QHBoxLayout; rowHdr->setContentsMargins(0,0,0,0); rowHdr->setSpacing(6);
        auto* nameLbl = new QLabel(fieldName);
        nameLbl->setStyleSheet(QString("color:%1; font-size:12px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND()));
        auto* bar = new QFrame; bar->setFixedSize(3,14);
        bar->setStyleSheet(QString("background:%1; border-radius:1px;").arg(accentClr));
        rowHdr->addWidget(bar); rowHdr->addWidget(nameLbl); rowHdr->addStretch(); rowHdr->addWidget(maxLbl);
        selL->addLayout(rowHdr); selL->addWidget(sp); selL->addSpacing(4);
    };
    selL->addWidget([&](){ auto* l = new QLabel("Course:"); l->setStyleSheet(QString("color:%1; font-size:12px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND())); return l; }());
    selL->addWidget(couCombo); selL->addSpacing(4);
    selL->addWidget([&](){ auto* l = new QLabel("Student:"); l->setStyleSheet(QString("color:%1; font-size:12px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND())); return l; }());
    selL->addWidget(stuCombo);
    auto* div = new QFrame; div->setFrameShape(QFrame::HLine); div->setFrameShadow(QFrame::Plain);
    div->setStyleSheet(QString("QFrame{border:none; border-top:1px solid %1; background:transparent; max-height:1px; margin:8px 0;}").arg(DS::BORDER())); selL->addWidget(div);
    addGradeRow("Final Grade",     DS::STF_ACCENT, finalSp, finalMaxLbl);
    addGradeRow("Practical Grade", DS::STU_ACCENT, pracSp,  pracMaxLbl);
    selL->addWidget(pracNaLbl);
    addGradeRow("Year Work Grade", DS::ADM_ACCENT, ywSp,    ywMaxLbl);
    addGradeRow("Quiz Grade",      DS::WARNING,    quizSp,  quizMaxLbl);
    auto* saveGrBtn = makeBtn("\U0001F4BE  Save Grades", DS::STF_ACCENT);
    saveGrBtn->setFixedHeight(50);
    selL->addSpacing(8); selL->addWidget(saveGrBtn); selL->addStretch();
    rowL->addWidget(makeCard("\u270F\uFE0F  Enter Grades", selL, DS::STF_ACCENT), 1);

    auto* tblL = new QVBoxLayout; tblL->setContentsMargins(16,16,16,16);
    auto* grTbl = new QTableWidget(0,5);
    grTbl->setHorizontalHeaderLabels({"Student","Total","Percentage","Score","Grade"});
    grTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(grTbl);

    auto updateCourseInfo = [=]() {
        QString cname = couCombo->currentText();
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name != cname) continue;
            float fMax=d.cou[j].gr.finalMax, pMax=d.cou[j].gr.practicalMax;
            float yMax=d.cou[j].gr.yearWorkMax, qMax=d.cou[j].gr.quizMax, total=d.cou[j].gr.total;
            finalSp->setMaximum(fMax>0?fMax:500); pracSp->setMaximum(pMax>0?pMax:200);
            ywSp->setMaximum(yMax>0?yMax:200);   quizSp->setMaximum(qMax>0?qMax:100);
            finalSp->setValue(0); pracSp->setValue(0); ywSp->setValue(0); quizSp->setValue(0);
            finalMaxLbl->setText(QString("out of  %1  (course total: %2)").arg(fMax,0,'f',0).arg(total,0,'f',0));
            ywMaxLbl->setText(QString("out of  %1").arg(yMax,0,'f',0));
            quizMaxLbl->setText(QString("out of  %1").arg(qMax,0,'f',0));
            bool hasPrac = (pMax > 0.0f);
            pracSp->setEnabled(hasPrac);
            pracSp->setStyleSheet(hasPrac ? spinActiveSS : spinDisabledSS);
            pracMaxLbl->setText(hasPrac ? QString("out of  %1").arg(pMax,0,'f',0) : "N/A");
            pracNaLbl->setVisible(!hasPrac);
            finalSp->setStyleSheet(spinActiveSS); ywSp->setStyleSheet(spinActiveSS); quizSp->setStyleSheet(spinActiveSS);
            break;
        }
    };

    auto fillStuCombo = [=]() {
        stuCombo->clear();
        QString cname = couCombo->currentText();
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name==cname) {
                for (int k = 0; k < d.cou[j].studentCount; k++)
                    stuCombo->addItem(d.stu[d.cou[j].enr.studentIndex[k]].inf.name);
                break;
            }
        }
    };

    auto doFillGrTbl = [=]() {
        grTbl->setRowCount(0);
        QString cname = couCombo->currentText();
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name!=cname) continue;
            for (int k = 0; k < d.cou[j].studentCount; k++) {
                int si=d.cou[j].enr.studentIndex[k]; grade& g=d.cou[j].enr.g[k];
                int row=grTbl->rowCount(); grTbl->insertRow(row); grTbl->setRowHeight(row,44);
                grTbl->setItem(row,0,new QTableWidgetItem(d.stu[si].inf.name));
                if (g.gradeAssigned) {
                    grTbl->setItem(row,1,new QTableWidgetItem(QString("%1/%2").arg(g.total,0,'f',0).arg(d.cou[j].gr.total,0,'f',0)));
                    grTbl->setItem(row,2,new QTableWidgetItem(QString("%1%").arg(g.percentage,0,'f',1)));
                    grTbl->setItem(row,3,new QTableWidgetItem(QString("%1").arg(g.total,0,'f',0)));
                    auto* gItm = new QTableWidgetItem(g.letter); gItm->setTextAlignment(Qt::AlignCenter);
                    gItm->setForeground(QColor(g.letter.startsWith("A")?DS::SUCCESS:g.letter=="F"?DS::DANGER:DS::PRIMARY));
                    grTbl->setItem(row,4,gItm);
                } else {
                    for (int c2=1;c2<5;c2++) { auto* it=new QTableWidgetItem("Pending"); it->setForeground(QColor("#3C4E6A")); grTbl->setItem(row,c2,it); }
                }
            }
            break;
        }
    };

    fillStuCombo(); updateCourseInfo(); doFillGrTbl();
    tblL->addWidget(grTbl);
    rowL->addWidget(makeCard("\U0001F4CB  Course Grades", tblL, DS::STF_ACCENT), 2);
    mainL->addLayout(rowL); mainL->addStretch();

    connect(couCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int) { fillStuCombo(); updateCourseInfo(); doFillGrTbl(); });
    connect(saveGrBtn, &QPushButton::clicked, this, [=]() {
        QString cname = couCombo->currentText(); int stuSel = stuCombo->currentIndex();
        if (cname.isEmpty() || stuSel < 0) { showMsg("Please select a course and student first!",false); return; }
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name!=cname) continue;
            if (stuSel>=d.cou[j].studentCount) { showMsg("Invalid student selection!",false); return; }
            if (finalSp->value()>d.cou[j].gr.finalMax) { showMsg(QString("Final grade exceeds maximum (%1)!").arg(d.cou[j].gr.finalMax,0,'f',0),false); return; }
            if (pracSp->isEnabled()&&pracSp->value()>d.cou[j].gr.practicalMax) { showMsg(QString("Practical exceeds maximum (%1)!").arg(d.cou[j].gr.practicalMax,0,'f',0),false); return; }
            if (ywSp->value()>d.cou[j].gr.yearWorkMax) { showMsg(QString("Year Work exceeds maximum (%1)!").arg(d.cou[j].gr.yearWorkMax,0,'f',0),false); return; }
            if (quizSp->value()>d.cou[j].gr.quizMax) { showMsg(QString("Quiz exceeds maximum (%1)!").arg(d.cou[j].gr.quizMax,0,'f',0),false); return; }
            grade& g = d.cou[j].enr.g[stuSel];
            g.final_    = (float)finalSp->value();
            g.practical = pracSp->isEnabled() ? (float)pracSp->value() : 0.0f;
            g.yearWork  = (float)ywSp->value();
            g.quiz      = (float)quizSp->value();
            calculateGrade(j, g, d);
            g.gradeAssigned = true;
            int si = d.cou[j].enr.studentIndex[stuSel];
            calculateGPA(si, d, false);
            addNotification(d.stu[si].inf.id, 0,
                QString("\U0001F4CA Grade posted for %1: %2 (%3%)").arg(cname).arg(g.letter).arg(g.percentage,0,'f',1),
                "\U0001F4CA", DS::SUCCESS, d);
            showMsg(QString("\u2705  Grades saved!  Letter: %1  (%2%)").arg(g.letter).arg(g.percentage,0,'f',1));
            doFillGrTbl(); break;
        }
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// STAFF COURSES
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stfCoursesPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id,1);
    QStringList items = stfSidebarItems();
    if (unreadN>0) items[6]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSidebarPages(), PAGE_STF_COURSES, DS::STF_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4DA  Teaching Courses");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* rowL = new QHBoxLayout; rowL->setSpacing(20);
    const QString fs = inputComboStyle(DS::STF_ACCENT);
    auto* addLayout2 = new QVBoxLayout; addLayout2->setContentsMargins(20,20,20,20); addLayout2->setSpacing(10);
    auto* pickLbl = new QLabel("Select a course to add:");
    pickLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND()));
    auto* couCombo2 = new QComboBox; couCombo2->setStyleSheet(fs);
    for (int j = 0; j < d.numOfCou; j++) {
        bool teaching = false;
        for (int k = 0; k < d.stf[i].stfNumOfCou; k++) if (d.stf[i].teachingCourses[k]==d.cou[j].name) { teaching=true; break; }
        if (!teaching) couCombo2->addItem(d.cou[j].name);
    }
    applyComboDelegate(couCombo2);
    auto* addBtn2 = makeBtn("\u2795  Add Teaching Course", DS::STF_ACCENT);
    addLayout2->addWidget(pickLbl); addLayout2->addWidget(couCombo2); addLayout2->addSpacing(4);
    addLayout2->addWidget(addBtn2); addLayout2->addStretch();
    rowL->addWidget(makeCard("\u2795  Add Course", addLayout2, DS::STF_ACCENT), 1);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16); tblLayout->setSpacing(8);
    auto* tbl = new QTableWidget(0,3);
    tbl->setHorizontalHeaderLabels({"Course Name","Academic Year","Students"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(tbl);
    auto doFill = [=]() {
        tbl->setRowCount(0);
        for (int j = 0; j < d.stf[i].stfNumOfCou; j++) {
            int row=tbl->rowCount(); tbl->insertRow(row);
            tbl->setItem(row,0,new QTableWidgetItem(d.stf[i].teachingCourses[j]));
            int yr=0,cnt=0;
            for (int k = 0; k < d.numOfCou; k++) if (d.cou[k].name==d.stf[i].teachingCourses[j]) { yr=d.cou[k].academicYear; cnt=d.cou[k].studentCount; break; }
            tbl->setItem(row,1,new QTableWidgetItem(QString("Year %1").arg(yr)));
            tbl->setItem(row,2,new QTableWidgetItem(QString("%1 students").arg(cnt)));
        }
    };
    doFill();
    auto* remBtn2 = makeBtn("\U0001F5D1  Remove Selected", DS::DANGER);
    tblLayout->addWidget(tbl); tblLayout->addWidget(remBtn2);
    rowL->addWidget(makeCard("\U0001F4CB  My Teaching Courses", tblLayout, DS::STF_ACCENT), 2);
    mainL->addLayout(rowL); mainL->addStretch();
    connect(addBtn2, &QPushButton::clicked, this, [=]() {
        if (couCombo2->count()==0) { showMsg("No available courses!",false); return; }
        if (d.stf[i].stfNumOfCou>=10) { showMsg("Maximum 10 courses!",false); return; }
        QString cname=couCombo2->currentText();
        d.stf[i].teachingCourses[d.stf[i].stfNumOfCou]=cname; d.stf[i].stfNumOfCou++;
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name==cname) {
                for (int k = 0; k < 5; k++) if (d.cou[j].academicMemberName[k].isEmpty()) { d.cou[j].academicMemberName[k]=d.stf[i].inf.name; break; }
                break;
            }
        }
        couCombo2->removeItem(couCombo2->currentIndex()); doFill(); showMsg("Course added!");
    });
    connect(remBtn2, &QPushButton::clicked, this, [=]() {
        int row=tbl->currentRow(); if (row<0) { showMsg("Select a course!",false); return; }
        QString cname=tbl->item(row,0)->text();
        if (!confirmDialog("Remove Course",
            QString("Remove <b>%1</b> from your teaching list?<br>This cannot be undone.").arg(cname),
            "Yes, Remove", DS::DANGER)) return;
        for (int j = 0; j < d.stf[i].stfNumOfCou; j++) {
            if (d.stf[i].teachingCourses[j]==cname) {
                for (int k=j;k<d.stf[i].stfNumOfCou-1;k++) d.stf[i].teachingCourses[k]=d.stf[i].teachingCourses[k+1];
                d.stf[i].teachingCourses[d.stf[i].stfNumOfCou-1]=""; d.stf[i].stfNumOfCou--; break;
            }
        }
        couCombo2->addItem(cname); doFill(); showMsg("Course removed!");
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// STAFF APPOINTMENTS
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stfAppointmentsPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id,1);
    QStringList items = stfSidebarItems();
    if (unreadN>0) items[6]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSidebarPages(), PAGE_STF_APPT, DS::STF_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F5D3  My Appointments");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Student Name","Date","Time","Status","Action"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tbl->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    tbl->setColumnWidth(3, 130);
    styleTable(tbl);
    for (int j = 0; j < d.numOfApp; j++) {
        if (d.app[j].stfIndex!=i) continue;
        int row=tbl->rowCount(); tbl->insertRow(row); tbl->setRowHeight(row,52);
        tbl->setItem(row,0,new QTableWidgetItem(d.app[j].studentName));
        tbl->setItem(row,1,new QTableWidgetItem(QString("%1/%2").arg(d.app[j].sch.day).arg(d.app[j].sch.month)));
        tbl->setItem(row,2,new QTableWidgetItem(QString("%1:%2").arg(d.app[j].sch.hour,2,10,QChar('0')).arg(d.app[j].sch.minute,2,10,QChar('0'))));
        tbl->setCellWidget(row,3,makeStatusChip(d.app[j].status));
        // Row background tint based on status
        QColor rowBg = (d.app[j].status=="Approved")
            ? (g_lightMode ? QColor(0,180,100,25)  : QColor(0,255,136,18))
            : (d.app[j].status=="Rejected")
            ? (g_lightMode ? QColor(200,0,60,20)   : QColor(255,45,107,18))
            : (g_lightMode ? QColor(160,110,0,25)  : QColor(255,180,0,18));
        for (int c = 0; c < 3; c++) if (tbl->item(row,c)) tbl->item(row,c)->setBackground(rowBg);
        if (d.app[j].status=="Pending") {
            auto* actW = new QWidget; auto* actL = new QHBoxLayout(actW);
            actL->setContentsMargins(6,6,6,6); actL->setSpacing(8);
            auto* approveBtn = new QPushButton("\u2713 Approve"); approveBtn->setCursor(Qt::PointingHandCursor);
            approveBtn->setStyleSheet(QString("QPushButton{ background:%1; color:#000A14; border:none; border-radius:8px; padding:6px 14px; font-size:12px; font-weight:600; } QPushButton:hover{ background:#00CC70; }").arg(DS::SUCCESS));
            auto* rejectBtn = new QPushButton("\u2717 Reject"); rejectBtn->setCursor(Qt::PointingHandCursor);
            rejectBtn->setStyleSheet(QString("QPushButton{ background:%1; color:white; border:none; border-radius:8px; padding:6px 14px; font-size:12px; font-weight:600; } QPushButton:hover{ background:#CC1A50; }").arg(DS::DANGER));
            int appIdx = j;
            connect(approveBtn, &QPushButton::clicked, this, [this, appIdx, i]() {
                d.app[appIdx].status = "Approved";
                addNotification(d.app[appIdx].studentID, 0,
                    QString("\u2705 Appointment on %1/%2 at %3:%4 Approved by %5")
                        .arg(d.app[appIdx].sch.day).arg(d.app[appIdx].sch.month)
                        .arg(d.app[appIdx].sch.hour,2,10,QChar('0'))
                        .arg(d.app[appIdx].sch.minute,2,10,QChar('0'))
                        .arg(d.stf[i].inf.name),
                    "\u2705", DS::SUCCESS, d);
                refreshPage(PAGE_STF_APPT); stack->setCurrentIndex(PAGE_STF_APPT);
            });
            connect(rejectBtn, &QPushButton::clicked, this, [this, appIdx, i]() {
                d.app[appIdx].status = "Rejected";
                addNotification(d.app[appIdx].studentID, 0,
                    QString("\u274C Appointment on %1/%2 was Rejected by %3")
                        .arg(d.app[appIdx].sch.day).arg(d.app[appIdx].sch.month)
                        .arg(d.stf[i].inf.name),
                    "\u274C", DS::DANGER, d);
                refreshPage(PAGE_STF_APPT); stack->setCurrentIndex(PAGE_STF_APPT);
            });
            actL->addWidget(approveBtn); actL->addWidget(rejectBtn); actL->addStretch();
            tbl->setCellWidget(row,4,actW);
        } else { tbl->setItem(row,4,new QTableWidgetItem("\u2014")); }
    }
    tblLayout->addWidget(tbl);
    mainL->addWidget(makeCard("\U0001F4CB  Appointment Requests", tblLayout, DS::STF_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// STAFF TOP STUDENTS
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stfTopStudentsPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id,1);
    QStringList items = stfSidebarItems();
    if (unreadN>0) items[6]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSidebarPages(), PAGE_STF_TOP, DS::STF_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F3C6  Top Students");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* couCombo3 = new QComboBox;
    couCombo3->setStyleSheet(QString(
        "QComboBox{ border:2px solid %1; border-radius:10px; padding:10px 14px; font-size:14px; color:%2; background:%3; }"
        "QComboBox:hover{ border-color:%4; } QComboBox:focus{ border-color:%4; }"
        "QComboBox::drop-down{ border:none; padding-right:14px; }"
        "QComboBox::down-arrow{ width:10px; height:10px; }"
        "QComboBox QAbstractItemView{ border:1px solid %1; border-radius:8px; outline:none;"
        "  background:%3; color:%2; selection-background-color:%5; selection-color:%2; padding:4px; }"
        "QComboBox QAbstractItemView::item{ padding:6px 10px; min-height:28px; background:%3; color:%2; }"
        "QComboBox QAbstractItemView::item:hover{ background:%5; }"
        "QComboBox QAbstractItemView::item:selected{ background:%5; }"
    ).arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT()).arg(DS::STF_ACCENT).arg(DS::SEL_BG()));
    for (int j = 0; j < d.stf[i].stfNumOfCou; j++) couCombo3->addItem(d.stf[i].teachingCourses[j]);
    applyComboDelegate(couCombo3);
    mainL->addWidget(couCombo3);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Rank","Student Name","Total","Percentage","Grade"});
    tbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    styleTable(tbl);
    QString topSS = QString(TABLE_SS);
    topSS.replace(g_lightMode ? "alternate-background-color:#F4F7FF" : "alternate-background-color:#0F1325",
                  g_lightMode ? "alternate-background-color:#FFFBF0" : "alternate-background-color:#1A1500");
    tbl->setStyleSheet(topSS);
    auto doFill = [=]() {
        tbl->setRowCount(0); QString cname=couCombo3->currentText(); if (cname.isEmpty()) return;
        for (int j = 0; j < d.numOfCou; j++) {
            if (d.cou[j].name!=cname) continue;
            int idxArr[SIZE]; int cnt=0;
            for (int k = 0; k < d.cou[j].studentCount; k++) if (d.cou[j].enr.g[k].gradeAssigned) idxArr[cnt++]=k;
            for (int a=0;a<cnt-1;a++) for (int b=0;b<cnt-a-1;b++)
                if (d.cou[j].enr.g[idxArr[b]].percentage<d.cou[j].enr.g[idxArr[b+1]].percentage) { int t=idxArr[b]; idxArr[b]=idxArr[b+1]; idxArr[b+1]=t; }
            const QString medals[]=  {"\U0001F947","\U0001F948","\U0001F949","#4","#5"};
            int top=qMin(5,cnt);
            for (int k = 0; k < top; k++) {
                int gi=idxArr[k]; int si=d.cou[j].enr.studentIndex[gi];
                int row=tbl->rowCount(); tbl->insertRow(row); tbl->setRowHeight(row,48);
                tbl->setItem(row,0,new QTableWidgetItem(medals[k]));
                tbl->setItem(row,1,new QTableWidgetItem(d.stu[si].inf.name));
                tbl->setItem(row,2,new QTableWidgetItem(QString("%1/%2").arg(d.cou[j].enr.g[gi].total,0,'f',0).arg(d.cou[j].gr.total,0,'f',0)));
                tbl->setItem(row,3,new QTableWidgetItem(QString("%1%").arg(d.cou[j].enr.g[gi].percentage,0,'f',1)));
                auto* gItm=new QTableWidgetItem(d.cou[j].enr.g[gi].letter);
                gItm->setForeground(QColor(d.cou[j].enr.g[gi].letter.startsWith("A")?DS::SUCCESS:DS::PRIMARY));
                gItm->setTextAlignment(Qt::AlignCenter); tbl->setItem(row,4,gItm);
            }
            break;
        }
    };
    doFill();
    connect(couCombo3, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int){ doFill(); });
    tblLayout->addWidget(tbl);
    mainL->addWidget(makeCard("\U0001F3C6  Top 5 Students", tblLayout, DS::STF_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

#include "MainWindow.h"
  #include "StyleHelpers.h"
  #include <QPainter>
  #include <QPainterPath>
  #include <QLinearGradient>

  // ══════ Animated Bar Chart (grades) ════════════════════════════════════════
  class BarChartWidget : public QWidget {
      struct Bar{QString label;float value,maxValue;QColor color;};
      QVector<Bar> bars; float anim=0.f; QTimer* tmr=nullptr;
  public:
      explicit BarChartWidget(QWidget* p=nullptr):QWidget(p){setMinimumHeight(200);setMaximumHeight(240);}
      void addBar(const QString& lbl,float val,float mx,const QColor& col){bars.append({lbl,val,mx,col});}
      void startAnimation(){
          anim=0.f; tmr=new QTimer(this);
          connect(tmr,&QTimer::timeout,this,[this]{anim+=0.05f;if(anim>=1.f){anim=1.f;tmr->stop();}update();});
          tmr->start(16);
      }
      void paintEvent(QPaintEvent*) override {
          if(bars.isEmpty()) return;
          QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
          bool light=g_lightMode;
          int W=width(),H=height(),padX=24,padY=28,lblH=38,n=bars.size();
          int bw=qMax(18,qMin(50,(W-2*padX)/(n*2))),gap=bw;
          int totalW=n*(bw+gap)-gap,sx=(W-totalW)/2,maxH=H-padY-lblH-20;
          for(int gi=1;gi<=4;gi++){
              int gy=padY+maxH*(4-gi)/4;
              p.setPen(QPen(QColor(light?200:30,light?210:40,light?230:64,80),1,Qt::DashLine));
              p.drawLine(padX,gy,W-padX,gy);
              p.setPen(QColor(light?100:80,light?120:90,light?150:110,130));
              p.setFont(QFont("",8));
              p.drawText(QRect(0,gy-8,padX-2,16),Qt::AlignRight|Qt::AlignVCenter,QString("%1%").arg(25*gi));
          }
          for(int i=0;i<n;i++){
              const auto& b=bars[i];
              float ratio=b.maxValue>0?qMin(b.value/b.maxValue,1.f):0.f;
              int bh=(int)(maxH*ratio*anim),bx=sx+i*(bw+gap),by=padY+maxH-bh;
              p.setPen(Qt::NoPen);
              p.setBrush(QColor(light?220:20,light?228:28,light?245:50,light?180:255));
              p.drawRoundedRect(bx,padY,bw,maxH,4,4);
              if(bh>0){
                  QLinearGradient g2(bx,by,bx,by+bh);
                  g2.setColorAt(0,b.color.lighter(140)); g2.setColorAt(1,b.color);
                  p.setBrush(g2); p.drawRoundedRect(bx,by,bw,bh,4,4);
                  p.setPen(b.color); p.setFont(QFont("",8,QFont::Bold));
                  p.drawText(QRect(bx-gap/2,by-18,bw+gap,18),Qt::AlignCenter,QString::number((int)b.value));
              }
              p.setPen(QColor(light?60:120,light?80:140,light?110:165));
              p.setFont(QFont("",8));
              // Show full name, wrap to 2 lines if needed
              QFontMetrics fm(p.font());
              QString lbl=b.label;
              QRect lblRect(bx-gap,H-lblH,bw+gap*2,lblH);
              p.drawText(lblRect,Qt::AlignHCenter|Qt::AlignTop|Qt::TextWordWrap,lbl);
          }
      }
  };
  

  // ══════ GPA Trend Line Chart ═════════════════════════════════════════════════
  class GpaTrendWidget : public QWidget {
      QVector<float> pts; QStringList labels;
      float anim=0.f; QTimer* tmr=nullptr;
  public:
      explicit GpaTrendWidget(QWidget* p=nullptr):QWidget(p){setMinimumHeight(160);setMaximumHeight(190);}
      void addPoint(const QString& lbl, float val){ labels<<lbl; pts<<val; }
      void startAnimation(){
          anim=0.f; tmr=new QTimer(this);
          connect(tmr,&QTimer::timeout,this,[this]{anim+=0.04f;if(anim>=1.f){anim=1.f;tmr->stop();}update();});
          tmr->start(16);
      }
      void paintEvent(QPaintEvent*) override {
          if(pts.size()<2) return;
          QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
          bool light=g_lightMode;
          int W=width(),H=height(),padX=36,padY=20,lblH=22,n=pts.size();
          int maxH=H-padY-lblH-10;
          float maxV=*std::max_element(pts.begin(),pts.end());
          float minV=*std::min_element(pts.begin(),pts.end());
          float range=qMax(maxV-minV,0.5f);
          // Grid lines
          for(int gi=0;gi<4;gi++){
              float gy=padY+maxH*gi/3.f;
              p.setPen(QPen(QColor(light?200:30,light?210:40,light?230:64,60),1,Qt::DashLine));
              p.drawLine((int)(padX),int(gy),(int)(W-padX),(int)(gy));
          }
          // Build polyline
          QVector<QPointF> polyline;
          int visN=qMax(1,(int)(pts.size()*anim));
          float xStep=(float)(W-2*padX)/qMax(n-1,1);
          for(int i=0;i<visN&&i<n;i++){
              float norm=maxH>0?(pts[i]-minV)/range:0.5f;
              float x=padX+i*xStep;
              float y=padY+maxH*(1.f-norm);
              polyline<<QPointF(x,y);
          }
          if(polyline.size()>1){
              // Fill area under curve
              QPolygonF fill=polyline;
              fill<<QPointF(polyline.last().x(),padY+maxH)<<QPointF(polyline.first().x(),padY+maxH);
              QLinearGradient grad(0,padY,0,padY+maxH);
              grad.setColorAt(0,QColor(0,207,255,60)); grad.setColorAt(1,QColor(0,207,255,0));
              p.setPen(Qt::NoPen); p.setBrush(grad); p.drawPolygon(fill);
              // Line
              p.setPen(QPen(QColor("#00CFFF"),2.5f,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));
              p.setBrush(Qt::NoBrush);
              QPainterPath path; path.moveTo(polyline[0]);
              for(int i=1;i<polyline.size();i++) path.lineTo(polyline[i]);
              p.drawPath(path);
          }
          // Dots + values + labels
          p.setFont(QFont("",8,QFont::Bold));
          for(int i=0;i<visN&&i<n;i++){
              const QPointF& pt=polyline[i];
              p.setPen(Qt::NoPen);
              p.setBrush(QColor("#00CFFF")); p.drawEllipse(pt,5,5);
              p.setBrush(QColor(light ? "#FFFFFF" : "#0C0F1C")); p.drawEllipse(pt,2.5,2.5);
              p.setPen(QColor(light?30:200,light?60:215,light?100:255));
              p.drawText(QRectF(pt.x()-20,pt.y()-20,40,16),Qt::AlignCenter,
                         QString::number(pts[i],'f',1));
              p.setPen(QColor(light?80:120,light?100:140,light?130:165));
              p.setFont(QFont("",7));
              QString lbl=labels[i];
              p.drawText(QRectF(pt.x()-24,H-lblH,48,lblH),Qt::AlignCenter,lbl);
              p.setFont(QFont("",8,QFont::Bold));
          }
      }
  };
  
static QStringList stuSidebarItems() {
    return { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DA  Course Registration",
             "  \U0001F4CA  Grades", "  \U0001F4C5  Request Appointment", "  \U0001F5D3  My Appointments",
             "  \U0001F550  Course Schedule", "  \U0001F4CB  Exam Schedule", "  \U0001F3C6  Honor List",
             "  \U0001F514  Notifications" };
}
static QList<int> stuSidebarPages() {
    return { 2,3,4,5,7,6,8,9,10,22 };
}

// ══════════════════════════════════════════════════════════════════════════════
// STUDENT DASHBOARD
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::studentDashboard()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    calculateGPA(i, d, false);

    int unreadN = countUnreadNotif(d.stu[i].inf.id, 0);
    QString notifLabel = unreadN > 0 ? QString("  \U0001F514  Notifications (%1)").arg(unreadN) : "  \U0001F514  Notifications";

    auto* sidebar = makeSidebar(
        { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DA  Course Registration",
          "  \U0001F4CA  Grades", "  \U0001F4C5  Request Appointment", "  \U0001F5D3  My Appointments",
          "  \U0001F550  Course Schedule", "  \U0001F4CB  Exam Schedule", "  \U0001F3C6  Honor List",
          notifLabel },
        { PAGE_STU_DASH, PAGE_STU_PROFILE, PAGE_STU_COURSEREG,
          PAGE_STU_GRADES, PAGE_STU_REQ_APPT, PAGE_STU_APPT,
          PAGE_STU_COSCH, PAGE_STU_EXSCH, PAGE_STU_HONOR,
          PAGE_STU_NOTIF },
        PAGE_STU_DASH, DS::STU_ACCENT
    );

    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content);
    mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);

    auto* banner = new QWidget;
    banner->setStyleSheet(g_lightMode
        ? "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #FFFFFF,stop:1 #F5F1EA);"
          "border-radius:16px; border:1px solid #E0D8CC;"
        : "background:qlineargradient(x1:0,y1:0,x2:1,y2:0,stop:0 #001830,stop:1 #001020);"
          "border-radius:16px; border:1px solid #00CFFF;");
    addShadow(banner, 28, 6, 30);
    auto* bannerL = new QHBoxLayout(banner);
    bannerL->setContentsMargins(28,22,28,22);
    auto* bannerText = new QVBoxLayout;
    auto* greetLbl = new QLabel(QString("\U0001F44B  Welcome, %1!").arg(d.stu[i].inf.name));
    greetLbl->setStyleSheet(QString("color:%1; font-size:22px; font-weight:800; background:transparent;").arg(DS::TEXT_PRIMARY()));
    auto* subLbl = new QLabel(QString("Department: %1  \u2022  Year %2  \u2022  ID: %3")
        .arg(d.stu[i].department).arg(d.stu[i].academicYear).arg(d.stu[i].inf.id));
    subLbl->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));
    bannerText->addWidget(greetLbl); bannerText->addWidget(subLbl);
    auto* gpaBadge = new QLabel(QString("%1\nGPA").arg(d.stu[i].GPA, 0, 'f', 2));
    gpaBadge->setFixedSize(80,80); gpaBadge->setAlignment(Qt::AlignCenter);
    QString gpaColor = (d.stu[i].GPA >= 3.5f) ? DS::SUCCESS
                     : (d.stu[i].GPA >= 2.5f) ? DS::STU_ACCENT : DS::WARNING;
    gpaBadge->setStyleSheet(QString(
        "background:%1; color:%2; font-size:13px; font-weight:800; border-radius:40px;")
        .arg(gpaColor).arg(g_lightMode ? "#FFFFFF" : "#000A14"));
    bannerL->addLayout(bannerText, 1); bannerL->addWidget(gpaBadge);
    mainL->addWidget(banner);

    auto* statsRow = new QHBoxLayout; statsRow->setSpacing(16);
    int courseCount = 0;
    for (int k = 0; k < 10; k++) if (!d.stu[i].studentCourses[k].isEmpty()) courseCount++;
    statsRow->addWidget(makeStatCard(QString::number(courseCount), "Enrolled Courses", "\U0001F4DA", DS::STU_ACCENT));
    statsRow->addWidget(makeStatCard(d.stu[i].letter.isEmpty() ? "N/A" : d.stu[i].letter, "Letter Grade", "\U0001F3C5", DS::STF_ACCENT));
    statsRow->addWidget(makeStatCard(QString("%1%").arg(d.stu[i].percentage, 0, 'f', 1), "Percentage", "\U0001F4AF", DS::WARNING));
    statsRow->addWidget(makeStatCard(QString::number(unreadN), "Unread Alerts", "\U0001F514", DS::DANGER));
    mainL->addLayout(statsRow);

    auto* coursesL = new QVBoxLayout; coursesL->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,3);
    tbl->setHorizontalHeaderLabels({"Course","Status","Grade"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(tbl);
    for (int j = 0; j < 10; j++) {
        if (d.stu[i].studentCourses[j].isEmpty()) continue;
        int row = tbl->rowCount(); tbl->insertRow(row);
        tbl->setItem(row, 0, new QTableWidgetItem(d.stu[i].studentCourses[j]));
        bool graded = false; QString ltr;
        for (int k = 0; k < d.numOfCou; k++) {
            if (d.cou[k].name != d.stu[i].studentCourses[j]) continue;
            for (int m = 0; m < d.cou[k].studentCount; m++) {
                if (d.cou[k].enr.studentIndex[m] == i) {
                    graded = d.cou[k].enr.g[m].gradeAssigned;
                    ltr    = d.cou[k].enr.g[m].letter; break;
                }
            }
            break;
        }
        auto* stItm = new QTableWidgetItem(graded ? "\u2713 Graded" : "In Progress");
        stItm->setForeground(QColor(graded ? DS::SUCCESS : DS::STU_ACCENT));
        stItm->setTextAlignment(Qt::AlignCenter);
        tbl->setItem(row, 1, stItm);
        auto* grItm = new QTableWidgetItem(graded ? ltr : "\u2014");
        grItm->setTextAlignment(Qt::AlignCenter);
        grItm->setForeground(QColor(ltr.startsWith("A") ? DS::SUCCESS :
                                    ltr.startsWith("B") ? DS::STU_ACCENT :
                                    ltr == "F"          ? DS::DANGER : DS::WARNING));
        tbl->setItem(row, 2, grItm);
    }
    coursesL->addWidget(tbl);
    mainL->addWidget(makeCard("\U0001F4CB  My Courses", coursesL, DS::STU_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// STUDENT PROFILE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuProfilePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9] = QString("  \U0001F514  Notifications (%1)").arg(unreadN);

    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_PROFILE, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content);
    mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F464  My Profile");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);

    const QString valSS = QString("color:%1; font-size:15px; font-weight:600; background:transparent;").arg(DS::TEXT_PRIMARY());
    const QString keyS = "color:#4A6080; font-size:12px; font-weight:700; background:transparent; text-transform:uppercase; letter-spacing:0.5px;";
    auto* infoL = new QVBoxLayout; infoL->setContentsMargins(28,24,28,24); infoL->setSpacing(14);
    auto mkRow = [&](const QString& key, const QString& val) {
        auto* row = new QHBoxLayout;
        auto* k = new QLabel(key); k->setStyleSheet(keyS);
        auto* v = new QLabel(val); v->setStyleSheet(valSS);
        row->addWidget(k,1); row->addWidget(v,2); infoL->addLayout(row);
        auto* sep = new QFrame; sep->setFrameShape(QFrame::HLine); sep->setFrameShadow(QFrame::Plain);
        sep->setStyleSheet(QString("QFrame{border:none; border-top:1px solid %1; background:transparent; max-height:1px;}").arg(DS::BORDER())); infoL->addWidget(sep);
    };
    mkRow("Student ID",    QString::number(d.stu[i].inf.id));
    mkRow("Full Name",     d.stu[i].inf.name);
    mkRow("Mobile",        d.stu[i].inf.mobileNumber);
    mkRow("Address",       d.stu[i].inf.address);
    mkRow("Department",    d.stu[i].department);
    mkRow("Academic Year", QString("Year %1").arg(d.stu[i].academicYear));
    mkRow("GPA",           QString("%1").arg(d.stu[i].GPA, 0, 'f', 2));
    mainL->addWidget(makeCard("\U0001F4CB  Student Information", infoL, DS::STU_ACCENT));

    const QString fs = inputLineStyle(DS::STU_ACCENT);
    const QString ls = labelSS();
    auto mkL3 = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* editLayout = new QVBoxLayout; editLayout->setContentsMargins(28,24,28,24); editLayout->setSpacing(10);
    auto* oldPw  = new QLineEdit; oldPw->setEchoMode(QLineEdit::Password); oldPw->setPlaceholderText("Current password\u2026"); oldPw->setStyleSheet(fs);
    auto* nameE  = new QLineEdit; nameE->setPlaceholderText("New name (optional)\u2026"); nameE->setStyleSheet(fs);
    auto* depE   = new QLineEdit; depE->setText(d.stu[i].department); depE->setStyleSheet(fs);
    auto* mobE   = new QLineEdit; mobE->setText(d.stu[i].inf.mobileNumber); mobE->setStyleSheet(fs);
    auto* addrE  = new QLineEdit; addrE->setText(d.stu[i].inf.address); addrE->setStyleSheet(fs);
    auto* yearSp = new QSpinBox; yearSp->setRange(1,4); yearSp->setValue(d.stu[i].academicYear); yearSp->setStyleSheet(fs);
    auto* newPw  = new QLineEdit; newPw->setEchoMode(QLineEdit::Password); newPw->setPlaceholderText("New password (optional)\u2026"); newPw->setStyleSheet(fs);
    editLayout->addWidget(mkL3("Current Password:")); editLayout->addWidget(oldPw);
    editLayout->addWidget(mkL3("Full Name (optional):")); editLayout->addWidget(nameE);
    editLayout->addWidget(mkL3("Department:")); editLayout->addWidget(depE);
    editLayout->addWidget(mkL3("Academic Year:")); editLayout->addWidget(yearSp);
    editLayout->addWidget(mkL3("Mobile Number:")); editLayout->addWidget(mobE);
    editLayout->addWidget(mkL3("Address:")); editLayout->addWidget(addrE);
    editLayout->addWidget(mkL3("New Password (optional):")); editLayout->addWidget(newPw);
    editLayout->addSpacing(8);
    auto* saveBtn = makeBtn("\U0001F4BE  Save Changes", DS::STU_ACCENT);
    editLayout->addWidget(saveBtn);
    mainL->addWidget(makeCard("\u270F\uFE0F  Edit Profile", editLayout, DS::STU_ACCENT));
    mainL->addStretch();
    connect(saveBtn, &QPushButton::clicked, this, [=]() {
        if (oldPw->text() != d.stu[i].inf.password) { showMsg("Wrong current password!", false); return; }
        if (!nameE->text().trimmed().isEmpty()) d.stu[i].inf.name = nameE->text().trimmed();
        d.stu[i].department = depE->text().trimmed();
        d.stu[i].academicYear = yearSp->value();
        d.stu[i].inf.mobileNumber = mobE->text().trimmed();
        d.stu[i].inf.address = addrE->text().trimmed();
        if (!newPw->text().isEmpty()) d.stu[i].inf.password = newPw->text();
        showMsg("Profile updated!");
        refreshPage(PAGE_STU_PROFILE); stack->setCurrentIndex(PAGE_STU_PROFILE);
    });
    animateFadeIn(content, 250);
    auto* scroll = new QScrollArea; scroll->setWidget(content); scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame); scroll->setStyleSheet(QString("background:%1; border:none;").arg(DS::BG_PAGE()));
    return wrapWithSidebar(sidebar, scroll);
}

// ══════════════════════════════════════════════════════════════════════════════
// COURSE REGISTRATION
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuCourseRegPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9] = QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_COURSEREG, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4DA  Course Registration");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* rowL = new QHBoxLayout; rowL->setSpacing(20);
    auto* avTbl = new QTableWidget(0,3);
    avTbl->setHorizontalHeaderLabels({"Course Name","Credit Hours","Status"});
    avTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    avTbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    styleTable(avTbl);
    for (int j = 0; j < d.numOfCou; j++) {
        if (d.cou[j].academicYear != d.stu[i].academicYear) continue;
        bool reg = false;
        for (int k = 0; k < 10; k++) if (d.stu[i].studentCourses[k] == d.cou[j].name) { reg = true; break; }
        int row = avTbl->rowCount(); avTbl->insertRow(row);
        avTbl->setItem(row,0,new QTableWidgetItem(d.cou[j].name));
        avTbl->setItem(row,1,new QTableWidgetItem(QString("%1 hrs").arg(d.cou[j].creditHours)));
        auto* stItm = new QTableWidgetItem(reg ? "\u2713 Registered" : "Available");
        stItm->setForeground(QColor(reg ? DS::SUCCESS : DS::PRIMARY));
        avTbl->setItem(row,2,stItm);
    }
    auto* availLayout = new QVBoxLayout; availLayout->setContentsMargins(16,16,16,16); availLayout->setSpacing(10);
    availLayout->addWidget(avTbl);
    auto* btnsRow = new QHBoxLayout;
    auto* regBtn  = makeBtn("\u2795  Register Selected", DS::SUCCESS);
    auto* dropBtn = makeBtn("\u2796  Drop Selected", DS::DANGER);
    btnsRow->addWidget(regBtn); btnsRow->addWidget(dropBtn);
    availLayout->addLayout(btnsRow);
    rowL->addWidget(makeCard(QString("\U0001F4CB  Available Courses \u2014 Year %1").arg(d.stu[i].academicYear), availLayout, DS::STU_ACCENT), 3);
    auto* regTbl = new QTableWidget(0,2);
    regTbl->setHorizontalHeaderLabels({"Course","Year"});
    regTbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(regTbl);
    for (int j = 0; j < 10; j++) {
        if (d.stu[i].studentCourses[j].isEmpty()) continue;
        int row = regTbl->rowCount(); regTbl->insertRow(row);
        regTbl->setItem(row,0,new QTableWidgetItem(d.stu[i].studentCourses[j]));
        regTbl->setItem(row,1,new QTableWidgetItem(QString("Year %1").arg(d.stu[i].academicYear)));
    }
    auto* regLayout = new QVBoxLayout; regLayout->setContentsMargins(16,16,16,16); regLayout->addWidget(regTbl);
    rowL->addWidget(makeCard("\u2705  Registered Courses", regLayout, DS::STU_ACCENT), 2);
    mainL->addLayout(rowL); mainL->addStretch();
    connect(regBtn, &QPushButton::clicked, this, [=]() {
        int row = avTbl->currentRow(); if (row < 0) { showMsg("Select a course!", false); return; }
        QString cname = avTbl->item(row,0)->text();
        for (int k = 0; k < 10; k++) if (d.stu[i].studentCourses[k] == cname) { showMsg("Already registered!", false); return; }
        bool done = false;
        for (int k = 0; k < 10; k++) {
            if (d.stu[i].studentCourses[k].isEmpty()) {
                d.stu[i].studentCourses[k] = cname;
                for (int j = 0; j < d.numOfCou; j++) {
                    if (d.cou[j].name == cname) { d.cou[j].enr.studentIndex[d.cou[j].studentCount]=i; d.cou[j].studentCount++; break; }
                }
                done = true; break;
            }
        }
        if (done) { showMsg("Course registered!"); refreshPage(PAGE_STU_COURSEREG); stack->setCurrentIndex(PAGE_STU_COURSEREG); }
        else showMsg("Maximum 10 courses reached!", false);
    });
    connect(dropBtn, &QPushButton::clicked, this, [=]() {
        int row = avTbl->currentRow(); if (row < 0) { showMsg("Select a course!", false); return; }
        QString cname = avTbl->item(row,0)->text();
        if (!confirmDialog("Drop Course",
            QString("Are you sure you want to drop <b>%1</b>?<br>This will remove all grade data for this course.").arg(cname),
            "Yes, Drop It", DS::DANGER)) return;
        bool found = false;
        for (int k = 0; k < 10; k++) {
            if (d.stu[i].studentCourses[k] == cname) {
                for (int j = 0; j < d.numOfCou; j++) {
                    if (d.cou[j].name == cname) {
                        for (int m = 0; m < d.cou[j].studentCount; m++) {
                            if (d.cou[j].enr.studentIndex[m] == i) {
                                for (int n = m; n < d.cou[j].studentCount-1; n++) {
                                    d.cou[j].enr.studentIndex[n] = d.cou[j].enr.studentIndex[n+1];
                                    d.cou[j].enr.g[n] = d.cou[j].enr.g[n+1];
                                }
                                d.cou[j].studentCount--; break;
                            }
                        }
                        break;
                    }
                }
                for (int n = k; n < 9; n++) d.stu[i].studentCourses[n] = d.stu[i].studentCourses[n+1];
                d.stu[i].studentCourses[9] = ""; found = true; break;
            }
        }
        if (found) { showMsg("Course dropped!"); refreshPage(PAGE_STU_COURSEREG); stack->setCurrentIndex(PAGE_STU_COURSEREG); }
        else showMsg("Not registered in this course!", false);
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// STUDENT GRADES
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuGradesPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    calculateGPA(i, d, false);
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_GRADES, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4CA  My Grades");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* gpaRow = new QHBoxLayout; gpaRow->setSpacing(16);
    QString gpaStatus = d.stu[i].GPA >= 3.7f ? "Excellent"
                      : d.stu[i].GPA >= 3.0f ? "Very Good"
                      : d.stu[i].GPA >= 2.0f ? "Good"
                      : "Pass";
    QString gpaLabel = QString("GPA - %1").arg(gpaStatus);
    gpaRow->addWidget(makeStatCard(QString("%1").arg(d.stu[i].GPA,0,'f',2), gpaLabel,"\U0001F4C8",DS::PRIMARY));
    gpaRow->addWidget(makeStatCard(d.stu[i].letter.isEmpty()?"N/A":d.stu[i].letter,"Letter Grade","\U0001F3C5",DS::STF_ACCENT));
    gpaRow->addWidget(makeStatCard(QString("%1%").arg(d.stu[i].percentage,0,'f',1),"Percentage","\U0001F4AF",DS::WARNING));
    gpaRow->addWidget(makeStatCard(QString("%1/%2").arg(d.stu[i].totalMarks,0,'f',0).arg(d.stu[i].couTotalMarks,0,'f',0),"Total Marks","\U0001F4CB",DS::SUCCESS));
    mainL->addLayout(gpaRow);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,7);
    tbl->setHorizontalHeaderLabels({"Course","Final","Practical","Year Work","Quiz","Total","Grade"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int c2 = 1; c2 < 7; c2++) tbl->horizontalHeader()->setSectionResizeMode(c2, QHeaderView::ResizeToContents);
    styleTable(tbl);
    for (int j = 0; j < 10; j++) {
        if (d.stu[i].studentCourses[j].isEmpty()) continue;
        for (int k = 0; k < d.numOfCou; k++) {
            if (d.cou[k].name != d.stu[i].studentCourses[j]) continue;
            for (int m = 0; m < d.cou[k].studentCount; m++) {
                if (d.cou[k].enr.studentIndex[m] != i) continue;
                grade& g = d.cou[k].enr.g[m];
                int row = tbl->rowCount(); tbl->insertRow(row);
                tbl->setItem(row,0,new QTableWidgetItem(d.cou[k].name));
                if (g.gradeAssigned) {
                    tbl->setItem(row,1,new QTableWidgetItem(QString("%1/%2").arg(g.final_,0,'f',0).arg(d.cou[k].gr.finalMax,0,'f',0)));
                    tbl->setItem(row,2,new QTableWidgetItem(QString("%1/%2").arg(g.practical,0,'f',0).arg(d.cou[k].gr.practicalMax,0,'f',0)));
                    tbl->setItem(row,3,new QTableWidgetItem(QString("%1/%2").arg(g.yearWork,0,'f',0).arg(d.cou[k].gr.yearWorkMax,0,'f',0)));
                    tbl->setItem(row,4,new QTableWidgetItem(QString("%1/%2").arg(g.quiz,0,'f',0).arg(d.cou[k].gr.quizMax,0,'f',0)));
                    tbl->setItem(row,5,new QTableWidgetItem(QString("%1/%2").arg(g.total,0,'f',0).arg(d.cou[k].gr.total,0,'f',0)));
                    auto* grItm = new QTableWidgetItem(g.letter); grItm->setTextAlignment(Qt::AlignCenter);
                    grItm->setForeground(QColor(g.letter.startsWith("A")?DS::SUCCESS:g.letter.startsWith("B")?DS::PRIMARY:g.letter=="F"?DS::DANGER:DS::WARNING));
                    tbl->setItem(row,6,grItm);
                } else { for (int c2=1;c2<7;c2++) tbl->setItem(row,c2,new QTableWidgetItem("Pending")); }
                break;
            }
            break;
        }
    }
    tblLayout->addWidget(tbl);
      mainL->addWidget(makeCard("\U0001F4CB  Detailed Grades", tblLayout, DS::STU_ACCENT));
      // Bar chart
      {auto* chart=new BarChartWidget;
       QStringList cols={"#00CFFF","#C040FF","#00FF88","#FFB400","#FF2D6B","#00FFD4","#FF6030","#40CFFF"};
       int ci=0;bool anyGraded=false;
       for(int j=0;j<10;j++){if(d.stu[i].studentCourses[j].isEmpty())continue;
         for(int k=0;k<d.numOfCou;k++){if(d.cou[k].name!=d.stu[i].studentCourses[j])continue;
           for(int m=0;m<d.cou[k].studentCount;m++){if(d.cou[k].enr.studentIndex[m]!=i)continue;
             grade& g2=d.cou[k].enr.g[m];
             if(g2.gradeAssigned){float mx=d.cou[k].gr.total>0?d.cou[k].gr.total:100.f;
               chart->addBar(d.cou[k].name,g2.total,mx,QColor(cols[ci++%cols.size()]));anyGraded=true;}
           }break;}}
       if(anyGraded){auto* chartL=new QVBoxLayout;chartL->setContentsMargins(16,12,16,12);
         chartL->addWidget(chart);mainL->addWidget(makeCard("\U0001F4CA  Grades Bar Chart",chartL,DS::WARNING));
         QTimer::singleShot(120,chart,[chart](){chart->startAnimation();});
       }else{delete chart;
         auto* el=new QVBoxLayout;el->setContentsMargins(16,16,16,16);
         el->addWidget(makeEmptyState("\U0001F4CA","No Grades Yet","Grades appear here after they are assigned."));
         mainL->addWidget(makeCard("\U0001F4CA  Grades Bar Chart",el,DS::WARNING));}}
      mainL->addStretch();
      animateFadeIn(content, 250);
      return wrapWithSidebar(sidebar, content);
  }

// ══════════════════════════════════════════════════════════════════════════════
// REQUEST APPOINTMENT
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuRequestAppointmentPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_REQ_APPT, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4C5  Request Appointment");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* formLayout = new QVBoxLayout; formLayout->setContentsMargins(32,32,32,32); formLayout->setSpacing(16);
    const QString fs = inputComboStyle(DS::STU_ACCENT);
    const QString ls = labelSS();
    auto mkLbl = [&](const QString& t){ auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto* stfCombo = new QComboBox; stfCombo->setStyleSheet(fs);
    for (int j = 0; j < d.numOfStf; j++)
        stfCombo->addItem(QString("%1  \u2014  %2").arg(d.stf[j].inf.name).arg(d.stf[j].position));
    applyComboDelegate(stfCombo);
    auto* daySp  = new QSpinBox; daySp->setRange(1,31);  daySp->setStyleSheet(fs);
    auto* monSp  = new QSpinBox; monSp->setRange(1,12);  monSp->setStyleSheet(fs);
    auto* hourSp = new QSpinBox; hourSp->setRange(8,18); hourSp->setStyleSheet(fs);
    auto* minCom = new QComboBox; minCom->setStyleSheet(fs); minCom->addItems({"00","15","30","45"});
    applyComboDelegate(minCom);
    formLayout->addWidget(mkLbl("Staff Member:")); formLayout->addWidget(stfCombo);
    auto* dateRow = new QHBoxLayout; dateRow->setSpacing(12);
    auto* dayW = new QVBoxLayout; dayW->setSpacing(6); dayW->addWidget(mkLbl("Day (1-31):")); dayW->addWidget(daySp);
    auto* monW = new QVBoxLayout; monW->setSpacing(6); monW->addWidget(mkLbl("Month (1-12):")); monW->addWidget(monSp);
    dateRow->addLayout(dayW); dateRow->addLayout(monW); formLayout->addLayout(dateRow);
    auto* timeRow = new QHBoxLayout; timeRow->setSpacing(12);
    auto* hrW = new QVBoxLayout; hrW->setSpacing(6); hrW->addWidget(mkLbl("Hour (8-18):")); hrW->addWidget(hourSp);
    auto* minW = new QVBoxLayout; minW->setSpacing(6); minW->addWidget(mkLbl("Minute:")); minW->addWidget(minCom);
    timeRow->addLayout(hrW); timeRow->addLayout(minW); formLayout->addLayout(timeRow);
    auto* reqBtn = makeBtn("\U0001F4C5  Send Appointment Request", DS::STU_ACCENT);
    reqBtn->setFixedHeight(52);
    formLayout->addSpacing(8); formLayout->addWidget(reqBtn);
    mainL->addWidget(makeCard("\U0001F4C5  Schedule an Appointment", formLayout, DS::STU_ACCENT));
    mainL->addStretch();
    connect(reqBtn, &QPushButton::clicked, this, [=]() {
        if (d.numOfApp >= SIZE) { showMsg("Maximum appointments reached!", false); return; }
        int stfIdx = stfCombo->currentIndex();
        int day=daySp->value(), mon=monSp->value(), hour=hourSp->value();
        int min=minCom->currentText().toInt();
        for (int j = 0; j < d.numOfApp; j++) {
            if (d.app[j].stfIndex==stfIdx && d.app[j].sch.day==day && d.app[j].sch.month==mon &&
                d.app[j].sch.hour==hour && d.app[j].sch.minute==min) {
                showMsg("This time slot is already taken!", false); return;
            }
        }
        appointment& ap = d.app[d.numOfApp];
        ap.studentID=d.stu[i].inf.id; ap.studentName=d.stu[i].inf.name;
        ap.stfIndex=stfIdx; ap.stfName=d.stf[stfIdx].inf.name;
        ap.status="Pending"; ap.sch.day=day; ap.sch.month=mon; ap.sch.hour=hour; ap.sch.minute=min;
        d.numOfApp++;
        addNotification(d.stf[stfIdx].inf.id, 1,
            QString("\U0001F4C5 New appointment request from %1 on %2/%3 at %4:%5")
                .arg(d.stu[i].inf.name).arg(day).arg(mon)
                .arg(hour,2,10,QChar('0')).arg(min,2,10,QChar('0')),
            "\U0001F4C5", DS::STU_ACCENT, d);
        showMsg("Appointment request sent successfully!");
    });
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// MY APPOINTMENTS
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuAppointmentsPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_APPT, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F5D3  My Appointments");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Staff Member","Position","Date","Time","Status"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tbl->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    tbl->setColumnWidth(4, 130);
    styleTable(tbl);
    for (int j = 0; j < d.numOfApp; j++) {
        if (d.app[j].studentID != d.stu[i].inf.id) continue;
        int row = tbl->rowCount(); tbl->insertRow(row); tbl->setRowHeight(row, 52);
        tbl->setItem(row,0,new QTableWidgetItem(d.app[j].stfName));
        tbl->setItem(row,1,new QTableWidgetItem(d.stf[d.app[j].stfIndex].position));
        tbl->setItem(row,2,new QTableWidgetItem(QString("%1/%2").arg(d.app[j].sch.day).arg(d.app[j].sch.month)));
        tbl->setItem(row,3,new QTableWidgetItem(QString("%1:%2").arg(d.app[j].sch.hour,2,10,QChar('0')).arg(d.app[j].sch.minute,2,10,QChar('0'))));
        tbl->setCellWidget(row,4,makeStatusChip(d.app[j].status));
        // Row background tint based on status
        QColor rowBg = (d.app[j].status=="Approved")
            ? (g_lightMode ? QColor(0,180,100,25)  : QColor(0,255,136,18))
            : (d.app[j].status=="Rejected")
            ? (g_lightMode ? QColor(200,0,60,20)   : QColor(255,45,107,18))
            : (g_lightMode ? QColor(160,110,0,25)  : QColor(255,180,0,18));
        for (int c = 0; c < 4; c++) if (tbl->item(row,c)) tbl->item(row,c)->setBackground(rowBg);
    }
    tblLayout->addWidget(tbl);
    mainL->addWidget(makeCard("\U0001F4CB  Your Appointment Requests", tblLayout, DS::STU_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// COURSE SCHEDULE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuCourseSchedulePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_COSCH, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F550  Course Schedule");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,4);
    tbl->setHorizontalHeaderLabels({"Course","Day","Time","Credit Hours"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(tbl);
    for (int j = 0; j < d.numOfCou; j++) {
        if (d.cou[j].academicYear != d.stu[i].academicYear) continue;
        if (!d.cou[j].schedule.isSet) continue;
        int row = tbl->rowCount(); tbl->insertRow(row);
        tbl->setItem(row,0,new QTableWidgetItem(d.cou[j].name));
        tbl->setItem(row,1,new QTableWidgetItem(getDayName(d.cou[j].schedule.weekDay)));
        tbl->setItem(row,2,new QTableWidgetItem(QString("%1:%2").arg(d.cou[j].schedule.hour,2,10,QChar('0')).arg(d.cou[j].schedule.minute,2,10,QChar('0'))));
        tbl->setItem(row,3,new QTableWidgetItem(QString("%1 hrs").arg(d.cou[j].creditHours)));
    }
    tblLayout->addWidget(tbl);
    mainL->addWidget(makeCard(QString("\U0001F4CB  Year %1 \u2014 Course Schedule").arg(d.stu[i].academicYear), tblLayout, DS::STU_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// EXAM SCHEDULE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuExamSchedulePage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_EXSCH, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F4CB  Exam Schedule");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,4);
    tbl->setHorizontalHeaderLabels({"Course","Year","Date","Time"});
    tbl->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    styleTable(tbl);
    for (int j = 0; j < d.numOfExm; j++) {
        if (d.exm[j].co.academicYear != d.stu[i].academicYear) continue;
        int row = tbl->rowCount(); tbl->insertRow(row);
        tbl->setItem(row,0,new QTableWidgetItem(d.exm[j].co.name));
        tbl->setItem(row,1,new QTableWidgetItem(QString("Year %1").arg(d.exm[j].co.academicYear)));
        tbl->setItem(row,2,new QTableWidgetItem(QString("%1/%2/%3").arg(d.exm[j].sch.day).arg(d.exm[j].sch.month).arg(d.exm[j].sch.year)));
        tbl->setItem(row,3,new QTableWidgetItem(QString("%1:%2").arg(d.exm[j].sch.hour,2,10,QChar('0')).arg(d.exm[j].sch.minute,2,10,QChar('0'))));
    }
    tblLayout->addWidget(tbl);
    mainL->addWidget(makeCard("\U0001F4CB  My Exam Schedule", tblLayout, DS::STU_ACCENT));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

// ══════════════════════════════════════════════════════════════════════════════
// HONOR LIST
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::stuHonorListPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id,0);
    QStringList items = stuSidebarItems();
    if (unreadN>0) items[9]=QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSidebarPages(), PAGE_STU_HONOR, DS::STU_ACCENT);
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(24);
    auto* title = new QLabel("\U0001F3C6  Honor List");
    title->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    mainL->addWidget(title);
    auto* tblLayout = new QVBoxLayout; tblLayout->setContentsMargins(16,16,16,16);
    auto* tbl = new QTableWidget(0,5);
    tbl->setHorizontalHeaderLabels({"Rank","Student Name","Department","GPA","Grade"});
    tbl->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    styleTable(tbl);
    int myYear = d.stu[i].academicYear;
    int idxArr[SIZE]; int cnt = 0;
    for (int j = 0; j < d.numOfStu; j++)
        if (d.stu[j].GPA >= 3.5f && d.stu[j].GPA <= 4.0f && d.stu[j].academicYear == myYear)
            idxArr[cnt++] = j;
    for (int a = 0; a < cnt-1; a++)
        for (int b = 0; b < cnt-a-1; b++)
            if (d.stu[idxArr[b]].GPA < d.stu[idxArr[b+1]].GPA) { int t=idxArr[b]; idxArr[b]=idxArr[b+1]; idxArr[b+1]=t; }
    const QString medals[] = {"\U0001F947","\U0001F948","\U0001F949","#4","#5","#6","#7","#8","#9","#10"};
    for (int k = 0; k < cnt; k++) {
        int si = idxArr[k];
        int row = tbl->rowCount(); tbl->insertRow(row); tbl->setRowHeight(row,48);
        tbl->setItem(row,0,new QTableWidgetItem(k<10?medals[k]:QString("#%1").arg(k+1)));
        tbl->setItem(row,1,new QTableWidgetItem(d.stu[si].inf.name));
        tbl->setItem(row,2,new QTableWidgetItem(d.stu[si].department));
        tbl->setItem(row,3,new QTableWidgetItem(QString("%1").arg(d.stu[si].GPA,0,'f',2)));
        auto* gItm = new QTableWidgetItem(d.stu[si].letter);
        gItm->setForeground(QColor(DS::SUCCESS)); gItm->setTextAlignment(Qt::AlignCenter);
        tbl->setItem(row,4,gItm);
    }
    tblLayout->addWidget(tbl);
    mainL->addWidget(makeCard(
        QString("\U0001F3C6  Honor List \u2014 Year %1  \u2014  %2 Students").arg(myYear).arg(cnt),
        tblLayout, DS::WARNING));
    mainL->addStretch();
    animateFadeIn(content, 250);
    return wrapWithSidebar(sidebar, content);
}

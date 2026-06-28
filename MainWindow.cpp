#include "MainWindow.h"
  #include "StyleHelpers.h"
  #include "ForgotPasswordDialog.h"
  #include <QScrollArea>
  #include <QRegularExpression>
  #include <QRandomGenerator>
  #include <cmath>

  bool g_lightMode = false;

  // ══════ Animated Particle Background (login) ════════════════════════════════
  class ParticleWidget : public QWidget {
      struct Particle { float x,y,vx,vy,r,a; };
      QVector<Particle> pts; QTimer* tmr;
      void reset(Particle& p,bool init=false){
          p.x=(float)QRandomGenerator::global()->bounded(qMax(width(),400));
          p.y=init?(float)QRandomGenerator::global()->bounded(qMax(height(),600)):(float)qMax(height(),600)+10.f;
          p.vx=(QRandomGenerator::global()->bounded(100)-50)/120.f;
          p.vy=-(float)(QRandomGenerator::global()->bounded(40)+15)/100.f;
          p.r=QRandomGenerator::global()->bounded(25)/10.f+1.5f;
          p.a=(float)(QRandomGenerator::global()->bounded(60)+25);
      }
  public:
      explicit ParticleWidget(QWidget* parent=nullptr):QWidget(parent){
          setAttribute(Qt::WA_TransparentForMouseEvents);
          pts.resize(55); for(auto& p:pts) reset(p,true);
          tmr=new QTimer(this);
          connect(tmr,&QTimer::timeout,this,[this]{
              for(auto& p:pts){p.x+=p.vx;p.y+=p.vy;if(p.y<-10||p.x<-10||p.x>width()+10)reset(p);}
              update();
          });
          tmr->start(16);
      }
      void paintEvent(QPaintEvent*) override {
          QPainter painter(this); painter.setRenderHint(QPainter::Antialiasing);
          painter.setPen(Qt::NoPen);
          for(const auto& p:pts){painter.setBrush(QColor(0,207,255,(int)p.a));painter.drawEllipse(QPointF(p.x,p.y),p.r,p.r);}
          for(int i=0;i<pts.size();++i) for(int j=i+1;j<pts.size();++j){
              float dx=pts[i].x-pts[j].x,dy=pts[i].y-pts[j].y,d=std::sqrt(dx*dx+dy*dy);
              if(d<90.f){painter.setPen(QPen(QColor(0,207,255,(int)((1.f-d/90.f)*22)),0.6f));
                  painter.drawLine(QPointF(pts[i].x,pts[i].y),QPointF(pts[j].x,pts[j].y));}
          }
      }
  };

  // ══════ Hover Glow Effect for Stat Cards ════════════════════════════════════
  class CardHoverEffect : public QObject {
      QWidget* target; QGraphicsDropShadowEffect* shadow; QString normalSS,hoverSS;
  public:
      CardHoverEffect(QWidget* t,QGraphicsDropShadowEffect* s,const QString& nss,const QString& hss)
          :QObject(t),target(t),shadow(s),normalSS(nss),hoverSS(hss){t->installEventFilter(this);}
      bool eventFilter(QObject* obj,QEvent* e) override {
          if(obj!=target) return false;
          if(e->type()==QEvent::Enter){
              target->setStyleSheet(hoverSS);
              if(shadow){shadow->setBlurRadius(38);shadow->setOffset(0,10);shadow->setColor(QColor(0,207,255,80));}
          } else if(e->type()==QEvent::Leave){
              target->setStyleSheet(normalSS);
              if(shadow){shadow->setBlurRadius(24);shadow->setOffset(0,6);shadow->setColor(QColor(0,200,255,30));}
          }
          return false;
      }
  };

  MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    initData(d);
    loadData(d);
    stack = new QStackedWidget(this);

    stack->addWidget(loginPage());
    stack->addWidget(signUpPage());
    stack->addWidget(studentDashboard());
    stack->addWidget(stuProfilePage());
    stack->addWidget(stuCourseRegPage());
    stack->addWidget(stuGradesPage());
    stack->addWidget(stuAppointmentsPage());
    stack->addWidget(stuRequestAppointmentPage());
    stack->addWidget(stuCourseSchedulePage());
    stack->addWidget(stuExamSchedulePage());
    stack->addWidget(stuHonorListPage());
    stack->addWidget(staffDashboard());
    stack->addWidget(stfProfilePage());
    stack->addWidget(stfSetGradesPage());
    stack->addWidget(stfCoursesPage());
    stack->addWidget(stfAppointmentsPage());
    stack->addWidget(stfTopStudentsPage());
    stack->addWidget(adminDashboard());
    stack->addWidget(admProfilePage());
    stack->addWidget(admCoursesPage());
    stack->addWidget(admCourseSchedulePage());
    stack->addWidget(admExamSchedulePage());
    stack->addWidget(stuNotificationsPage());
    stack->addWidget(stfNotificationsPage());
    stack->addWidget(admNotificationsPage());

    setCentralWidget(stack);
    stack->setCurrentIndex(PAGE_LOGIN);
}

// ══════════════════════════════════════════════════════════════════════════════
// SHARED HELPERS
// ══════════════════════════════════════════════════════════════════════════════
void MainWindow::addShadow(QWidget* w, int blur, int dy, int alpha)
{
    auto* eff = new QGraphicsDropShadowEffect(w);
    eff->setBlurRadius(blur);
    eff->setOffset(0, dy);
    eff->setColor(g_lightMode ? QColor(61, 125, 255, alpha / 3) : QColor(0, 200, 255, alpha));
    w->setGraphicsEffect(eff);
}

void MainWindow::animateFadeIn(QWidget* /*w*/, int /*durationMs*/) {}

QLineEdit* MainWindow::makeField(const QString& placeholder, bool password)
{
    auto* e = new QLineEdit;
    if (password) e->setEchoMode(QLineEdit::Password);
    e->setPlaceholderText(placeholder);
    e->setFixedHeight(46);
    e->setStyleSheet(QString(
        "QLineEdit{ border:2px solid %1; border-radius:10px;"
        "  padding:0 44px 0 14px; font-size:14px; color:%2; background:%3; }"
        "QLineEdit:focus{ border-color:%4; background:%5; }"
        "QLineEdit:hover{ border-color:%6; }")
        .arg(DS::BORDER())
        .arg(DS::TEXT_PRIMARY())
        .arg(DS::BG_INPUT())
        .arg(DS::BORDER_FOCUS())
        .arg(DS::BG_INPUT_FOCUS())
        .arg(g_lightMode ? "#B0C0E8" : "#2A3860"));
    if (password) {
        auto* container = new QWidget(e->parentWidget());
        auto* eyeBtn = new QPushButton("👁", e);
        eyeBtn->setFixedSize(32, 32);
        eyeBtn->setFlat(true);
        eyeBtn->setCursor(Qt::PointingHandCursor);
        eyeBtn->setStyleSheet(QString("QPushButton{ background:transparent; border:none; font-size:16px; color:%1; }"
                              "QPushButton:hover{ color:%2; }")
                              .arg(DS::TEXT_MUTED()).arg(DS::BORDER_FOCUS()));
        eyeBtn->move(e->width() - 40, (46 - 32) / 2);
        QObject::connect(e, &QLineEdit::textChanged, eyeBtn, [e, eyeBtn](const QString&) {
            eyeBtn->move(e->width() - 40, (e->height() - 32) / 2);
        });
        QObject::connect(eyeBtn, &QPushButton::clicked, e, [e, eyeBtn]() {
            if (e->echoMode() == QLineEdit::Password) {
                e->setEchoMode(QLineEdit::Normal);
                eyeBtn->setText("🙈");
            } else {
                e->setEchoMode(QLineEdit::Password);
                eyeBtn->setText("👁");
            }
        });
        QObject::connect(e, &QLineEdit::returnPressed, eyeBtn, [e, eyeBtn]() {
            eyeBtn->move(e->width() - 40, (e->height() - 32) / 2);
        });
    }
    return e;
}

QLabel* MainWindow::makeLabel(const QString& text, int ptSize, bool bold, const QString& color)
{
    auto* l = new QLabel(text);
    l->setStyleSheet(QString("color:%1; font-size:%2px; %3 background:transparent;")
                         .arg(color).arg(ptSize)
                         .arg(bold ? "font-weight:700;" : ""));
    return l;
}

QPushButton* MainWindow::makeBtn(const QString& text, const QString& color)
{
    auto* b = new QPushButton(text);
    b->setFixedHeight(46);
    b->setCursor(Qt::PointingHandCursor);
    QColor c(color);
    QString hover     = c.lighter(115).name();
    QString pressed   = c.darker(115).name();
    QString textColor = (c.lightness() > 160) ? "#0D1B4B" : "white";
    if (g_lightMode) {
        b->setStyleSheet(QString(
            "QPushButton{ background:qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            "  stop:0 %2, stop:1 %1);"
            "  color:%4; border:none; border-radius:10px;"
            "  font-size:14px; font-weight:700; letter-spacing:0.3px; }"
            "QPushButton:hover{ background:qlineargradient(x1:0,y1:0,x2:0,y2:1,"
            "  stop:0 %2, stop:1 %2); }"
            "QPushButton:pressed{ background:%3; padding-top:2px; }")
            .arg(color).arg(hover).arg(pressed).arg(textColor));
    } else {
        b->setStyleSheet(QString(
            "QPushButton{ background:%1; color:%4; border:none; border-radius:10px;"
            "  font-size:14px; font-weight:700; letter-spacing:0.3px; }"
            "QPushButton:hover{ background:%2; }"
            "QPushButton:pressed{ background:%3; padding-top:2px; }")
            .arg(color).arg(hover).arg(pressed).arg(textColor));
    }
    return b;
}

QPushButton* MainWindow::makeOutlineBtn(const QString& text, const QString& color)
{
    auto* b = new QPushButton(text);
    b->setFixedHeight(42);
    b->setCursor(Qt::PointingHandCursor);
    QColor c(color);
    QString textColor = (c.lightness() > 160) ? "#000A14" : color;
    b->setStyleSheet(QString(
        "QPushButton{ background:transparent; color:%1; border:2px solid %1;"
        "  border-radius:10px; font-size:13px; font-weight:600; }"
        "QPushButton:hover{ background:%1; color:%2; }")
        .arg(color).arg(textColor == color ? "#000A14" : "white"));
    return b;
}

QWidget* MainWindow::makeStatCard(const QString& value, const QString& label,
                                   const QString& icon, const QString& color)
{
    auto* card = new QWidget;
    card->setMinimumHeight(110);
    card->setObjectName("statCard");
    card->setStyleSheet(QString(
        "QWidget#statCard{ background:%1; border-radius:16px;"
        "  border:1px solid %2; border-left:4px solid %3; }"
        "QLabel{ border:none; background:transparent; }")
        .arg(DS::BG_CARD()).arg(DS::BORDER()).arg(color));
    addShadow(card, g_lightMode ? 14 : 20, g_lightMode ? 2 : 4, g_lightMode ? 8 : 20);
    addShadow(card, g_lightMode ? 18 : 24, g_lightMode ? 3 : 6, g_lightMode ? 12 : 30);
    auto* L = new QHBoxLayout(card);
    L->setContentsMargins(20,18,20,18);
    QColor ic(color);
    // In light mode use a softer, more opaque icon background
    QString iconBg = g_lightMode
        ? QString("rgba(%1,%2,%3,18)").arg(ic.red()).arg(ic.green()).arg(ic.blue())
        : QString("rgba(%1,%2,%3,30)").arg(ic.red()).arg(ic.green()).arg(ic.blue());
    auto* iconLbl = new QLabel(icon);
    iconLbl->setFixedSize(52,52);
    iconLbl->setAlignment(Qt::AlignCenter);
    iconLbl->setStyleSheet(QString("background:%1; border-radius:14px; font-size:22px;").arg(iconBg));
    auto* textW = new QWidget; textW->setStyleSheet("background:transparent;");
    auto* textL = new QVBoxLayout(textW);
    textL->setContentsMargins(0,0,0,0); textL->setSpacing(4);
    auto* valLbl = new QLabel(value);
    // In light mode use a slightly darker variant of the accent for readability
    QString valColor = g_lightMode ? QColor(ic).darker(110).name() : color;
    valLbl->setStyleSheet(QString("color:%1; font-size:22px; font-weight:800; background:transparent;").arg(valColor));
    auto* lblLbl = new QLabel(label);
    lblLbl->setStyleSheet(QString(
        "color:%1; font-size:11px; font-weight:600;"
        "background:transparent; letter-spacing:0.5px;").arg(DS::TEXT_SECOND()));
    textL->addWidget(valLbl); textL->addWidget(lblLbl);
    L->addWidget(iconLbl); L->addSpacing(14); L->addWidget(textW); L->addStretch();
      // Hover effect
      QString normalSS = card->styleSheet();
      QString hoverSS = QString(
          "QWidget#statCard{ background:%1; border-radius:16px; border:1px solid %2; border-left:4px solid %2; }"
          "QLabel{ border:none; background:transparent; }").arg(DS::BG_CARD()).arg(color);
      auto* gfx = qobject_cast<QGraphicsDropShadowEffect*>(card->graphicsEffect());
      new CardHoverEffect(card, gfx, normalSS, hoverSS);
      card->setCursor(Qt::PointingHandCursor);
      return card;
}

QWidget* MainWindow::makeCard(const QString& title, QLayout* layout, const QString& accentColor)
{
    auto* card = new QWidget;
    card->setObjectName("mainCard");
    card->setStyleSheet(QString(
        "QWidget#mainCard{ background:%1; border-radius:16px; border:1px solid %2; }"
        "QLabel{ border:none; background:transparent; }")
        .arg(DS::BG_CARD()).arg(DS::BORDER()));
    addShadow(card, g_lightMode ? 18 : 24, g_lightMode ? 3 : 6, g_lightMode ? 12 : 25);
    auto* outerL = new QVBoxLayout(card);
    outerL->setContentsMargins(0,0,0,0); outerL->setSpacing(0);
    auto* titleBar = new QWidget; titleBar->setFixedHeight(52);
    QColor ac(accentColor);
    QString titleBg;
    if (g_lightMode) {
        titleBg = "transparent";
    } else {
        titleBg = QString("rgba(%1,%2,%3,30)").arg(ac.red()).arg(ac.green()).arg(ac.blue());
    }
    if (g_lightMode) {
        titleBar->setStyleSheet(QString(
            "background:#FFFFFF;"
            "border-top-left-radius:16px; border-top-right-radius:16px;"
            "border-bottom:2px solid %1;")
            .arg(accentColor));
    } else {
        titleBar->setStyleSheet(QString("background:%1; border-radius:16px; border-bottom:1px solid %2;")
            .arg(titleBg).arg(DS::BORDER()));
    }
    auto* titleL = new QHBoxLayout(titleBar);
    titleL->setContentsMargins(20,0,20,0);
    auto* titleLbl = new QLabel(title);
    QString titleColor = accentColor;
    titleLbl->setStyleSheet(QString(
        "color:%1; font-size:14px; font-weight:700; background:transparent;").arg(titleColor));
    titleL->addWidget(titleLbl);
    auto* body = new QWidget;
    body->setStyleSheet(QString(
        "background:%1; border-bottom-left-radius:16px; border-bottom-right-radius:16px;"
    ).arg(DS::BG_CARD()));
    body->setLayout(layout);
    outerL->addWidget(titleBar); outerL->addWidget(body);
    return card;
}

QFrame* MainWindow::makeSidebar(const QStringList& items,
                                 const QList<int>& pages,
                                 int activeIdx,
                                 const QString& accent)
{
    auto* sidebar = new QFrame;
    sidebar->setFixedWidth(220);
    QColor ac(accent);
    if (g_lightMode) {
        sidebar->setStyleSheet(QString(
            "QFrame{ background:#FFFFFF;"
            "  border:none; border-right:1px solid %1; }").arg(DS::BORDER()));
    } else {
        sidebar->setStyleSheet(QString(
            "QFrame{ background:%1; border:none; border-right:1px solid %2; }")
            .arg(DS::BG_SIDEBAR()).arg(DS::BORDER()));
    }
    auto* L = new QVBoxLayout(sidebar);
    L->setContentsMargins(0,0,0,0); L->setSpacing(0);

    // ── Logo strip ────────────────────────────────────────────────────────────
    auto* logoStrip = new QWidget;
    logoStrip->setFixedHeight(72);
    logoStrip->setStyleSheet(QString(
        "background:%1; border-bottom:1px solid %2;")
        .arg(DS::SIDEBAR_LOGO_BG()).arg(DS::BORDER()));
    auto* logoL = new QHBoxLayout(logoStrip);
    logoL->setContentsMargins(16,0,16,0);
    auto* logoIcon = new QLabel("\U0001F393");
    logoIcon->setStyleSheet("font-size:22px; background:transparent;");
    auto* logoText = new QLabel("College\nSystem");
    logoText->setStyleSheet(QString(
        "color:%1; font-size:12px; font-weight:700; background:transparent;").arg(accent));
    logoL->addWidget(logoIcon); logoL->addSpacing(8); logoL->addWidget(logoText); logoL->addStretch();
    L->addWidget(logoStrip);
    L->addSpacing(8);

    // ── Nav items ─────────────────────────────────────────────────────────────
    for (int i = 0; i < items.size(); i++) {
        QString itemText = items[i];
        int badgeNum = 0;
        QString cleanText = itemText;
        QRegularExpression rx("\\((\\d+)\\)\\s*$");
        QRegularExpressionMatch m = rx.match(itemText);
        if (m.hasMatch()) {
            badgeNum = m.captured(1).toInt();
            cleanText = itemText.left(m.capturedStart()).trimmed();
        }

        auto* btnContainer = new QWidget;
        btnContainer->setFixedHeight(46);
        btnContainer->setStyleSheet("background:transparent; border:none;");
        auto* btnLayout = new QHBoxLayout(btnContainer);
        btnLayout->setContentsMargins(0,0,0,0); btnLayout->setSpacing(0);

        bool isActive = (pages[i] == activeIdx);
        auto* btn = new QPushButton(cleanText);
        btn->setFixedHeight(46);
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        QColor ac2(accent);
        if (isActive) {
            btn->setStyleSheet(QString(
                "QPushButton{ text-align:left; padding:0 18px;"
                "  color:%1;"
                "  background:rgba(%2,%3,%4,%5);"
                "  border:none; border-left:3px solid %1;"
                "  font-size:13px; font-weight:700; border-radius:0; }"
                "QPushButton:hover{ background:rgba(%2,%3,%4,%6); }")
                .arg(accent)
                .arg(ac2.red()).arg(ac2.green()).arg(ac2.blue())
                .arg(g_lightMode ? 20 : 35)
                .arg(g_lightMode ? 32 : 55));
        } else {
            btn->setStyleSheet(QString(
                "QPushButton{ text-align:left; padding:0 20px;"
                "  color:%1; background:transparent; border:none;"
                "  font-size:13px; font-weight:500; border-radius:0; }"
                "QPushButton:hover{ color:%2; background:%3; }")
                .arg(g_lightMode ? DS::TEXT_SECOND() : "rgba(200,216,240,0.55)")
                .arg(g_lightMode ? DS::TEXT_PRIMARY() : "#E2EEFF")
                .arg(g_lightMode ? "rgba(67,97,238,0.07)" : "rgba(61,125,255,0.06)"));
        }

        if (badgeNum > 0) {
            auto* badge = new QLabel(QString::number(badgeNum));
            badge->setFixedSize(20, 20);
            badge->setAlignment(Qt::AlignCenter);
            badge->setStyleSheet(
                "background:#FF2D6B; color:white; border-radius:10px;"
                " font-size:10px; font-weight:800; border:none;");
            badge->setObjectName("notifBadge");
            btnLayout->addWidget(btn, 1);
            btnLayout->addWidget(badge);
            btnLayout->addSpacing(12);
        } else {
            btnLayout->addWidget(btn, 1);
        }

        int pageIdx = pages[i];
        connect(btn, &QPushButton::clicked, this, [this, pageIdx]() {
            refreshPage(pageIdx);
            stack->setCurrentIndex(pageIdx);
        });
        L->addWidget(btnContainer);
    }

    L->addStretch();

    // ── Footer ────────────────────────────────────────────────────────────────
    QString footerSS = QString(
        "QPushButton{text-align:left;padding:0 20px;color:%1;background:transparent;"
        "border:none;font-size:12px;font-weight:500;border-top:1px solid %2;}"
        "QPushButton:hover{color:%3;background:%4;}")
        .arg(DS::TEXT_MUTED(), DS::BORDER())
        .arg(g_lightMode ? DS::TEXT_PRIMARY() : "#00CFFF")
        .arg(g_lightMode ? "rgba(0,0,0,0.04)" : "rgba(0,136,221,0.08)");

    auto* themeBtn = new QPushButton(g_lightMode ? "  \U0001F319  Dark Mode" : "  \u2600  Light Mode");
    themeBtn->setFixedHeight(44); themeBtn->setFlat(true); themeBtn->setCursor(Qt::PointingHandCursor);
    themeBtn->setStyleSheet(footerSS);
    connect(themeBtn,&QPushButton::clicked,this,[this](){ toggleTheme(); });
    L->addWidget(themeBtn);

    auto* signOut = new QPushButton("  \u23FB  Sign Out");
    signOut->setFixedHeight(48);
    signOut->setFlat(true);
    signOut->setCursor(Qt::PointingHandCursor);
    signOut->setStyleSheet(QString(
        "QPushButton{ text-align:left; padding:0 20px; color:%1;"
        "  background:transparent; border:none; font-size:13px; font-weight:500;"
        "  border-top:1px solid %2; }"
        "QPushButton:hover{ color:#E8003A; background:rgba(232,0,58,0.07); }")
        .arg(DS::TEXT_MUTED())
        .arg(DS::BORDER()));
    connect(signOut, &QPushButton::clicked, this, [this]() {
        d.loggedInIndex = -1; d.loggedInType = -1;
        stack->setCurrentIndex(PAGE_LOGIN);
    });
    L->addWidget(signOut);
    return sidebar;
}

QWidget* MainWindow::wrapWithSidebar(QWidget* sidebar, QWidget* content)
{
    auto* w = new QWidget;
    auto* L = new QHBoxLayout(w);
    L->setContentsMargins(0,0,0,0); L->setSpacing(0);
    L->addWidget(sidebar); L->addWidget(content, 1);
    return w;
}

void MainWindow::showMsg(const QString& msg, bool success)
{
    auto* dlg = new QDialog(this);
    dlg->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dlg->setAttribute(Qt::WA_TranslucentBackground);
    dlg->setFixedWidth(380);
    auto* outer = new QVBoxLayout(dlg);
    outer->setContentsMargins(16,16,16,16);

    const QString color  = success ? DS::SUCCESS : DS::DANGER;
    const QString bgCol  = g_lightMode
        ? (success ? "#F0FFF8" : "#FFF0F4")
        : (success ? DS::SUCCESS_BG : DS::DANGER_BG);
    const QString iconTx = success ? "\u2714" : "\u26A0";

    auto* card = new QWidget;
    card->setStyleSheet(QString(
        "background:%1;"
        " border-radius:14px;"
        " border:2px solid %2;")
        .arg(bgCol).arg(color));

    // Glow effect via drop shadow
    auto* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 0);
    shadow->setColor(success
        ? QColor(0, 255, 136, 90)
        : QColor(255, 45, 107, 90));
    card->setGraphicsEffect(shadow);

    auto* cL = new QHBoxLayout(card);
    cL->setContentsMargins(20,16,14,16);
    cL->setSpacing(0);

    auto* icon = new QLabel(iconTx);
    icon->setStyleSheet(QString(
        "font-size:20px; font-weight:900; color:%1; background:transparent; border:none;")
        .arg(color));

    auto* lbl = new QLabel(msg);
    lbl->setWordWrap(true);
    lbl->setStyleSheet(QString(
        "color:%1; font-size:14px; font-weight:600; background:transparent; border:none;")
        .arg(color));

    auto* closeBtn = new QPushButton("\u2715");
    closeBtn->setFixedSize(28,28);
    closeBtn->setStyleSheet(QString(
        "QPushButton{ background:transparent; color:%1; border:none; font-size:13px; font-weight:700; }"
        "QPushButton:hover{ background:rgba(255,255,255,0.08); border-radius:14px; }")
        .arg(color));
    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);

    cL->addWidget(icon); cL->addSpacing(10);
    cL->addWidget(lbl, 1); cL->addWidget(closeBtn, 0, Qt::AlignTop);
    outer->addWidget(card);
    QTimer::singleShot(3200, dlg, &QDialog::accept);
    dlg->exec();
}

void MainWindow::refreshPage(int idx)
{
    QWidget* old   = stack->widget(idx);
    QWidget* fresh = nullptr;
    switch (idx) {
    case PAGE_LOGIN:        fresh = loginPage();                  break;
    case PAGE_SIGNUP:       fresh = signUpPage();                 break;
    case PAGE_STU_DASH:     fresh = studentDashboard();           break;
    case PAGE_STU_PROFILE:  fresh = stuProfilePage();             break;
    case PAGE_STU_COURSEREG:fresh = stuCourseRegPage();           break;
    case PAGE_STU_GRADES:   fresh = stuGradesPage();              break;
    case PAGE_STU_APPT:     fresh = stuAppointmentsPage();        break;
    case PAGE_STU_REQ_APPT: fresh = stuRequestAppointmentPage();  break;
    case PAGE_STU_COSCH:    fresh = stuCourseSchedulePage();      break;
    case PAGE_STU_EXSCH:    fresh = stuExamSchedulePage();        break;
    case PAGE_STU_HONOR:    fresh = stuHonorListPage();           break;
    case PAGE_STF_DASH:     fresh = staffDashboard();             break;
    case PAGE_STF_PROFILE:  fresh = stfProfilePage();             break;
    case PAGE_STF_GRADES:   fresh = stfSetGradesPage();           break;
    case PAGE_STF_COURSES:  fresh = stfCoursesPage();             break;
    case PAGE_STF_APPT:     fresh = stfAppointmentsPage();        break;
    case PAGE_STF_TOP:      fresh = stfTopStudentsPage();         break;
    case PAGE_ADM_DASH:     fresh = adminDashboard();             break;
    case PAGE_ADM_PROFILE:  fresh = admProfilePage();             break;
    case PAGE_ADM_COURSES:  fresh = admCoursesPage();             break;
    case PAGE_ADM_COSCH:    fresh = admCourseSchedulePage();      break;
    case PAGE_ADM_EXSCH:    fresh = admExamSchedulePage();        break;
    case PAGE_STU_NOTIF:    fresh = stuNotificationsPage();       break;
    case PAGE_STF_NOTIF:    fresh = stfNotificationsPage();       break;
    case PAGE_ADM_NOTIF:    fresh = admNotificationsPage();       break;
    default: return;
    }
    if (!fresh || !old) return;
    stack->insertWidget(idx, fresh);
    stack->removeWidget(old);
    old->deleteLater();
}

// ── Notification helpers ──────────────────────────────────────────────────────
int MainWindow::countUnreadNotif(long long int userId, int userType)
{
    int cnt = 0;
    for (int i = 0; i < d.numOfNotif; i++)
        if (d.notif[i].userId == userId && d.notif[i].userType == userType && !d.notif[i].isRead)
            cnt++;
    return cnt;
}

bool MainWindow::confirmDialog(const QString& title, const QString& msg,
                                      const QString& confirmText, const QString& color)
  {
      QDialog dlg(this);
      dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
      dlg.setModal(true);
      dlg.setFixedWidth(400);
      dlg.setStyleSheet(QString(
          "QDialog{ background:%1; border:1px solid %2; border-radius:18px; }"
          "QLabel{ background:transparent; }"
          "QPushButton{ border-radius:10px; font-size:13px; font-weight:700; border:none; padding:10px 24px; }")
          .arg(DS::BG_CARD()).arg(DS::BORDER()));
      auto* L = new QVBoxLayout(&dlg); L->setContentsMargins(32,32,32,28); L->setSpacing(16);

      // Icon + title row
      auto* hdr = new QHBoxLayout;
      auto* iconLbl = new QLabel("\u26A0");
      iconLbl->setStyleSheet(QString("font-size:28px; color:%1;").arg(color));
      auto* titleLbl = new QLabel(title);
      titleLbl->setStyleSheet(QString("font-size:16px; font-weight:800; color:%1;").arg(color));
      hdr->addWidget(iconLbl); hdr->addSpacing(10); hdr->addWidget(titleLbl); hdr->addStretch();
      L->addLayout(hdr);

      // Separator
      auto* sep = new QFrame; sep->setFrameShape(QFrame::HLine);
      sep->setStyleSheet(QString("QFrame{border:none;border-top:1px solid %1;background:transparent;max-height:1px;}").arg(DS::BORDER()));
      L->addWidget(sep);

      // Message
      auto* msgLbl = new QLabel(msg); msgLbl->setWordWrap(true);
      msgLbl->setStyleSheet(QString("color:%1; font-size:13px;")
          .arg(g_lightMode ? "#4A5580" : "#7B8EAA"));
      L->addWidget(msgLbl); L->addSpacing(8);

      // Buttons
      auto* btnRow = new QHBoxLayout; btnRow->setSpacing(12);
      auto* cancelBtn = new QPushButton("Cancel");
      cancelBtn->setStyleSheet(QString(
          "QPushButton{background:%1;color:%2;}"
          "QPushButton:hover{background:%3;color:%4;}")
          .arg(g_lightMode ? "#F5F1EA" : "#1A2240")
          .arg(g_lightMode ? "#4A4A6A" : "#7B8EAA")
          .arg(g_lightMode ? "#E8E2D8" : "#243050")
          .arg(g_lightMode ? "#1A1A2E" : "#E2EEFF"));
      auto* confirmBtn = new QPushButton(confirmText);
      confirmBtn->setStyleSheet(QString(
          "QPushButton{background:%1;color:white;}"
          "QPushButton:hover{background:%2;}").arg(color).arg(QColor(color).lighter(120).name()));
      btnRow->addStretch(); btnRow->addWidget(cancelBtn); btnRow->addWidget(confirmBtn);
      L->addLayout(btnRow);

      bool result = false;
      connect(cancelBtn,  &QPushButton::clicked, &dlg, [&]{ result=false; dlg.accept(); });
      connect(confirmBtn, &QPushButton::clicked, &dlg, [&]{ result=true;  dlg.accept(); });
      dlg.exec();
      return result;
  }

  void MainWindow::toggleTheme() {
      g_lightMode = !g_lightMode;
      int cur = stack->currentIndex();
      // Rebuild every page so colours update everywhere, not just current
      const QList<int> allPages = {
          PAGE_LOGIN, PAGE_SIGNUP,
          PAGE_STU_DASH, PAGE_STU_PROFILE, PAGE_STU_COURSEREG,
          PAGE_STU_GRADES, PAGE_STU_APPT, PAGE_STU_REQ_APPT,
          PAGE_STU_COSCH, PAGE_STU_EXSCH, PAGE_STU_HONOR,
          PAGE_STF_DASH, PAGE_STF_PROFILE, PAGE_STF_GRADES,
          PAGE_STF_COURSES, PAGE_STF_APPT, PAGE_STF_TOP,
          PAGE_ADM_DASH, PAGE_ADM_PROFILE, PAGE_ADM_COURSES,
          PAGE_ADM_COSCH, PAGE_ADM_EXSCH,
          PAGE_STU_NOTIF, PAGE_STF_NOTIF, PAGE_ADM_NOTIF
      };
      for (int p : allPages) refreshPage(p);
      stack->setCurrentIndex(cur);
  }

  QWidget* MainWindow::makeEmptyState(const QString& icon, const QString& title, const QString& subtitle) {
      auto* w=new QWidget; auto* L=new QVBoxLayout(w);
      L->setAlignment(Qt::AlignCenter); L->setContentsMargins(40,50,40,50); L->setSpacing(10);
      auto* iconLbl=new QLabel(icon); iconLbl->setAlignment(Qt::AlignCenter);
      iconLbl->setStyleSheet("font-size:52px;background:transparent;");
      auto* titleLbl=new QLabel(title); titleLbl->setAlignment(Qt::AlignCenter);
      titleLbl->setStyleSheet(QString("color:%1;font-size:16px;font-weight:700;background:transparent;").arg(DS::TEXT_PRIMARY()));
      L->addWidget(iconLbl); L->addSpacing(4); L->addWidget(titleLbl);
      if(!subtitle.isEmpty()){
          auto* sub=new QLabel(subtitle); sub->setAlignment(Qt::AlignCenter); sub->setWordWrap(true);
          sub->setStyleSheet(QString("color:%1;font-size:13px;background:transparent;").arg(DS::TEXT_SECOND()));
          L->addWidget(sub);
      }
      return w;
  }

  void MainWindow::closeEvent(QCloseEvent* event)
{
    saveData(d);
    QMainWindow::closeEvent(event);
}

// ══════════════════════════════════════════════════════════════════════════════
// LOGIN PAGE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::loginPage()
{
    auto* w = new QWidget;
    w->setObjectName("loginBg");
    if (g_lightMode) {
        w->setStyleSheet(
            "QWidget#loginBg{ background:qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "  stop:0 #F2EDE4, stop:0.5 #EDE8DF, stop:1 #E8E2D8); }");
    } else {
        w->setStyleSheet(
            "QWidget#loginBg{ background:qlineargradient(x1:0,y1:0,x2:1,y2:1,"
            "  stop:0 #04060D, stop:0.5 #080C18, stop:1 #030508); }");
    }
    auto* mainL = new QHBoxLayout(w);
    mainL->setContentsMargins(0,0,0,0); mainL->setSpacing(0);

    // Particles fill the ENTIRE page background
    { auto* particles = new ParticleWidget(w);
      QTimer::singleShot(50, w, [w, particles](){
          particles->setGeometry(0, 0, w->width(), w->height());
          particles->lower();
      }); }

    // Left side — branding text floating in free space (transparent, no border)
    auto* leftPanel = new QWidget;
    leftPanel->setStyleSheet("background:transparent;");
    auto* leftL = new QVBoxLayout(leftPanel);
    leftL->setContentsMargins(60,80,60,80);
    leftL->setAlignment(Qt::AlignCenter);
    auto* bigIcon = new QLabel("\U0001F393");
    bigIcon->setStyleSheet("font-size:80px; background:transparent;");
    bigIcon->setAlignment(Qt::AlignCenter);
    auto* brandTitle = new QLabel("College System");
    brandTitle->setStyleSheet(QString(
        "color:%1; font-size:42px; font-weight:800; background:transparent;").arg(DS::TEXT_PRIMARY()));
    brandTitle->setAlignment(Qt::AlignCenter);
    auto* brandSub = new QLabel("Institutional Portal");
    brandSub->setStyleSheet(QString(
        "color:%1; font-size:15px; background:transparent; letter-spacing:3px;").arg(DS::BORDER_FOCUS()));
    brandSub->setAlignment(Qt::AlignCenter);
    auto* tagline = new QLabel(
        "Empowering students, staff,\n"
        "and administrators with a\n"
        "unified academic platform.");
    tagline->setStyleSheet(QString(
        "color:%1; font-size:14px; background:transparent; line-height:1.6;").arg(DS::TEXT_SECOND()));
    tagline->setAlignment(Qt::AlignCenter);
    tagline->setWordWrap(true);
    leftL->addStretch();
    leftL->addWidget(bigIcon); leftL->addSpacing(18);
    leftL->addWidget(brandTitle); leftL->addSpacing(8);
    leftL->addWidget(brandSub); leftL->addSpacing(24);
    leftL->addWidget(tagline);
    leftL->addStretch();

    // Right side — login card, no background fill, no border
    auto* rightPanel = new QWidget;
    rightPanel->setStyleSheet("background:transparent;");
    auto* rightL = new QVBoxLayout(rightPanel); rightL->setAlignment(Qt::AlignCenter);
    auto* card = new QWidget; card->setFixedWidth(420);
    card->setObjectName("loginCard");
    card->setStyleSheet(QString(
        "QWidget#loginCard{ background:%1; border-radius:24px; border:1px solid %2; }"
        "QLabel{ border:none; background:transparent; }")
        .arg(DS::BG_CARD()).arg(DS::BORDER()));
    addShadow(card, 48, 16, 50);
    auto* cardL = new QVBoxLayout(card);
    cardL->setContentsMargins(40,40,40,40); cardL->setSpacing(0);

    auto* welcomeTitle = new QLabel("Welcome");
    welcomeTitle->setStyleSheet(QString("color:%1; font-size:26px; font-weight:800; background:transparent;").arg(DS::TEXT_PRIMARY()));
    auto* welcomeSub = new QLabel("Sign in to continue to your portal");
    welcomeSub->setStyleSheet(QString("color:%1; font-size:14px; background:transparent;").arg(DS::TEXT_SECOND()));

    auto* roleLbl = new QLabel("Account Type");
    roleLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND()));
    auto* roleCombo = new QComboBox;
    roleCombo->addItems({"\U0001F393  Student", "\U0001F468\u200D\U0001F3EB  Academic Staff", "\U0001F3DB  Administration"});
    roleCombo->setFixedHeight(46);
    roleCombo->setStyleSheet(QString(
        "QComboBox{ border:2px solid %1; border-radius:10px; padding:0 14px; font-size:14px; color:%2; background:%3; }"
        "QComboBox:hover{ border-color:%4; }"
        "QComboBox:focus{ border-color:%4; }"
        "QComboBox::drop-down{ border:none; padding-right:14px; }"
        "QComboBox::down-arrow{ width:10px; height:10px; }"
        "QComboBox QAbstractItemView{ border:1px solid %1; border-radius:10px; outline:none;"
        "  background:%3; color:%2; selection-background-color:%5; selection-color:%2; padding:4px; }"
        "QComboBox QAbstractItemView::item{ padding:6px 10px; min-height:28px; background:%3; color:%2; }"
        "QComboBox QAbstractItemView::item:hover{ background:%5; }"
        "QComboBox QAbstractItemView::item:selected{ background:%5; }")
        .arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT()).arg(DS::BORDER_FOCUS()).arg(DS::SEL_BG()));
    applyComboDelegate(roleCombo);

    auto* idLbl   = new QLabel("ID Number");   idLbl->setStyleSheet(roleLbl->styleSheet());
    auto* idEdit  = makeField("Enter your ID...");
    auto* passLbl = new QLabel("Password");    passLbl->setStyleSheet(roleLbl->styleSheet());
    auto* passEdit= makeField("Enter your password...", true);

    auto* forgotBtn = new QPushButton("Forgot password?");
    forgotBtn->setFlat(true); forgotBtn->setCursor(Qt::PointingHandCursor);
    forgotBtn->setStyleSheet(
        "QPushButton{ background:transparent; color:#00CFFF; border:none; font-size:12px; font-weight:600; }"
        "QPushButton:hover{ color:#00A8CC; text-decoration:underline; }");
    auto* loginBtn = makeBtn("Sign In  \u2192", DS::PRIMARY);
    loginBtn->setFixedHeight(52);

    auto* sepL = new QHBoxLayout;
    auto* sep1 = new QFrame; sep1->setFrameShape(QFrame::HLine); sep1->setFrameShadow(QFrame::Plain);
    sep1->setStyleSheet(QString("QFrame{border:none; border-top:1px solid %1; background:transparent; max-height:1px;}").arg(DS::BORDER()));
    auto* sepLbl = new QLabel("or");
    sepLbl->setStyleSheet(QString("color:%1; font-size:12px; background:transparent;").arg(DS::TEXT_MUTED()));
    sepLbl->setAlignment(Qt::AlignCenter);
    auto* sep2 = new QFrame; sep2->setFrameShape(QFrame::HLine); sep2->setFrameShadow(QFrame::Plain);
    sep2->setStyleSheet(QString("QFrame{border:none; border-top:1px solid %1; background:transparent; max-height:1px;}").arg(DS::BORDER()));
    sepL->addWidget(sep1,1); sepL->addWidget(sepLbl); sepL->addWidget(sep2,1);

    auto* signupRow = new QHBoxLayout; signupRow->setAlignment(Qt::AlignCenter);
    auto* signupLbl = new QLabel("Don't have an account?");
    signupLbl->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));
    auto* signupBtn = new QPushButton("Create Account");
    signupBtn->setFlat(true); signupBtn->setCursor(Qt::PointingHandCursor);
    signupBtn->setStyleSheet(
        "QPushButton{ background:transparent; color:#00CFFF; border:none; font-size:13px; font-weight:700; }"
        "QPushButton:hover{ color:#00A8CC; text-decoration:underline; }");
    signupRow->addWidget(signupLbl); signupRow->addWidget(signupBtn);

    cardL->addWidget(welcomeTitle); cardL->addSpacing(4); cardL->addWidget(welcomeSub);
    cardL->addSpacing(28);
    cardL->addWidget(roleLbl); cardL->addSpacing(6); cardL->addWidget(roleCombo);
    cardL->addSpacing(16);
    cardL->addWidget(idLbl); cardL->addSpacing(6); cardL->addWidget(idEdit);
    cardL->addSpacing(16);
    cardL->addWidget(passLbl); cardL->addSpacing(6); cardL->addWidget(passEdit);
    cardL->addSpacing(8); cardL->addWidget(forgotBtn, 0, Qt::AlignRight);
    cardL->addSpacing(20); cardL->addWidget(loginBtn);
    cardL->addSpacing(16); cardL->addLayout(sepL);
    cardL->addSpacing(12); cardL->addLayout(signupRow);

    rightL->addWidget(card, 0, Qt::AlignCenter);
    mainL->addWidget(leftPanel, 1); mainL->addWidget(rightPanel, 1);

    // Enter key triggers login from any field
    auto doLogin = [=]() { loginBtn->click(); };
    connect(idEdit,   &QLineEdit::returnPressed, loginBtn, &QPushButton::click);
    connect(passEdit, &QLineEdit::returnPressed, loginBtn, &QPushButton::click);

    connect(forgotBtn, &QPushButton::clicked, this, [this]() {
        ForgotPasswordDialog dlg(d, this); dlg.exec();
    });

    connect(loginBtn, &QPushButton::clicked, this, [=]() {
        bool ok = false;
        long long id = idEdit->text().trimmed().toLongLong(&ok);
        QString pass = passEdit->text();
        int role = roleCombo->currentIndex();
        if (!ok || idEdit->text().trimmed().isEmpty()) {
            showMsg("Please enter a valid ID number.", false);
            idEdit->clear(); passEdit->clear(); return;
        }
        bool found = false, wrongPass = false;
        if (role == 0) {
            for (int i = 0; i < d.numOfStu; i++) {
                if (d.stu[i].inf.id == id) {
                    found = true;
                    if (d.stu[i].inf.password == pass) {
                        d.loggedInIndex = i; d.loggedInType = 0;
                        refreshPage(PAGE_STU_DASH);
                        stack->setCurrentIndex(PAGE_STU_DASH);
                    } else { wrongPass = true; }
                    break;
                }
            }
        } else if (role == 1) {
            for (int i = 0; i < d.numOfStf; i++) {
                if (d.stf[i].inf.id == id) {
                    found = true;
                    if (d.stf[i].inf.password == pass) {
                        d.loggedInIndex = i; d.loggedInType = 1;
                        refreshPage(PAGE_STF_DASH);
                        stack->setCurrentIndex(PAGE_STF_DASH);
                    } else { wrongPass = true; }
                    break;
                }
            }
        } else {
            for (int i = 0; i < d.numOfAdm; i++) {
                if (d.adm[i].inf.id == id) {
                    found = true;
                    if (d.adm[i].inf.password == pass) {
                        d.loggedInIndex = i; d.loggedInType = 2;
                        refreshPage(PAGE_ADM_DASH);
                        stack->setCurrentIndex(PAGE_ADM_DASH);
                    } else { wrongPass = true; }
                    break;
                }
            }
        }
        if (wrongPass) showMsg("Incorrect password!", false);
        else if (!found) showMsg("ID not found. Please check your role.", false);
        idEdit->clear(); passEdit->clear();
    });

    connect(signupBtn, &QPushButton::clicked, this, [this]() {
        refreshPage(PAGE_SIGNUP); stack->setCurrentIndex(PAGE_SIGNUP);
    });

    return w;
}

// ══════════════════════════════════════════════════════════════════════════════
// SIGN-UP PAGE
// ══════════════════════════════════════════════════════════════════════════════
QWidget* MainWindow::signUpPage()
{
    auto* w = new QWidget;
    w->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* scroll = new QScrollArea;
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("background:transparent; border:none;");
    auto* inner = new QWidget;
    inner->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* outerL = new QVBoxLayout(inner);
    outerL->setAlignment(Qt::AlignHCenter);
    outerL->setContentsMargins(40,40,40,40);

    auto* card = new QWidget; card->setFixedWidth(520);
    card->setObjectName("signupCard");
    card->setStyleSheet(QString(
        "QWidget#signupCard{ background:%1; border-radius:20px; border:1px solid %2; }"
        "QLabel{ border:none; background:transparent; }")
        .arg(DS::BG_CARD()).arg(DS::BORDER()));
    addShadow(card, 40, 12, g_lightMode ? 15 : 40);
    auto* cardL = new QVBoxLayout(card);
    cardL->setContentsMargins(0,0,0,0); cardL->setSpacing(0);

    auto* hdr = new QWidget; hdr->setFixedHeight(68);
    if (g_lightMode) {
        hdr->setStyleSheet(
            "background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #F5F1EA, stop:1 #EDE8DF);"
            "border-radius:20px; border-bottom:1px solid #E0D8CC;");
    } else {
        hdr->setStyleSheet(
            "background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #001828, stop:1 #12002A);"
            "border-radius:20px;");
    }
    auto* hdrL = new QHBoxLayout(hdr); hdrL->setContentsMargins(28,0,28,0);
    auto* hdrTitle = new QLabel("\U0001F393  Create New Account");
    hdrTitle->setStyleSheet(QString("color:%1; font-size:18px; font-weight:700; background:transparent;").arg(DS::TEXT_PRIMARY()));
    auto* backToLogin = new QPushButton("\u2190 Login");
    backToLogin->setFlat(true); backToLogin->setCursor(Qt::PointingHandCursor);
    backToLogin->setStyleSheet(
        "QPushButton{ background:rgba(0,207,255,0.10); color:#00CFFF; border:none;"
        "  border-radius:8px; padding:6px 16px; font-size:13px; font-weight:600; }"
        "QPushButton:hover{ background:rgba(0,207,255,0.25); }");
    hdrL->addWidget(hdrTitle); hdrL->addStretch(); hdrL->addWidget(backToLogin);
    cardL->addWidget(hdr);

    auto* body = new QWidget;
    body->setStyleSheet(QString("background:%1; border-radius:0;").arg(DS::BG_CARD()));
    auto* bodyL = new QVBoxLayout(body);
    bodyL->setContentsMargins(36,28,36,36); bodyL->setSpacing(10);

    const QString fs = inputLineStyle(DS::PRIMARY);
    const QString ls = labelSS();
    auto mkLbl  = [&](const QString& t) { auto* l = new QLabel(t); l->setStyleSheet(ls); return l; };
    auto mkEdit = [&](const QString& ph = "", bool pw = false) {
        auto* e = new QLineEdit;
        if (pw) e->setEchoMode(QLineEdit::Password);
        e->setPlaceholderText(ph); e->setStyleSheet(fs); return e;
    };

    auto* roleCombo2 = new QComboBox;
    roleCombo2->addItems({"Student", "Academic Staff Member", "Administration"});
    roleCombo2->setStyleSheet(QString(
        "QComboBox{ border:2px solid %1; border-radius:10px; padding:10px 14px; font-size:14px; color:%2; background:%3; }"
        "QComboBox:hover{ border-color:%4; }"
        "QComboBox:focus{ border-color:%4; }"
        "QComboBox::drop-down{ border:none; padding-right:14px; }"
        "QComboBox::down-arrow{ width:10px; height:10px; }"
        "QComboBox QAbstractItemView{ border:1px solid %1; border-radius:8px; outline:none;"
        "  background:%3; color:%2; selection-background-color:%5; selection-color:%2; padding:4px; }"
        "QComboBox QAbstractItemView::item{ padding:6px 10px; min-height:28px; background:%3; color:%2; }"
        "QComboBox QAbstractItemView::item:hover{ background:%5; }"
        "QComboBox QAbstractItemView::item:selected{ background:%5; }")
        .arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT()).arg(DS::BORDER_FOCUS()).arg(DS::SEL_BG()));
    applyComboDelegate(roleCombo2);

    auto* nameEdit  = mkEdit("Full name...");
    auto* phoneEdit = mkEdit("11-digit mobile number...");
    auto* passEdit2 = mkEdit("Password...", true);
    auto* confEdit  = mkEdit("Confirm password...", true);
    auto* addrEdit  = mkEdit("Your address...");
    auto* depEdit   = mkEdit("e.g. SWE, AI, CSEC...");
    auto* posEdit   = mkEdit("e.g. Professor, Lecturer...");
    auto* yearLbl   = mkLbl("Academic Year (1-4):"); auto* depLbl = mkLbl("Department:"); auto* posLbl = mkLbl("Position:");
    auto* yearSpin  = new QSpinBox; yearSpin->setRange(1,4); yearSpin->setStyleSheet(fs);

    auto* idGenCard = new QWidget;
    idGenCard->setStyleSheet(QString("background:%1; border-radius:10px; border:1px solid %2;")
        .arg(g_lightMode ? "#EDFFF6" : "#001A10").arg(g_lightMode ? "#00A860" : "#003820"));
    idGenCard->hide();
    auto* igL = new QHBoxLayout(idGenCard); igL->setContentsMargins(14,12,14,12);
    auto* idGenLbl = new QLabel; idGenLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;")
        .arg(g_lightMode ? "#007A45" : "#00FF88"));
    igL->addWidget(idGenLbl, 1);

    bodyL->addWidget(mkLbl("Register as:")); bodyL->addWidget(roleCombo2);
    bodyL->addWidget(mkLbl("Full Name:"));        bodyL->addWidget(nameEdit);
    bodyL->addWidget(mkLbl("Mobile (11 digits):")); bodyL->addWidget(phoneEdit);
    bodyL->addWidget(mkLbl("Password:"));         bodyL->addWidget(passEdit2);
    // Password strength bar
    auto* strengthBar = new QProgressBar;
    strengthBar->setRange(0,4); strengthBar->setValue(0); strengthBar->setFixedHeight(6);
    strengthBar->setTextVisible(false);
    strengthBar->setStyleSheet(
        "QProgressBar{ background:#E0D8CC; border-radius:3px; border:none; }"
        "QProgressBar::chunk{ border-radius:3px; background:#E8003A; }");
    auto* strengthLbl = new QLabel("Strength: Too short");
    strengthLbl->setStyleSheet(QString("color:%1; font-size:11px; background:transparent;").arg(DS::TEXT_MUTED()));
    QObject::connect(passEdit2, &QLineEdit::textChanged, passEdit2, [=](const QString& p) {
        int score = 0;
        if (p.length() >= 6)  score++;
        if (p.length() >= 10) score++;
        if (QRegularExpression("[A-Z]").match(p).hasMatch()) score++;
        if (QRegularExpression("[0-9!@#$%]").match(p).hasMatch()) score++;
        strengthBar->setValue(score);
        static const QString colors[]  = {"#E8003A","#FF6B00","#FFB400","#00CC70","#00FF88"};
        static const QString labels[] = {
            "Too short","Weak","Fair","Good","Strong"};
        strengthBar->setStyleSheet(QString(
            "QProgressBar{ background:#E0D8CC; border-radius:3px; border:none; }"
            "QProgressBar::chunk{ border-radius:3px; background:%1; }").arg(colors[score]));
        strengthLbl->setText(QString("Strength: %1").arg(labels[score]));
    });
    bodyL->addWidget(strengthBar); bodyL->addWidget(strengthLbl);
    bodyL->addWidget(mkLbl("Confirm Password:")); bodyL->addWidget(confEdit);
    bodyL->addWidget(mkLbl("Address:"));          bodyL->addWidget(addrEdit);
    bodyL->addWidget(depLbl); bodyL->addWidget(depEdit);
    bodyL->addWidget(yearLbl); bodyL->addWidget(yearSpin);
    bodyL->addWidget(posLbl); bodyL->addWidget(posEdit);
    bodyL->addWidget(idGenCard);

    auto updateFields = [=](int role) {
        bool isStu = (role == 0), isStf = (role == 1), isAdm = (role == 2);
        depLbl->setVisible(isStu || isStf); depEdit->setVisible(isStu || isStf);
        yearLbl->setVisible(isStu); yearSpin->setVisible(isStu);
        posLbl->setVisible(isStf || isAdm); posEdit->setVisible(isStf || isAdm);
    };
    updateFields(0);
    connect(roleCombo2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int r){ updateFields(r); });

    auto* regBtn = makeBtn("Create Account  \u2192", DS::PRIMARY);
    regBtn->setFixedHeight(50);
    bodyL->addSpacing(8); bodyL->addWidget(regBtn);
    cardL->addWidget(body);
    outerL->addWidget(card, 0, Qt::AlignHCenter);
    scroll->setWidget(inner);
    auto* mainL = new QVBoxLayout(w);
    mainL->setContentsMargins(0,0,0,0); mainL->addWidget(scroll);

    connect(backToLogin, &QPushButton::clicked, this, [this]{ stack->setCurrentIndex(PAGE_LOGIN); });

    connect(regBtn, &QPushButton::clicked, this, [=]() {
        int role = roleCombo2->currentIndex();
        QString name = nameEdit->text().trimmed(), phone = phoneEdit->text().trimmed();
        QString pass = passEdit2->text(), conf = confEdit->text(), addr = addrEdit->text().trimmed();
        if (name.isEmpty() || phone.isEmpty() || pass.isEmpty() || addr.isEmpty()) {
            showMsg("Please fill all required fields!", false); return;
        }
        if (phone.length() != 11) { showMsg("Mobile number must be exactly 11 digits!", false); return; }
        if (isMobileTaken(phone, d)) { showMsg("This mobile number is already registered!", false); return; }
        if (pass != conf) { showMsg("Passwords do not match!", false); return; }
        if (pass.length() < 3) { showMsg("Password must be at least 3 characters!", false); return; }
        long long newId = 0;
        if (role == 0) {
            if (d.numOfStu >= SIZE) { showMsg("Student limit reached!", false); return; }
            newId = stuIDCounter++;
            d.stu[d.numOfStu].inf = { newId, name, phone, pass, addr };
            d.stu[d.numOfStu].department = depEdit->text().trimmed();
            d.stu[d.numOfStu].academicYear = yearSpin->value();
            d.numOfStu++;
        } else if (role == 1) {
            if (d.numOfStf >= SIZE) { showMsg("Staff limit reached!", false); return; }
            newId = stfIDCounter++;
            d.stf[d.numOfStf].inf = { newId, name, phone, pass, addr };
            d.stf[d.numOfStf].department = depEdit->text().trimmed();
            d.stf[d.numOfStf].position = posEdit->text().trimmed();
            d.numOfStf++;
        } else {
            if (d.numOfAdm >= SIZE) { showMsg("Admin limit reached!", false); return; }
            newId = admIDCounter++;
            d.adm[d.numOfAdm].inf = { newId, name, phone, pass, addr };
            d.adm[d.numOfAdm].position = posEdit->text().trimmed();
            d.numOfAdm++;
        }
        idGenLbl->setText(QString("Account created! Your ID is: %1").arg(newId));
        idGenCard->show();
        nameEdit->clear(); phoneEdit->clear(); passEdit2->clear();
        confEdit->clear(); addrEdit->clear(); depEdit->clear(); posEdit->clear();
        showMsg(QString("Account created! Your ID: %1").arg(newId));
    });

    return w;
}

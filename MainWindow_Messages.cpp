#include "MainWindow.h"
#include "StyleHelpers.h"
#include <QScrollArea>
#include <memory>

// ═══════════════════════════════════════════════════════════════════════════
// Notifications Page builder
// ═══════════════════════════════════════════════════════════════════════════
static QWidget* buildNotifPage(
    MainWindow* mw,
    AppData& d,
    long long int myId, int myType,
    QFrame* sidebar,
    const QString& accent,
    int /*refreshPageIdx*/,
    QStackedWidget* /*stack2*/)
{
    auto* content = new QWidget;
    content->setStyleSheet(QString("background:%1;").arg(DS::BG_PAGE()));
    auto* mainL = new QVBoxLayout(content); mainL->setContentsMargins(32,28,32,28); mainL->setSpacing(20);

    auto* hdrRow = new QHBoxLayout;
    auto* titleLbl = new QLabel("\U0001F514  Notifications");
    titleLbl->setStyleSheet(QString("font-size:22px; font-weight:800; color:%1; background:transparent;").arg(DS::TEXT_PRIMARY()));
    int unread = 0;
    for (int j = 0; j < d.numOfNotif; j++)
        if (d.notif[j].userId==myId && d.notif[j].userType==myType && !d.notif[j].isRead) unread++;
    auto* unreadBadge = new QLabel(unread>0 ? QString("%1 unread").arg(unread) : "All read");
    unreadBadge->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(unread>0?accent:DS::TEXT_MUTED()));
    auto* markAllBtn = new QPushButton("\u2713  Mark All as Read");
    markAllBtn->setFixedHeight(38); markAllBtn->setCursor(Qt::PointingHandCursor);
    markAllBtn->setStyleSheet(QString(
        "QPushButton{ background:rgba(%1,%2,%3,20); color:%4; border:1px solid %4; border-radius:10px; padding:0 16px; font-size:12px; font-weight:600; }"
        "QPushButton:hover{ background:rgba(%1,%2,%3,40); }")
        .arg(QColor(accent).red()).arg(QColor(accent).green()).arg(QColor(accent).blue()).arg(accent));
    auto* clearAllBtn = new QPushButton("\U0001F5D1  Clear All");
    clearAllBtn->setFixedHeight(38); clearAllBtn->setCursor(Qt::PointingHandCursor);
    clearAllBtn->setStyleSheet(
        "QPushButton{ background:rgba(255,40,80,15); color:#E8003A; border:1px solid #E8003A; border-radius:10px; padding:0 16px; font-size:12px; font-weight:600; }"
        "QPushButton:hover{ background:rgba(255,40,80,25); }");
    hdrRow->addWidget(titleLbl); hdrRow->addSpacing(12); hdrRow->addWidget(unreadBadge); hdrRow->addStretch();
    hdrRow->addWidget(markAllBtn); hdrRow->addSpacing(8); hdrRow->addWidget(clearAllBtn);
    mainL->addLayout(hdrRow);

    auto* listW = new QWidget;
    listW->setStyleSheet(QString("background:%1; border-radius:14px; border:1px solid %2;").arg(DS::BG_CARD()).arg(DS::BORDER()));
    auto* listL = new QVBoxLayout(listW); listL->setContentsMargins(0,4,0,4); listL->setSpacing(0);

    int idxArr[NOTIF_SIZE]; int cnt=0;
    for (int j = 0; j < d.numOfNotif; j++)
        if (d.notif[j].userId==myId && d.notif[j].userType==myType) idxArr[cnt++]=j;
    for (int a=0;a<cnt-1;a++) for (int b=0;b<cnt-a-1;b++)
        if (d.notif[idxArr[b]].timestamp<d.notif[idxArr[b+1]].timestamp) { int t=idxArr[b]; idxArr[b]=idxArr[b+1]; idxArr[b+1]=t; }

    if (cnt == 0) {
        auto* emptyLbl = new QLabel("\U0001F4ED  No notifications yet");
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet(QString("color:%1; font-size:15px; font-style:italic; background:transparent; padding:40px;").arg(DS::TEXT_MUTED()));
        listL->addWidget(emptyLbl);
    }

    // Find sidebar badge label (created by makeSidebar when unread > 0)
    QLabel* sideBadge = nullptr;
    {
        QList<QLabel*> labels = sidebar->findChildren<QLabel*>("notifBadge");
        if (!labels.isEmpty()) sideBadge = labels.first();
    }

    QVector<QWidget*> notifItems;
    QVector<QLabel*>  notifDots;
    QVector<QLabel*>  notifTextLbls;
    int liveUnread = unread;

    auto sharedCount = std::make_shared<int>(liveUnread);

    for (int k = 0; k < cnt; k++) {
        notification& n = d.notif[idxArr[k]];
        auto* item = new QWidget;
        item->setObjectName("notifItem");
        QColor ac(n.color.isEmpty() ? accent : n.color);
        QString itemBg = n.isRead
            ? DS::BG_CARD()
            : QString("rgba(%1,%2,%3,%4)").arg(ac.red()).arg(ac.green()).arg(ac.blue()).arg(g_lightMode ? 12 : 8);
        QString borderDiv = g_lightMode ? DS::BORDER() : "#141A26";
        item->setStyleSheet(QString(
            "QWidget#notifItem{ background:%1; border-bottom:1px solid %2; border-left:4px solid %3; }"
            "QLabel{ border:none; background:transparent; }")
            .arg(itemBg).arg(borderDiv).arg(n.isRead ? DS::BORDER() : n.color));
        item->setMinimumHeight(64);
        auto* itemL = new QHBoxLayout(item); itemL->setContentsMargins(20,12,20,12);
        auto* iconLbl = new QLabel(n.icon.isEmpty() ? "\U0001F514" : n.icon);
        iconLbl->setStyleSheet("font-size:22px; background:transparent; border:none;");
        iconLbl->setFixedWidth(36);
        auto* textL = new QVBoxLayout; textL->setSpacing(2);
        auto* textLbl = new QLabel(n.text);
        textLbl->setWordWrap(true);
        textLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:%2; background:transparent; border:none;")
            .arg(n.isRead ? DS::TEXT_MUTED() : DS::TEXT_PRIMARY()).arg(n.isRead ? 400 : 600));
        auto* timeLbl = new QLabel(QString("Order #%1").arg(n.timestamp));
        timeLbl->setStyleSheet(QString("color:%1; font-size:11px; background:transparent; border:none;").arg(DS::TEXT_MUTED()));
        textL->addWidget(textLbl); textL->addWidget(timeLbl);
        QLabel* dot = nullptr;
        if (!n.isRead) {
            dot = new QLabel("\u25CF");
            dot->setStyleSheet(QString("color:%1; font-size:10px; background:transparent; border:none;").arg(n.color.isEmpty()?accent:n.color));
            dot->setFixedWidth(16);
            itemL->addWidget(dot);
        } else {
            itemL->addSpacing(16);
        }
        itemL->addWidget(iconLbl); itemL->addSpacing(8); itemL->addLayout(textL,1);

        // Per-item mark as read button (only if unread)
        if (!n.isRead) {
            int notifIdx = idxArr[k];
            auto* markBtn = new QPushButton("\u2713");
            markBtn->setFixedSize(28,28);
            markBtn->setCursor(Qt::PointingHandCursor);
            markBtn->setToolTip("Mark as read");
            QString btnColor = n.color.isEmpty() ? accent : n.color;
            markBtn->setStyleSheet(QString(
                "QPushButton{ background:transparent; color:%1; border:1px solid %1;"
                " border-radius:14px; font-size:12px; font-weight:700; }"
                "QPushButton:hover{ background:%1; color:%2; }").arg(btnColor).arg(g_lightMode ? "#FFFFFF" : "#0A0C14"));

            QWidget* itemPtr   = item;
            QLabel*  dotPtr    = dot;
            QLabel*  textPtr   = textLbl;
            QLabel*  badgePtr  = sideBadge;
            QLabel*  topBadge  = unreadBadge;

            QObject::connect(markBtn, &QPushButton::clicked, mw,
                [=, &d, sharedCount]() {
                    d.notif[notifIdx].isRead = true;
                    itemPtr->setStyleSheet(QString(
                        "QWidget#notifItem{ background:%1; border-bottom:1px solid %2; border-left:4px solid %3; }"
                        "QLabel{ border:none; background:transparent; }")
                        .arg(DS::BG_CARD()).arg(g_lightMode ? DS::BORDER() : "#141A26").arg(DS::BORDER()));
                    if (dotPtr)  dotPtr->hide();
                    if (textPtr) textPtr->setStyleSheet(
                        QString("color:%1; font-size:13px; font-weight:400; background:transparent; border:none;").arg(DS::TEXT_MUTED()));
                    markBtn->hide();
                    (*sharedCount)--;
                    int left = *sharedCount;
                    topBadge->setText(left > 0 ? QString("%1 unread").arg(left) : "All read");
                    topBadge->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;")
                        .arg(left > 0 ? accent : DS::TEXT_MUTED()));
                    if (badgePtr) {
                        if (left <= 0) badgePtr->hide();
                        else           badgePtr->setText(QString::number(left));
                    }
                });
            itemL->addWidget(markBtn);
        }

        notifItems.append(item);
        notifDots.append(dot);
        notifTextLbls.append(textLbl);
        listL->addWidget(item);
    }
    listL->addStretch();

    auto* scrollNotif = new QScrollArea; scrollNotif->setWidget(listW); scrollNotif->setWidgetResizable(true);
    scrollNotif->setFrameShape(QFrame::NoFrame); scrollNotif->setStyleSheet(QString("background:%1; border:none;").arg(DS::BG_PAGE()));
    mainL->addWidget(scrollNotif,1);

    QObject::connect(markAllBtn, &QPushButton::clicked, mw, [=, &d]() {
        for (int j=0;j<d.numOfNotif;j++)
            if (d.notif[j].userId==myId && d.notif[j].userType==myType) d.notif[j].isRead=true;
        for (int k = 0; k < notifItems.size(); k++) {
            notifItems[k]->setStyleSheet(QString(
                "QWidget#notifItem{ background:%1; border-bottom:1px solid %2; border-left:4px solid %3; }"
                "QLabel{ border:none; background:transparent; }")
                .arg(DS::BG_CARD()).arg(g_lightMode ? DS::BORDER() : "#141A26").arg(DS::BORDER()));
            if (notifDots[k])     notifDots[k]->hide();
            if (notifTextLbls[k]) notifTextLbls[k]->setStyleSheet(
                QString("color:%1; font-size:13px; font-weight:400; background:transparent; border:none;").arg(DS::TEXT_MUTED()));
            QList<QPushButton*> btns = notifItems[k]->findChildren<QPushButton*>();
            for (auto* b : btns) b->hide();
        }
        unreadBadge->setText("All read");
        unreadBadge->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_MUTED()));
        if (sideBadge) sideBadge->hide();
    });

    QObject::connect(clearAllBtn, &QPushButton::clicked, mw, [=, &d]() {
        int write = 0;
        for (int j = 0; j < d.numOfNotif; j++) {
            if (d.notif[j].userId == myId && d.notif[j].userType == myType) continue;
            d.notif[write++] = d.notif[j];
        }
        d.numOfNotif = write;
        QLayoutItem* child;
        while ((child = listL->takeAt(0)) != nullptr) {
            if (child->widget()) child->widget()->deleteLater();
            delete child;
        }
        auto* emptyLbl = new QLabel("\U0001F4ED  No notifications yet");
        emptyLbl->setAlignment(Qt::AlignCenter);
        emptyLbl->setStyleSheet(QString("color:%1; font-size:15px; font-style:italic; background:transparent; padding:40px;").arg(DS::TEXT_MUTED()));
        listL->addWidget(emptyLbl);
        listL->addStretch();
        unreadBadge->setText("All read");
        unreadBadge->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_MUTED()));
        if (sideBadge) sideBadge->hide();
    });

    auto* pageScroll = new QScrollArea; pageScroll->setWidget(content); pageScroll->setWidgetResizable(true);
    pageScroll->setFrameShape(QFrame::NoFrame); pageScroll->setStyleSheet(QString("background:%1; border:none;").arg(DS::BG_PAGE()));
    return mw->wrapWithSidebar(sidebar, pageScroll);
}

// ═══════════════════════════════════════════════════════════════════════════
// Sidebar item helpers (used by notification pages)
// ═══════════════════════════════════════════════════════════════════════════
static QStringList stuSbItems2() {
    return { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DA  Course Registration",
             "  \U0001F4CA  Grades", "  \U0001F4C5  Request Appointment", "  \U0001F5D3  My Appointments",
             "  \U0001F550  Course Schedule", "  \U0001F4CB  Exam Schedule", "  \U0001F3C6  Honor List",
             "  \U0001F514  Notifications" };
}
static QList<int> stuSbPages2() { return {2,3,4,5,7,6,8,9,10,22}; }

static QStringList stfSbItems2() {
    return { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DD  Set Grades",
             "  \U0001F4DA  My Courses", "  \U0001F5D3  Appointments", "  \U0001F3C6  Top Students",
             "  \U0001F514  Notifications" };
}
static QList<int> stfSbPages2() { return {11,12,13,14,15,16,23}; }

static QStringList admSbItems2() {
    return { "  \U0001F3E0  Dashboard", "  \U0001F464  Profile", "  \U0001F4DA  Manage Courses",
             "  \U0001F550  Course Schedule", "  \U0001F4CB  Exam Schedule",
             "  \U0001F514  Notifications" };
}
static QList<int> admSbPages2() { return {17,18,19,20,21,24}; }

// ── Student ──────────────────────────────────────────────────────────────────
QWidget* MainWindow::stuNotificationsPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stu[i].inf.id, 0);
    QStringList items = stuSbItems2();
    if (unreadN > 0) items[9] = QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stuSbPages2(), PAGE_STU_NOTIF, DS::STU_ACCENT);
    return buildNotifPage(this, d, d.stu[i].inf.id, 0, sidebar, DS::STU_ACCENT, PAGE_STU_NOTIF, stack);
}

QWidget* MainWindow::stfNotificationsPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.stf[i].inf.id, 1);
    QStringList items = stfSbItems2();
    if (unreadN > 0) items[6] = QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, stfSbPages2(), PAGE_STF_NOTIF, DS::STF_ACCENT);
    return buildNotifPage(this, d, d.stf[i].inf.id, 1, sidebar, DS::STF_ACCENT, PAGE_STF_NOTIF, stack);
}

QWidget* MainWindow::admNotificationsPage()
{
    int i = d.loggedInIndex; if (i < 0) i = 0;
    int unreadN = countUnreadNotif(d.adm[i].inf.id, 2);
    QStringList items = admSbItems2();
    if (unreadN > 0) items[5] = QString("  \U0001F514  Notifications (%1)").arg(unreadN);
    auto* sidebar = makeSidebar(items, admSbPages2(), PAGE_ADM_NOTIF, DS::ADM_ACCENT);
    return buildNotifPage(this, d, d.adm[i].inf.id, 2, sidebar, DS::ADM_ACCENT, PAGE_ADM_NOTIF, stack);
}

#pragma once
  #include "ThemeState.h"
  #include <QMainWindow>
  #include <QStackedWidget>
  #include <QLabel>
  #include <QPushButton>
  #include <QLineEdit>
  #include <QComboBox>
  #include <QTableWidget>
  #include <QVBoxLayout>
  #include <QHBoxLayout>
  #include <QFrame>
  #include <QListWidget>
  #include <QTextEdit>
  #include <QScrollArea>
  #include <QWidget>
  #include <QCloseEvent>
  #include <QMessageBox>
  #include <QFormLayout>
  #include <QGroupBox>
  #include <QSpinBox>
  #include <QDoubleSpinBox>
  #include <QGraphicsDropShadowEffect>
  #include <QGraphicsOpacityEffect>
  #include <QPropertyAnimation>
  #include <QParallelAnimationGroup>
  #include <QTimer>
  #include <QDialog>
  #include <QPainter>
  #include <QEvent>
  #include <QStyledItemDelegate>
  #include <QApplication>
  #include <QScreen>
  #include <QProgressBar>
  #include "DataStructures.h"
  #include "Logic.h"

  // ── Custom item delegate — bypasses OS native rendering for combo popups ──
  class ComboDelegate : public QStyledItemDelegate {
  public:
      explicit ComboDelegate(QObject* parent = nullptr) : QStyledItemDelegate(parent) {}

      void paint(QPainter* p, const QStyleOptionViewItem& opt,
                 const QModelIndex& idx) const override
      {
          p->save();
          p->setRenderHint(QPainter::Antialiasing);

          bool hover    = opt.state & QStyle::State_MouseOver;
          bool selected = opt.state & QStyle::State_Selected;

          // Background
          QColor bg, text;
          if (g_lightMode) {
              bg   = (hover || selected) ? QColor("#E8E4DC") : QColor("#FFFFFF");
              text = QColor("#1A1A2E");
          } else {
              bg   = (hover || selected) ? QColor("#0E2040") : QColor("#0F1325");
              text = QColor("#E2EEFF");
          }

          p->fillRect(opt.rect, bg);

          // Text
          p->setPen(text);
          QFont f = opt.font;
          f.setPointSize(10);
          p->setFont(f);
          p->drawText(opt.rect.adjusted(14, 0, -8, 0),
                      Qt::AlignVCenter, idx.data(Qt::DisplayRole).toString());
          p->restore();
      }

      QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override {
          return QSize(0, 34);
      }
  };

  // ── Apply ComboDelegate + force background on the popup container ──────────
  inline void applyComboDelegate(QComboBox* cb)
  {
      cb->setItemDelegate(new ComboDelegate(cb));

      // Also paint the container widget that Qt wraps around the view
      QAbstractItemView* v = cb->view();
      if (!v) return;
      v->setFrameShape(QFrame::NoFrame);
      QString bg = g_lightMode ? "#FFFFFF" : "#0F1325";
      QString br = g_lightMode ? "#E0D8CC" : "#1A2240";
      v->setStyleSheet(QString(
          "QAbstractItemView { background:%1; border:none; outline:none; }"
      ).arg(bg));
      if (auto* container = v->parentWidget()) {
          container->setStyleSheet(QString(
              "background:%1; border:1px solid %2; border-radius:8px;"
          ).arg(bg, br));
      }
  }

  namespace DS {
      constexpr const char* PRIMARY       = "#00CFFF";
      constexpr const char* PRIMARY_DARK  = "#00A8CC";
      constexpr const char* PRIMARY_LIGHT = "#001E2B";
      constexpr const char* PRIMARY_XLT   = "#001828";
      constexpr const char* SUCCESS       = "#00FF88";
      constexpr const char* SUCCESS_BG    = "#001A10";
      constexpr const char* WARNING       = "#FFB400";
      constexpr const char* WARNING_BG    = "#1A1000";
      constexpr const char* DANGER        = "#FF2D6B";
      constexpr const char* DANGER_BG     = "#1A0010";
      constexpr const char* PURPLE        = "#C040FF";
      constexpr const char* PURPLE_BG     = "#12002A";
      constexpr const char* STU_ACCENT    = "#00CFFF";
      constexpr const char* STF_ACCENT    = "#C040FF";
      constexpr const char* ADM_ACCENT    = "#00FFD4";

      inline const char* BG_PAGE()        { return g_lightMode ? "#F2EDE4" : "#0A0C14"; }
      inline const char* BG_SIDEBAR()     { return g_lightMode ? "#FFFFFF"  : "#06080F"; }
      inline const char* BG_SB_HOVER()    { return g_lightMode ? "#F5F1EA" : "#0D1120"; }
      inline const char* BG_SB_ACTIVE()   { return g_lightMode ? "#EDE8DF" : "#0E1830"; }
      inline const char* BG_CARD()        { return g_lightMode ? "#FFFFFF"  : "#0C0F1C"; }
      inline const char* TEXT_PRIMARY()   { return g_lightMode ? "#1A1A2E"  : "#E2EEFF"; }
      inline const char* TEXT_SECOND()    { return g_lightMode ? "#4A4A6A"  : "#7B8EAA"; }
      inline const char* TEXT_MUTED()     { return g_lightMode ? "#8888AA"  : "#3C4E6A"; }
      inline const char* BORDER()         { return g_lightMode ? "#E0D8CC"  : "#1A2240"; }
      inline const char* BORDER_FOCUS()   { return g_lightMode ? "#4361EE"  : "#00CFFF"; }
      inline const char* SIDEBAR_LOGO_BG(){ return g_lightMode ? "#F5F1EA"  : "#030508"; }
      inline const char* BG_INPUT()       { return g_lightMode ? "#FFFFFF"  : "#0F1325"; }
      inline const char* BG_INPUT_FOCUS() { return g_lightMode ? "#F8F5F0"  : "#0C1A28"; }
      inline const char* SEL_BG()         { return g_lightMode ? "#E8E4DC"  : "#0E2040"; }
  }

  class MainWindow : public QMainWindow {
      Q_OBJECT
  public:
      explicit MainWindow(QWidget* parent = nullptr);
      QFrame*  makeSidebar(const QStringList& items, const QList<int>& pages,
                           int activeIdx, const QString& accent = DS::PRIMARY);
      QWidget* wrapWithSidebar(QWidget* sidebar, QWidget* content);

  private:
      AppData d;
      QStackedWidget* stack;

      QWidget* loginPage();
      QWidget* signUpPage();
      QWidget* studentDashboard();
      QWidget* stuProfilePage();
      QWidget* stuCourseRegPage();
      QWidget* stuGradesPage();
      QWidget* stuAppointmentsPage();
      QWidget* stuRequestAppointmentPage();
      QWidget* stuCourseSchedulePage();
      QWidget* stuExamSchedulePage();
      QWidget* stuHonorListPage();
      QWidget* stuNotificationsPage();
      QWidget* staffDashboard();
      QWidget* stfProfilePage();
      QWidget* stfSetGradesPage();
      QWidget* stfCoursesPage();
      QWidget* stfAppointmentsPage();
      QWidget* stfTopStudentsPage();
      QWidget* stfNotificationsPage();
      QWidget* adminDashboard();
      QWidget* admProfilePage();
      QWidget* admCoursesPage();
      QWidget* admCourseSchedulePage();
      QWidget* admExamSchedulePage();
      QWidget* admNotificationsPage();

      QWidget*     makeCard(const QString& title, QLayout* layout,
                            const QString& accentColor = DS::PRIMARY);
      QPushButton* makeBtn(const QString& text, const QString& color = DS::PRIMARY);
      QPushButton* makeOutlineBtn(const QString& text, const QString& color = DS::PRIMARY);
      QLineEdit*   makeField(const QString& placeholder = "", bool password = false);
      QLabel*      makeLabel(const QString& text, int ptSize = 13, bool bold = false,
                             const QString& color = DS::PRIMARY);
      QWidget*     makeStatCard(const QString& value, const QString& label,
                                const QString& icon, const QString& color);
      QWidget*     makeEmptyState(const QString& icon, const QString& title,
                                  const QString& subtitle = QString());
      void         addShadow(QWidget* w, int blur = 20, int dy = 4, int alpha = 40);
      void         animateFadeIn(QWidget* w, int durationMs = 220);
      void         showMsg(const QString& msg, bool success = true);
      void         refreshPage(int idx);
      void         toggleTheme();
    bool         confirmDialog(const QString& title, const QString& msg,
                              const QString& confirmText = "Confirm",
                              const QString& color = "#FF2D6B");
      int          countUnreadNotif(long long int userId, int userType);

      static constexpr int PAGE_LOGIN        = 0;
      static constexpr int PAGE_SIGNUP       = 1;
      static constexpr int PAGE_STU_DASH     = 2;
      static constexpr int PAGE_STU_PROFILE  = 3;
      static constexpr int PAGE_STU_COURSEREG= 4;
      static constexpr int PAGE_STU_GRADES   = 5;
      static constexpr int PAGE_STU_APPT     = 6;
      static constexpr int PAGE_STU_REQ_APPT = 7;
      static constexpr int PAGE_STU_COSCH    = 8;
      static constexpr int PAGE_STU_EXSCH    = 9;
      static constexpr int PAGE_STU_HONOR    = 10;
      static constexpr int PAGE_STF_DASH     = 11;
      static constexpr int PAGE_STF_PROFILE  = 12;
      static constexpr int PAGE_STF_GRADES   = 13;
      static constexpr int PAGE_STF_COURSES  = 14;
      static constexpr int PAGE_STF_APPT     = 15;
      static constexpr int PAGE_STF_TOP      = 16;
      static constexpr int PAGE_ADM_DASH     = 17;
      static constexpr int PAGE_ADM_PROFILE  = 18;
      static constexpr int PAGE_ADM_COURSES  = 19;
      static constexpr int PAGE_ADM_COSCH    = 20;
      static constexpr int PAGE_ADM_EXSCH    = 21;
      static constexpr int PAGE_STU_NOTIF    = 22;
      static constexpr int PAGE_STF_NOTIF    = 23;
      static constexpr int PAGE_ADM_NOTIF    = 24;

  protected:
      void closeEvent(QCloseEvent* event) override;
  };
  
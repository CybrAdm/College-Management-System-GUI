#pragma once
  #include "ThemeState.h"
  #include <QTableWidget>
  #include <QHeaderView>
  #include <QFrame>
  #include <QWidget>
  #include <QLabel>
  #include <QHBoxLayout>
  #include <QComboBox>
  #include <QAbstractItemView>

  static const char* const TABLE_SS_DARK =
      "QTableWidget{"
      "  background:#0C0F1C; alternate-background-color:#121628;"
      "  border:none; gridline-color:#1A2240;"
      "  selection-background-color:#0A2848; selection-color:#00CFFF;"
      "}"
      "QTableWidget::item{ padding:10px 12px; color:#C8D8F0; border:none; }"
      "QTableWidget::item:selected{ background:#0A2848; color:#00CFFF; border-left:3px solid #00CFFF; }"
      "QTableWidget::item:hover{ background:#111830; }"
      "QHeaderView::section{"
      "  background:#080B18; color:#4A6080;"
      "  font-size:12px; font-weight:700;"
      "  padding:10px 12px; border:none; border-bottom:2px solid #1A2240;"
      "}";

  static const char* const TABLE_SS_LIGHT =
      "QTableWidget{"
      "  background:#FFFFFF; alternate-background-color:#FAF8F4;"
      "  border:none; gridline-color:#E8E2D8;"
      "  selection-background-color:#E8E4DC; selection-color:#1A1A2E;"
      "}"
      "QTableWidget::item{ padding:10px 12px; color:#1A1A2E; border:none; }"
      "QTableWidget::item:selected{ background:#E8E4DC; color:#1A1A2E; border-left:3px solid #4361EE; }"
      "QTableWidget::item:hover{ background:#F5F2EC; }"
      "QHeaderView::section{"
      "  background:#F5F1EA; color:#4A4A6A;"
      "  font-size:12px; font-weight:700;"
      "  padding:10px 12px; border:none; border-bottom:2px solid #E0D8CC;"
      "}";

  inline const char* TABLE_SS_CURRENT() { return g_lightMode ? TABLE_SS_LIGHT : TABLE_SS_DARK; }
  #define TABLE_SS (TABLE_SS_CURRENT())

  inline void styleTable(QTableWidget* t)
  {
      t->setStyleSheet(g_lightMode ? TABLE_SS_LIGHT : TABLE_SS_DARK);
      t->setAlternatingRowColors(true);
      t->setEditTriggers(QAbstractItemView::NoEditTriggers);
      t->verticalHeader()->setVisible(false);
      t->setSelectionBehavior(QAbstractItemView::SelectRows);
      t->setShowGrid(false);
      t->setFrameShape(QFrame::NoFrame);
      t->horizontalHeader()->setHighlightSections(false);
  }

  // ── Theme-aware input / label helpers ──────────────────────────────────────
  inline QString inputLineStyle(const QString& accent) {
      return QString(
          "QLineEdit, QSpinBox{"
          "  border:2px solid %1; border-radius:10px; padding:10px 14px;"
          "  font-size:14px; color:%2; background:%3; }"
          "QLineEdit:hover, QSpinBox:hover{ border-color:%4; }"
          "QLineEdit:focus, QSpinBox:focus{ border-color:%4; background:%5; }"
      ).arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT())
       .arg(accent).arg(DS::BG_INPUT_FOCUS());
  }
  inline QString inputComboStyle(const QString& accent) {
      return QString(
          "QComboBox, QSpinBox{"
          "  border:2px solid %1; border-radius:10px; padding:9px 12px;"
          "  font-size:13px; color:%2; background:%3; }"
          "QComboBox:hover{ border-color:%4; }"
          "QComboBox:focus, QSpinBox:focus{ border-color:%4; }"
          "QComboBox::drop-down{ border:none; padding-right:12px; }"
          "QComboBox::down-arrow{ width:10px; height:10px; }"
          "QComboBox QAbstractItemView{"
          "  background:%3; color:%2; border:1px solid %1; outline:none;"
          "  selection-background-color:%5; selection-color:%2; }"
          "QComboBox QAbstractItemView::item{ padding:7px 12px; min-height:30px; background:%3; color:%2; }"
          "QComboBox QAbstractItemView::item:hover{ background:%5; color:%2; }"
          "QComboBox QAbstractItemView::item:selected{ background:%5; color:%2; }"
      ).arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT())
       .arg(accent).arg(DS::SEL_BG());
  }
  inline QString inputMixedStyle(const QString& accent) {
      return QString(
          "QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox{"
          "  border:2px solid %1; border-radius:10px; padding:9px 12px;"
          "  font-size:13px; color:%2; background:%3; }"
          "QLineEdit:hover, QComboBox:hover, QSpinBox:hover, QDoubleSpinBox:hover{ border-color:%4; }"
          "QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus{ border-color:%4; background:%5; }"
          "QComboBox::drop-down{ border:none; padding-right:12px; }"
          "QComboBox::down-arrow{ width:10px; height:10px; }"
          "QComboBox QAbstractItemView{"
          "  background:%3; color:%2; border:1px solid %1; outline:none;"
          "  selection-background-color:%6; selection-color:%2; }"
          "QComboBox QAbstractItemView::item{ padding:7px 12px; min-height:30px; background:%3; color:%2; }"
          "QComboBox QAbstractItemView::item:hover{ background:%6; color:%2; }"
          "QComboBox QAbstractItemView::item:selected{ background:%6; color:%2; }"
          "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button{ width:20px; border:none; background:transparent; }"
      ).arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT())
       .arg(accent).arg(DS::BG_INPUT_FOCUS()).arg(DS::SEL_BG());
  }
  inline QString labelSS() {
      return QString("color:%1; font-size:13px; font-weight:600; background:transparent;")
          .arg(DS::TEXT_SECOND());
  }
  inline QString labelSmSS() {
      return QString("color:%1; font-size:12px; font-weight:600; background:transparent;")
          .arg(DS::TEXT_SECOND());
  }

  // ── Feature #5: Color-coded Status Chip ────────────────────────────────────
  inline QWidget* makeStatusChip(const QString& status)
  {
      auto* w = new QWidget;
      w->setAttribute(Qt::WA_TranslucentBackground);
      w->setFixedHeight(36);
      auto* L = new QHBoxLayout(w);
      L->setContentsMargins(6,4,6,4);
      L->setAlignment(Qt::AlignCenter);

      QString icon, textColor, bgColor, borderColor;
      if (status == "Approved") {
          icon        = "\u2714  Approved";
          textColor   = g_lightMode ? "#007A45" : "#00CC70";
          bgColor     = g_lightMode ? "rgba(0,160,90,12)" : "rgba(0,255,136,18)";
          borderColor = g_lightMode ? "#00A860" : "#00CC70";
      } else if (status == "Rejected") {
          icon        = "\u2716  Rejected";
          textColor   = g_lightMode ? "#CC0040" : "#FF2D6B";
          bgColor     = g_lightMode ? "rgba(200,0,60,10)" : "rgba(255,45,107,18)";
          borderColor = g_lightMode ? "#CC0040" : "#FF2D6B";
      } else {
          icon        = "\u23F3  Pending";
          textColor   = g_lightMode ? "#996000" : "#FFB400";
          bgColor     = g_lightMode ? "rgba(160,100,0,12)" : "rgba(255,180,0,18)";
          borderColor = g_lightMode ? "#996000" : "#FFB400";
      }

      auto* chip = new QLabel(icon);
      chip->setAlignment(Qt::AlignCenter);
      chip->setFixedHeight(26);
      chip->setMinimumWidth(100);
      chip->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
      chip->setStyleSheet(QString(
          "QLabel{"
          "  background:%1;"
          "  color:%2;"
          "  border:1.5px solid %3;"
          "  border-radius:12px;"
          "  font-size:11px;"
          "  font-weight:700;"
          "  padding:0 14px;"
          "}")
          .arg(bgColor).arg(textColor).arg(borderColor));
      L->addWidget(chip);
      return w;
  }

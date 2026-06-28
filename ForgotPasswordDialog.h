#pragma once
#include <QDialog>
#include <QStackedWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include "DataStructures.h"

class ForgotPasswordDialog : public QDialog {
    Q_OBJECT
public:
    explicit ForgotPasswordDialog(AppData& data, QWidget* parent = nullptr);

private:
    AppData& d;
    QStackedWidget* steps;

    QComboBox*  roleCombo;
    QLineEdit*  idEdit;
    QLabel*     step1Error;

    QLineEdit*  otpEdit;
    QLabel*     step2Info;
    QLabel*     step2Error;
    QString     generatedOtp;
    int         foundRole  = -1;
    int         foundIndex = -1;

    QLineEdit*  newPassEdit;
    QLineEdit*  confPassEdit;
    QLabel*     step3Error;

    QWidget* buildStep1();
    QWidget* buildStep2();
    QWidget* buildStep3();
    QWidget* buildStepDone();

    QPushButton* makeBtn(const QString& text,
                         const QString& bg  = "#00CFFF",
                         const QString& fg  = "#000A14");
    QLineEdit*   makeField(const QString& ph, bool pw = false);
    QLabel*      makeErrorLabel();

    void goStep(int idx);
    void handleVerifyIdentity();
    void handleVerifyOtp();
    void handleSetPassword();
    QString generateOtp();
};

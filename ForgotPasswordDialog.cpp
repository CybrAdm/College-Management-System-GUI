#include "ForgotPasswordDialog.h"
#include "MainWindow.h"
#include "StyleHelpers.h"
#include <QRandomGenerator>

ForgotPasswordDialog::ForgotPasswordDialog(AppData& data, QWidget* parent)
    : QDialog(parent), d(data)
{
    setWindowTitle("Reset Password");
    setFixedWidth(480);
    setModal(true);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(16,16,16,16);
    outer->setSpacing(0);

    auto* card = new QWidget;
    card->setObjectName("card");
    card->setStyleSheet(QString(
        "QWidget#card{ background:%1; border-radius:20px; border:1px solid %2; }")
        .arg(DS::BG_CARD()).arg(DS::BORDER()));
    auto* eff = new QGraphicsDropShadowEffect(card);
    eff->setBlurRadius(48); eff->setOffset(0,12);
    eff->setColor(g_lightMode ? QColor(0,0,0,30) : QColor(0,200,255,80));
    card->setGraphicsEffect(eff);

    auto* cardL = new QVBoxLayout(card);
    cardL->setContentsMargins(0,0,0,0);
    cardL->setSpacing(0);

    auto* hdr = new QWidget;
    hdr->setFixedHeight(72);
    if (g_lightMode) {
        hdr->setStyleSheet(
            "background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #EDE8DF, stop:1 #E0D8CC);"
            "border-radius:20px;");
    } else {
        hdr->setStyleSheet(
            "background:qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #004466, stop:1 #001830);"
            "border-radius:20px;");
    }
    auto* hdrL = new QHBoxLayout(hdr);
    hdrL->setContentsMargins(28,0,28,0);
    auto* iconLbl   = new QLabel("\U0001F510");
    iconLbl->setStyleSheet("font-size:24px; background:transparent;");
    auto* hdrTitle  = new QLabel("Reset Your Password");
    hdrTitle->setStyleSheet(QString(
        "color:%1; font-size:18px; font-weight:700; background:transparent; letter-spacing:0.5px;").arg(DS::TEXT_PRIMARY()));
    auto* closeBtn  = new QPushButton("\u2715");
    closeBtn->setFixedSize(32,32);
    closeBtn->setStyleSheet(QString(
        "QPushButton{ background:transparent; color:%1;"
        "border:1px solid %2; border-radius:16px; font-size:14px; font-weight:700; }"
        "QPushButton:hover{ background:%2; color:%3; }")
        .arg(DS::TEXT_SECOND()).arg(DS::BORDER()).arg(DS::BG_CARD()));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
    hdrL->addWidget(iconLbl);
    hdrL->addSpacing(10);
    hdrL->addWidget(hdrTitle);
    hdrL->addStretch();
    hdrL->addWidget(closeBtn);
    cardL->addWidget(hdr);

    steps = new QStackedWidget;
    steps->setStyleSheet(QString("background:%1;").arg(DS::BG_CARD()));
    steps->addWidget(buildStep1());
    steps->addWidget(buildStep2());
    steps->addWidget(buildStep3());
    steps->addWidget(buildStepDone());
    cardL->addWidget(steps);

    outer->addWidget(card);
    adjustSize();
}

QWidget* ForgotPasswordDialog::buildStep1() {
    auto* w = new QWidget; w->setStyleSheet(QString("background:%1;").arg(DS::BG_CARD()));
    auto* L = new QVBoxLayout(w);
    L->setContentsMargins(36,30,36,36); L->setSpacing(0);

    auto* stepBadge = new QLabel("STEP 1 OF 3");
    stepBadge->setStyleSheet(QString("color:%1; font-size:11px; font-weight:700; letter-spacing:1.5px; background:transparent;").arg(DS::BORDER_FOCUS()));
    auto* titleLbl = new QLabel("Verify Your Identity");
    titleLbl->setStyleSheet(QString("color:%1; font-size:20px; font-weight:700; background:transparent; margin-bottom:4px;").arg(DS::TEXT_PRIMARY()));
    auto* subLbl = new QLabel("Enter your role and ID to receive a reset code.");
    subLbl->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));
    subLbl->setWordWrap(true);

    L->addWidget(stepBadge);
    L->addSpacing(6);
    L->addWidget(titleLbl);
    L->addWidget(subLbl);
    L->addSpacing(24);

    auto* roleLbl = new QLabel("Account Type");
    roleLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND()));
    roleCombo = new QComboBox;
    roleCombo->addItems({"Student", "Academic Staff", "Administration"});
    roleCombo->setStyleSheet(inputComboStyle(DS::BORDER_FOCUS()));
    applyComboDelegate(roleCombo);

    auto* idLbl = new QLabel("Your ID Number");
    idLbl->setStyleSheet(roleLbl->styleSheet());
    idEdit = makeField("e.g. 100005");
    step1Error = makeErrorLabel();
    auto* nextBtn = makeBtn("Continue  \u2192");
    connect(nextBtn, &QPushButton::clicked, this, &ForgotPasswordDialog::handleVerifyIdentity);

    L->addWidget(roleLbl); L->addSpacing(6); L->addWidget(roleCombo);
    L->addSpacing(16);
    L->addWidget(idLbl); L->addSpacing(6); L->addWidget(idEdit);
    L->addSpacing(10); L->addWidget(step1Error);
    L->addSpacing(24); L->addWidget(nextBtn);
    L->addStretch();
    return w;
}

QWidget* ForgotPasswordDialog::buildStep2() {
    auto* w = new QWidget; w->setStyleSheet(QString("background:%1;").arg(DS::BG_CARD()));
    auto* L = new QVBoxLayout(w);
    L->setContentsMargins(36,30,36,36); L->setSpacing(0);

    auto* stepBadge = new QLabel("STEP 2 OF 3");
    stepBadge->setStyleSheet("color:#C040FF; font-size:11px; font-weight:700; letter-spacing:1.5px; background:transparent; border:none;");
    auto* titleLbl = new QLabel("Enter Reset Code");
    titleLbl->setStyleSheet(QString("color:%1; font-size:20px; font-weight:700; background:transparent; margin-bottom:4px;").arg(DS::TEXT_PRIMARY()));

    step2Info = new QLabel("");
    step2Info->setWordWrap(true);
    step2Info->setStyleSheet(QString(
        "color:%1; background:%2; border:1px solid %3;"
        "border-radius:10px; padding:12px 16px; font-size:13px; font-weight:600;")
        .arg(DS::TEXT_PRIMARY())
        .arg(g_lightMode ? "#F5F1EA" : "#001828")
        .arg(DS::BORDER_FOCUS()));

    auto* otpLbl = new QLabel("Reset Code");
    otpLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND()));
    otpEdit = makeField("Enter 6-digit code");
    step2Error = makeErrorLabel();

    auto* rowL = new QHBoxLayout;
    auto* backBtn   = makeBtn("\u2190 Back", g_lightMode ? "#E0D8CC" : "#1A2240", g_lightMode ? DS::TEXT_PRIMARY() : "#7B8EAA");
    auto* verifyBtn = makeBtn("Verify Code  \u2192");
    connect(backBtn,   &QPushButton::clicked, this, [this]{ goStep(0); });
    connect(verifyBtn, &QPushButton::clicked, this, &ForgotPasswordDialog::handleVerifyOtp);
    rowL->addWidget(backBtn); rowL->addSpacing(12); rowL->addWidget(verifyBtn);

    L->addWidget(stepBadge); L->addSpacing(6); L->addWidget(titleLbl);
    L->addSpacing(16); L->addWidget(step2Info);
    L->addSpacing(20); L->addWidget(otpLbl); L->addSpacing(6); L->addWidget(otpEdit);
    L->addSpacing(10); L->addWidget(step2Error);
    L->addSpacing(24); L->addLayout(rowL);
    L->addStretch();
    return w;
}

QWidget* ForgotPasswordDialog::buildStep3() {
    auto* w = new QWidget; w->setStyleSheet(QString("background:%1;").arg(DS::BG_CARD()));
    auto* L = new QVBoxLayout(w);
    L->setContentsMargins(36,30,36,36); L->setSpacing(0);

    auto* stepBadge = new QLabel("STEP 3 OF 3");
    stepBadge->setStyleSheet(QString("color:%1; font-size:11px; font-weight:700; letter-spacing:1.5px; background:transparent; border:none;").arg(DS::SUCCESS));
    auto* titleLbl = new QLabel("Set New Password");
    titleLbl->setStyleSheet(QString("color:%1; font-size:20px; font-weight:700; background:transparent; margin-bottom:4px;").arg(DS::TEXT_PRIMARY()));
    auto* subLbl = new QLabel("Choose a strong password for your account.");
    subLbl->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));

    auto* newLbl  = new QLabel("New Password");
    newLbl->setStyleSheet(QString("color:%1; font-size:13px; font-weight:600; background:transparent;").arg(DS::TEXT_SECOND()));
    newPassEdit  = makeField("Enter new password", true);
    auto* confLbl = new QLabel("Confirm Password");
    confLbl->setStyleSheet(newLbl->styleSheet());
    confPassEdit = makeField("Re-enter new password", true);
    step3Error   = makeErrorLabel();

    auto* rowL = new QHBoxLayout;
    auto* backBtn = makeBtn("\u2190 Back", g_lightMode ? "#E0D8CC" : "#1A2240", g_lightMode ? DS::TEXT_PRIMARY() : "#7B8EAA");
    auto* saveBtn = makeBtn("Save Password  \u2713", "#00FF88", "#000A14");
    connect(backBtn, &QPushButton::clicked, this, [this]{ goStep(1); });
    connect(saveBtn, &QPushButton::clicked, this, &ForgotPasswordDialog::handleSetPassword);
    rowL->addWidget(backBtn); rowL->addSpacing(12); rowL->addWidget(saveBtn);

    L->addWidget(stepBadge); L->addSpacing(6); L->addWidget(titleLbl); L->addWidget(subLbl);
    L->addSpacing(24);
    L->addWidget(newLbl); L->addSpacing(6); L->addWidget(newPassEdit);
    L->addSpacing(16);
    L->addWidget(confLbl); L->addSpacing(6); L->addWidget(confPassEdit);
    L->addSpacing(10); L->addWidget(step3Error);
    L->addSpacing(24); L->addLayout(rowL);
    L->addStretch();
    return w;
}

QWidget* ForgotPasswordDialog::buildStepDone() {
    auto* w = new QWidget; w->setStyleSheet(QString("background:%1;").arg(DS::BG_CARD()));
    auto* L = new QVBoxLayout(w);
    L->setContentsMargins(36,40,36,40);
    L->setAlignment(Qt::AlignCenter);

    auto* icon = new QLabel("\u2705");
    icon->setStyleSheet("font-size:52px; background:transparent;");
    icon->setAlignment(Qt::AlignCenter);
    auto* title = new QLabel("Password Reset!");
    title->setStyleSheet("color:#00FF88; font-size:22px; font-weight:700; background:transparent;");
    title->setAlignment(Qt::AlignCenter);
    auto* sub = new QLabel("Your password has been updated successfully.\nYou can now sign in with your new password.");
    sub->setStyleSheet(QString("color:%1; font-size:13px; background:transparent;").arg(DS::TEXT_SECOND()));
    sub->setAlignment(Qt::AlignCenter);
    sub->setWordWrap(true);
    auto* doneBtn = makeBtn("Back to Sign In  \u2192");
    doneBtn->setFixedHeight(52);
    connect(doneBtn, &QPushButton::clicked, this, &QDialog::accept);

    L->addWidget(icon);
    L->addSpacing(14);
    L->addWidget(title);
    L->addSpacing(10);
    L->addWidget(sub);
    L->addSpacing(32);
    L->addWidget(doneBtn);
    return w;
}

void ForgotPasswordDialog::handleVerifyIdentity() {
    step1Error->hide();
    long long id = idEdit->text().toLongLong();
    int role = roleCombo->currentIndex();
    foundIndex = -1; foundRole = role;
    if (role == 0)      { for (int i = 0; i < d.numOfStu; i++) if (d.stu[i].inf.id == id) { foundIndex = i; break; } }
    else if (role == 1) { for (int i = 0; i < d.numOfStf; i++) if (d.stf[i].inf.id == id) { foundIndex = i; break; } }
    else                { for (int i = 0; i < d.numOfAdm; i++) if (d.adm[i].inf.id == id) { foundIndex = i; break; } }

    if (foundIndex < 0) {
        step1Error->setText("  \u2717  ID not found. Please check and try again.");
        step1Error->show(); return;
    }
    generatedOtp = generateOtp();
    QString name = (role == 0) ? d.stu[foundIndex].inf.name
                 : (role == 1) ? d.stf[foundIndex].inf.name
                 :               d.adm[foundIndex].inf.name;
    step2Info->setText(
        QString("Hello, %1!\nYour reset code is:  %2\n"
                "(In a real system this would be sent to your registered email/phone.)")
            .arg(name).arg(generatedOtp));
    step2Error->hide(); otpEdit->clear(); goStep(1);
}

void ForgotPasswordDialog::handleVerifyOtp() {
    step2Error->hide();
    if (otpEdit->text().trimmed() != generatedOtp) {
        step2Error->setText("  \u2717  Invalid reset code. Please try again.");
        step2Error->show(); return;
    }
    step3Error->hide(); newPassEdit->clear(); confPassEdit->clear(); goStep(2);
}

void ForgotPasswordDialog::handleSetPassword() {
    step3Error->hide();
    QString np = newPassEdit->text();
    QString cp = confPassEdit->text();
    if (np.isEmpty()) { step3Error->setText("  \u2717  Password cannot be empty."); step3Error->show(); return; }
    if (np.length() < 3) { step3Error->setText("  \u2717  Password must be at least 3 characters."); step3Error->show(); return; }
    if (np != cp) { step3Error->setText("  \u2717  Passwords do not match."); step3Error->show(); return; }
    if (foundRole == 0)      d.stu[foundIndex].inf.password = np;
    else if (foundRole == 1) d.stf[foundIndex].inf.password = np;
    else                     d.adm[foundIndex].inf.password = np;
    goStep(3);
}

void ForgotPasswordDialog::goStep(int idx) { steps->setCurrentIndex(idx); adjustSize(); }

QString ForgotPasswordDialog::generateOtp() {
    return QString("%1").arg(QRandomGenerator::global()->bounded(100000, 999999));
}

QPushButton* ForgotPasswordDialog::makeBtn(const QString& text, const QString& bg, const QString& fg) {
    auto* b = new QPushButton(text);
    b->setFixedHeight(48);
    b->setCursor(Qt::PointingHandCursor);
    QColor c(bg);
    QString hover  = c.lighter(115).name();
    QString fgCol  = fg.isEmpty() ? (c.lightness() > 160 ? "#0D1B4B" : "white") : fg;
    b->setStyleSheet(QString(
        "QPushButton{ background:%1; color:%2; border:none; border-radius:10px;"
        "  font-size:14px; font-weight:700; letter-spacing:0.3px; }"
        "QPushButton:hover{ background:%3; }"
        "QPushButton:pressed{ background:%1; }").arg(bg).arg(fgCol).arg(hover));
    return b;
}

QLineEdit* ForgotPasswordDialog::makeField(const QString& ph, bool pw) {
    auto* e = new QLineEdit;
    if (pw) e->setEchoMode(QLineEdit::Password);
    e->setPlaceholderText(ph);
    e->setFixedHeight(48);
    e->setStyleSheet(QString(
        "QLineEdit{ border:2px solid %1; border-radius:10px;"
        "  padding:0 14px; font-size:14px; color:%2; background:%3; }"
        "QLineEdit:focus{ border-color:%4; background:%5; }")
        .arg(DS::BORDER()).arg(DS::TEXT_PRIMARY()).arg(DS::BG_INPUT())
        .arg(DS::BORDER_FOCUS()).arg(DS::BG_INPUT_FOCUS()));
    return e;
}

QLabel* ForgotPasswordDialog::makeErrorLabel() {
    auto* l = new QLabel;
    l->setStyleSheet(QString(
        "color:#CC0040; background:%1; border:1px solid #CC0040;"
        "border-radius:8px; padding:8px 12px; font-size:12px; font-weight:600;")
        .arg(g_lightMode ? "#FFF0F4" : "#1A0010"));
    l->setWordWrap(true);
    l->hide();
    return l;
}

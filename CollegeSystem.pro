QT += core gui widgets

CONFIG += c++17

TARGET = CollegeSystem
TEMPLATE = app

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    MainWindow_Student.cpp \
    MainWindow_Staff.cpp \
    MainWindow_Admin.cpp \
    MainWindow_Messages.cpp \
    Logic.cpp \
    ForgotPasswordDialog.cpp

HEADERS += \
    ThemeState.h \
    MainWindow.h \
    DataStructures.h \
    Logic.h \
    StyleHelpers.h \
    ForgotPasswordDialog.h

RESOURCES += \
    resources.qrc \
    resources.qrc

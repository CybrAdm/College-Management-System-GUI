# 🎓 College Management System

A desktop college management system built in C++ with the Qt Framework, featuring a modern graphical interface that handles students, academic staff, and administration — all from a single unified application.

---

## 📋 Overview

This system simulates the core operations of a college environment through a full GUI desktop application. Users can sign up and sign in as one of three roles, each with its own dashboard and set of features.

| Role | Description |
|------|-------------|
| 🧑‍🎓 Student | Register for courses, view grades & GPA, schedule appointments, view exam/course schedules |
| 👨‍🏫 Academic Staff | Assign grades, manage teaching courses, handle appointment requests |
| 🏛️ Administration | Manage courses, course schedules, and exam schedules |

---

## ✨ Features

### 🧑‍🎓 Student
- View & edit personal profile
- Register / drop courses by academic year
- Request & view appointments with staff members
- View course schedules and exam timetables
- Check grades per course and calculate GPA
- View top 5 honor students in their year
- Receive real-time notifications (grades, schedules, appointments)

### 👨‍🏫 Academic Staff
- View & edit personal profile
- Select / remove teaching courses
- Assign grades (final, practical, year work, quiz)
- Approve or reject student appointment requests
- View top 5 students per course
- Receive real-time notifications

### 🏛️ Administration
- View & edit personal profile
- Add / remove / view courses
- Set, view, and remove course schedules (with conflict detection)
- Set, view, and remove exam schedules
- Automatic student notifications on schedule changes

### 🔐 Authentication
- Role-based sign up & sign in
- Multi-step password reset dialog (identity verification → OTP → new password)
- Unique mobile number validation across all users

### 🎨 Interface
- Dark / Light theme toggle across the entire application
- Custom-styled sidebar navigation per role
- Interactive dashboards with stat cards, tables, and charts (GPA trend, grade bar chart)
- Notification center with unread badges and mark-as-read actions

---

## 🛠️ Tech Stack

| | |
|---|---|
| **Language** | C++ |
| **Framework** | Qt (Widgets) |
| **Data Structures** | Structs & fixed-size arrays |
| **Algorithms** | Bubble sort (for top student & honor list rankings) |
| **Persistence** | QDataStream binary serialization |

---

## 📐 Data Model

```
information        → shared by all users (ID, name, mobile, password, address)
student            → extends information + department, year, courses, GPA
staff              → extends information + department, position, teaching courses
administration     → extends information + position
course             → name, credit hours, grade scheme, enrolled students
enrollment         → per-student grades (final, practical, year work, quiz)
appointment        → student ↔ staff scheduling with status (Pending/Approved/Rejected)
exam               → course + date/time schedule
courseSchedule     → weekday + time per course
notification       → per-user alerts with read/unread state
AppData            → root container holding all system data
```

---

## 🚀 Getting Started

### Option 1: Download the Ready-to-Run Executable

No need to build anything — download the latest release, extract it, and run `CollegeSystem.exe` directly.

👉 [Download from Releases](../../releases/latest)

### Option 2: Build from Source

1. **Clone the repository**
   ```bash
   git clone https://github.com/CybrAdm/College-Management-System-GUI.git
   ```

2. **Open in Qt Creator**
   - Open the `.pro` file
   - Select your Kit (MinGW or MSVC)

3. **Build & Run**
   - Set build mode to `Release` or `Debug`
   - Build the project (`Ctrl+B`)
   - Run (`Ctrl+R`)

---

## 👥 Sample Credentials

| Role | ID | Password |
|------|----|----------|
| Student | 100000 | 123 |
| Staff | 200000 | 256 |
| Admin | 300000 | 256 |

---

## 📁 Project Structure

```
main.cpp                    → application entry point
MainWindow.h/.cpp           → main window, login, sign-up, shared UI helpers
MainWindow_Student.cpp      → student dashboard & pages
MainWindow_Staff.cpp        → staff dashboard & pages
MainWindow_Admin.cpp        → administration dashboard & pages
MainWindow_Messages.cpp     → notifications system
ForgotPasswordDialog.h/.cpp → password recovery dialog
Logic.h/.cpp                → business logic, grading, GPA, save/load
DataStructures.h            → all data structs & AppData container
StyleHelpers.h              → shared UI styling helpers
ThemeState.h                → global light/dark mode state
```

---

## 💾 Data Persistence

All application data is saved automatically to `college_data.dat` when the application closes, and reloaded automatically on the next launch. See `SAVE_LOAD.txt` for full details.

---

## 🤝 Contributors

| Name |
|------|
| Adam Ahmed Abdelbaset |
| Yousef Allam Abdullah |
| Marwan Mohammed Abed |
| Yousef Ehab Makram |
| Salah Eldin Ibrahim Salah Eldin |
| Malek Mohammed ElShazly |
| Philopater Farag Nabih |

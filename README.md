# MetroLink 🚇

MetroLink is a **console-based metro management system** written in **C**, developed as a first-year Computer Science project.  
It simulates a small metro network and provides features for users and administrators.

---

## 📌 Introduction

MetroLink helps manage metro stations, connections, users, fares, and bookings through a menu-driven C application.

---

## 🎯 Objectives

- Manage metro station data and connections
- Enable user signup/login and metro ticket booking
- Maintain a wallet system for users
- Provide admin controls to edit station & connection data
- Compute shortest path and fare between stations

---

## ✅ Features

### 👤 User Features
- Signup and login
- View all stations
- Book tickets with shortest path and fare
- Wallet deposit and balance
- View user transaction history
- Stored user data

### 🛠️ Admin Features
- Admin login
- Add/remove stations
- Create/remove links between stations
- Update pricing and rates
- Update database files

---

## 📁 Project Structure

├── admin.c
├── database.c
├── database.h
├── database.txt
├── pricing.txt
├── rates.c
├── rates.h
├── specs.txt
├── user.c
├── usercounter.txt
├── users.txt
└── README.md


---

## ▶️ How to Compile & Run

### Compile all source files
gcc database.c rates.c user.c admin.c -o metro_exec

### Running the combined build
./metro_exec


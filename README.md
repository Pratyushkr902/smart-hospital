# 🏥 Smart Hospital Management System

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Next.js](https://img.shields.io/badge/Frontend-Next.js%2014-black)](https://nextjs.org/)
[![Tailwind CSS](https://img.shields.io/badge/Styling-Tailwind%20CSS-38bdf8)](https://tailwindcss.com/)
[![C++20](https://img.shields.io/badge/Backend-C%2B%2B20%20%2F%20Crow-00599C)](https://crowcpp.org/)
[![Database](https://img.shields.io/badge/Database-SQLite-003B57)](https://www.sqlite.org/)

An intelligent, multi-role hospital management system featuring dynamic triage patient prioritization, role-based dashboards, and a high-performance backend architecture.

---

## 🏗️ Architecture & Tech Stack

```
Smart Hospital
├── Frontend (Next.js 14 App Router + Tailwind CSS)
│   ├── Reception Portal (/reception/checkin, /reception/queue)
│   ├── Doctor Portal (/doctor/queue)
│   ├── Admin Portal (/admin/overview, /admin/config, /admin/log)
│   └── Patient Portal (/patient)
│
├── Priority Engine
│   └── Multi-factor urgency scoring (symptoms, vitals, pain, age, risk, wait bonus)
│
└── Backend (C++20 + Crow REST API + SQLite)
    ├── Auth & RBAC (Admin, Doctor, Receptionist, Patient)
    ├── Core Priority Engine (std::priority_queue + std::unordered_map)
    └── Relational Persistence (SQLite)
```

---

## 🌟 Key Features

- **Role-Based Portals**:
  - **Receptionist**: Patient check-in, triage data capture (symptoms, vitals, pain scale, medical history), token generation.
  - **Doctor**: Real-time prioritized patient queue, consultation completion, medical notes.
  - **Admin**: Hospital-wide triage metrics, dynamic weight configuration, audit log tracking.
  - **Patient**: Live token tracking, queue position, estimated wait time.
- **Dynamic Priority Scoring**:
  - Scores patients between 0–100 using clinical severity and risk factors.
  - **Inversion Prevention**: Waiting-time bonus dynamically elevates long-waiting patients to prevent starvation.
- **High-Performance Core**:
  - Uses C++ STL `std::priority_queue` for $O(\log n)$ triage scheduling and `std::unordered_map` for $O(1)$ patient lookups.

---

## 🚀 Getting Started

### Prerequisites

- Node.js (v18.17 or higher)
- npm or yarn

### Installation & Run

```bash
# Clone the repository
git clone https://github.com/Pratyushkr902/smart-hospital.git
cd smart-hospital

# Install dependencies
npm install

# Start development server
npm run dev
```

Open [http://localhost:3000](http://localhost:3000) in your browser.

---

## 📂 Project Structure

```
app/
  page.js                 # Role selector / demo login
  reception/checkin/      # Patient check-in & triage input
  reception/queue/        # Departmental waiting queue
  doctor/queue/           # Doctor consultation queue
  admin/overview/         # Hospital-wide analytics
  admin/config/           # Live priority weight adjustments
  admin/log/              # Audit trail log
  patient/                # Live patient token tracker
components/               # Reusable UI widgets & table components
context/AppProvider.js    # Central state provider (queue, logs, config)
lib/engine.js             # Pure priority triage scoring functions
lib/mockData.js           # Initial seed data for departments & doctors
```

---

## 🤝 Contributing

We welcome contributions from developers of all skill levels! Whether you want to improve the **React/Next.js frontend** or build out the **C++20 Crow REST API / SQLite backend**, check out [CONTRIBUTING.md](CONTRIBUTING.md) for details on:
- Setting up the development environment
- Current open task roadmap
- Branching and pull request guidelines

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

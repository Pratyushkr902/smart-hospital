# Contributing to Smart Hospital Management System

Thank you for your interest in contributing to the **Smart Hospital Management System**! This project combines a modern React / Next.js frontend with a high-performance C++20 / Crow backend.

---

## 🏥 Project Overview & Architecture

- **Frontend**: Next.js 14 (App Router) + Tailwind CSS
  - Portals for **Reception**, **Doctor**, **Admin**, and **Patient**.
  - In-memory simulation & triage calculation (`lib/engine.js`).
- **Backend (In Progress / Open for Contributions)**:
  - Language: **C++20**
  - Web Framework: **Crow** (REST API)
  - Core Algorithm: **STL Priority Queue** (`std::priority_queue` + `std::unordered_map`) for real-time triage scoring and patient scheduling.
  - Storage: **SQLite** for relational records (patients, doctors, triage logs, audit trails).
  - Security: **RBAC** (Role-Based Access Control) & JWT Authentication.

---

## 🚀 How to Contribute

### 1. Frontend Tasks (React / Next.js)
- [ ] Connect React Context (`context/AppProvider.js`) to backend REST API endpoints via `fetch` / TanStack Query.
- [ ] Add real-time queue polling or WebSocket listeners for token call notifications.
- [ ] Enhance mobile responsiveness and accessibility (a11y).
- [ ] Add E2E tests (Playwright or Cypress).

### 2. Backend Tasks (C++20 & Crow)
- [ ] **Crow REST API Server**: Implement HTTP routing, CORS headers, JSON request/response parsing with `nlohmann/json`.
- [ ] **Priority Queue Engine**: Port `lib/engine.js` logic to C++20 `PriorityEngine` class using `std::priority_queue` with custom comparators.
- [ ] **SQLite Database Layer**: Create schema migrations and repository classes for users, patients, doctors, and audit logs.
- [ ] **Auth & RBAC**: Implement JWT token generation, password hashing, and role-based route middleware (`Admin`, `Doctor`, `Receptionist`, `Patient`).

---

## 🛠️ Getting Started

### Local Frontend Development

1. Fork and clone the repository:
   ```bash
   git clone https://github.com/Pratyushkr902/smart-hospital.git
   cd smart-hospital
   ```
2. Install dependencies:
   ```bash
   npm install
   ```
3. Run the development server:
   ```bash
   npm run dev
   ```
4. Open [http://localhost:3000](http://localhost:3000) in your browser.

---

## 🌿 Git Workflow

1. Fork the repository on GitHub.
2. Create a feature branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```
3. Commit your changes with clear, descriptive commit messages:
   ```bash
   git commit -m "feat: implement patient search in reception queue"
   ```
4. Push to your branch:
   ```bash
   git push origin feature/your-feature-name
   ```
5. Open a Pull Request (PR) against the `main` branch.

---

## 📋 Code of Conduct

Please be respectful, collaborative, and constructive when reviewing code, discussing issues, or proposing architectural improvements.

# Smart Hospital Management System

A high-performance hospital triage and queue management system combining a modern Next.js console with a C++20 Crow REST backend, pure C data structures, and SQLite3 persistence.

---

## 🏗️ Architecture

```
Smart Hospital
├── Frontend (Next.js 14 + Tailwind CSS)
│   ├── Reception Portal (/reception/checkin, /reception/queue)
│   ├── Doctor Portal (/doctor/queue)
│   ├── Admin Portal (/admin/overview, /admin/config, /admin/log)
│   └── Patient Portal (/patient)
│
├── Core REST Backend (C++20 + Crow Framework)
│   ├── Crow REST Routing (CROW_ROUTE /api/...)
│   ├── Role-Based Access Control (Admin, Doctor, Receptionist, Patient)
│   ├── Relational Persistence Layer (SQLite3 hospital.db)
│   └── Real-time State Rehydration on server reboot
│
└── Data Structures Engine (Pure C & C++ STL)
    ├── Binary Max-Heap (heap.c) — O(1) peek, O(log N) clinical priority dispatch
    ├── Self-Balancing AVL Tree (avl_tree.c) — O(log N) patient indexing
    ├── Chaining Hash Table (hash_table.c) — DJB2 rolling hash, O(1) token lookup
    ├── Circular FIFO Queue (queue.c) — O(1) general outpatient walk-in queue
    └── Doubly Linked List (linked_list.c) — O(1) node deletion & queue rearrangement
```

---

## 🚀 Quick Start

### 1. Run the C++ Crow REST API Server

```bash
make server
```
*The Crow server starts at `http://localhost:8080` with SQLite database `hospital.db` and CORS enabled.*

### 2. Run the Data Structures Test Suite

```bash
make run
```
*Compiles and benchmarks all 5 pure C data structures and C++ OOP models.*

### 3. Run the Next.js Frontend

```bash
npm install
npm run dev
```
*Open [http://localhost:3000](http://localhost:3000) in your browser.*

---

## 📂 Project Structure

```
smart-hospital/
├── app/                        # Next.js App Router (Reception, Doctor, Admin, Patient)
├── components/                 # Reusable UI widgets & Dashboard console
├── context/AppProvider.js      # State provider & Crow REST API integration
├── lib/engine.js               # Clinical priority scoring algorithms
│
└── backend/                    # Core C & C++ Engine
    ├── c_ds/                   # Pure C Data Structures
    │   ├── heap.h & .c         # Binary Max-Heap (Dynamic Priority Queue)
    │   ├── avl_tree.h & .c     # Self-Balancing AVL Binary Search Tree
    │   ├── hash_table.h & .c   # Hash Table with Separate Chaining
    │   ├── queue.h & .c        # Circular FIFO Queue
    │   └── linked_list.h & .c  # Doubly Linked List
    │
    ├── cpp_oop/                # C++20 Object-Oriented Architecture
    │   ├── models/User.hpp     # Abstract Base Class, RBAC permissions
    │   ├── models/Roles.hpp    # Derived Classes: Doctor, Receptionist, Admin
    │   ├── models/Patient.hpp  # Encapsulation, Vitals, Operator Overloading (<, <<)
    │   └── engine/             # Strategy Pattern & STL Priority Queue
    │
    ├── db/DatabaseManager.hpp  # SQLite3 Relational Persistence & State Rehydration
    ├── include/crow.h          # Crow REST API Framework Header
    ├── server.cpp              # Multi-threaded Crow REST Server (port 8080)
    ├── main.cpp                # Algorithmic simulation & benchmark runner
    └── Makefile                # Multi-target build system (clang / clang++ C++20)
```

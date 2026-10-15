# Smart Hospital Management System (Frontend + C/C++ PBL Engine)

A complete Smart Hospital Management System featuring:
- **Frontend**: Next.js (App Router) + Tailwind CSS with portals for Receptionist, Doctor, Admin, and Patient.
- **Backend / Core Engine**:
  - **Data Structures in Pure C**: Binary Max-Heap (Priority Queue), Doubly Linked List, Chaining Hash Table.
  - **OOP in Modern C++ (C++20)**: Inheritance, Polymorphism (RBAC), Encapsulation, Operator Overloading (`operator<`, `operator<<`), Strategy Design Pattern, and Smart Pointers (RAII).
  - **C++ STL Integration**: `std::priority_queue`, `std::unordered_map`, `std::stack`, `std::vector`.

---

## 🚀 How to Run

### 1. Run C / C++ PBL Engine & DSA Benchmarks

```bash
make run
```
Or directly from the backend folder:
```bash
cd backend
make run
```

### 2. Run Next.js Frontend

```bash
npm install
npm run dev
```
Then open [http://localhost:3000](http://localhost:3000) in your browser.

---

## 📂 Project Structure

```
smart-hospital/
├── app/                        # Next.js App Router (Reception, Doctor, Admin, Patient)
├── components/                 # Shared UI components & Dashboard shells
├── context/AppProvider.js      # Central state store (queue, audit log, config)
├── lib/engine.js               # Frontend priority engine matching C++ spec
│
└── backend/                    # C & C++ Core Engine
    ├── c_ds/                   # Pure C Data Structures
    │   ├── heap.h & .c         # Binary Max-Heap (Dynamic Priority Queue)
    │   ├── linked_list.h & .c  # Doubly Linked List
    │   └── hash_table.h & .c   # Hash Table with Separate Chaining
    │
    ├── cpp_oop/                # C++ Object-Oriented Architecture
    │   ├── models/User.hpp     # Abstract Base Class, Virtual Methods, RBAC
    │   ├── models/Roles.hpp    # Derived Classes: Doctor, Receptionist, Admin, Patient
    │   ├── models/Patient.hpp  # Encapsulation & Operator Overloading (<, <<)
    │   └── engine/             # Strategy Pattern & STL Hospital System
    │
    ├── main.cpp                # Comprehensive runnable simulation
    ├── Makefile                # Build system (clang + clang++ C++20)
    └── PBL_CONCEPTS_GUIDE.md   # Complete academic viva & project guide
```

---

## 📖 PBL & DSA Guide
For complete documentation of all data structures, time complexities, OOP design patterns, and viva evaluation Q&A, refer to [`backend/PBL_CONCEPTS_GUIDE.md`](backend/PBL_CONCEPTS_GUIDE.md).

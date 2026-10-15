# 🎓 Smart Hospital Management System — PBL Concepts Guide

This guide documents all **Data Structures (implemented in C & C++)** and **Object-Oriented Programming (OOP) concepts (implemented in C++)** designed for academic evaluation, project reports, and viva presentations.

---

## 📑 Table of Contents
1. [Data Structures in Pure C](#1-data-structures-in-pure-c)
2. [Data Structures in C++ (STL)](#2-data-structures-in-c-stl)
3. [OOP Concepts in C++](#3-oop-concepts-in-c)
4. [Design Patterns & Algorithms](#4-design-patterns--algorithms)
5. [How to Compile and Run](#5-how-to-compile-and-run)
6. [Top Viva Questions & Answers](#6-top-viva-questions--answers)

---

## 1. Data Structures in Pure C

Located in [`backend/c_ds/`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/):

### A. Binary Max-Heap (Priority Queue)
- **Files**: [`heap.h`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/heap.h), [`heap.c`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/heap.c)
- **Hospital Problem**: In the emergency ward, patients cannot be seen first-come, first-served. Critical patients (heart attack, trauma) must be seen before patients with mild fever.
- **Concept & Implementation**:
  - Array representation of a Complete Binary Tree:
    - $\text{Parent}(i) = \lfloor (i - 1) / 2 \rfloor$
    - $\text{Left Child}(i) = 2i + 1$
    - $\text{Right Child}(i) = 2i + 2$
  - `heapify_up`: Restores max-heap invariant after insertion.
  - `heapify_down`: Restores max-heap invariant after root extraction.
  - **Dynamic Array Resizing**: Doubles capacity when full (`realloc`).
- **Complexity**:
  - Insert: $O(\log N)$
  - Peek Max: $O(1)$
  - Extract Max: $O(\log N)$

### B. Doubly Linked List
- **Files**: [`linked_list.h`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/linked_list.h), [`linked_list.c`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/linked_list.c)
- **Hospital Problem**: Managing the physical waiting room line where patients can cancel their token, get transferred to ICU, or move seats without shifting hundreds of array elements.
- **Concept & Implementation**:
  - Nodes with both `prev` and `next` pointers.
  - Pointers: `head` and `tail` tracking.
  - Fast node removal: `target->prev->next = target->next; target->next->prev = target->prev; free(target);`
- **Complexity**:
  - Insert at Head/Tail: $O(1)$
  - Delete given node pointer: $O(1)$
  - Search by Token: $O(N)$

### C. Chaining Hash Table
- **Files**: [`hash_table.h`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/hash_table.h), [`hash_table.c`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/c_ds/hash_table.c)
- **Hospital Problem**: When a patient arrives at reception or enters their token `T-1004` on a kiosk, looking up their record must be instant ($O(1)$), not requiring a linear scan through all hospital records.
- **Concept & Implementation**:
  - **DJB2 Hash Function**: Computes polynomial rolling hash for string keys: `hash = ((hash << 5) + hash) + c`.
  - **Separate Chaining**: Handles hash collisions by creating a linked list at each bucket index.
- **Complexity**:
  - Average Search/Insert/Delete: $O(1)$
  - Worst Case: $O(N)$ (when all keys collide into one bucket)

---

## 2. Data Structures in C++ (STL)

Located in [`backend/cpp_oop/engine/HospitalSystem.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/engine/HospitalSystem.hpp):

| STL Container | Purpose in Smart Hospital | Complexity |
| :--- | :--- | :--- |
| `std::priority_queue<Patient>` | Manages the dynamic emergency triage queue ordered by effective clinical urgency. | Push/Pop: $O(\log N)$, Top: $O(1)$ |
| `std::unordered_map<std::string, Patient>` | Primary key index for instant patient search by Token (`T-1002`). | Average $O(1)$ lookup |
| `std::stack<AuditLogEntry>` | Tracks hospital actions (check-ins, calls, edits) in reverse chronological order (LIFO). | Push/Pop: $O(1)$ |
| `std::vector<std::shared_ptr<User>>` | Polymorphic collection of active hospital staff with automatic lifetime management. | Random access: $O(1)$ |

---

## 3. OOP Concepts in C++

Located in [`backend/cpp_oop/`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/):

### A. Encapsulation & Data Hiding
- **File**: [`Patient.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/models/Patient.hpp)
- Member variables (`urgencyScore`, `vitals`, `effectiveScore`, `status`) are `private`.
- Public methods (`calculateScores()`, `addWaitTime()`, getters/setters) enforce clinical rules:
  - Pain score is validated between 0–10.
  - Scores are clamped between 0–100.

### B. Inheritance & Role-Based Access Control (RBAC)
- **Files**: [`User.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/models/User.hpp), [`Roles.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/models/Roles.hpp)
- Base abstract class `User`:
  ```cpp
  class User {
  protected:
      std::string id, name;
      Role role;
  public:
      virtual ~User() = default;
      virtual std::string getRoleName() const = 0; // Pure virtual
      virtual bool hasPermission(Permission p) const = 0;
  };
  ```
- Derived classes: `Doctor`, `Receptionist`, `Administrator`, `PatientUser`.
- Each derived class implements specific permissions (e.g., only `Doctor` has `CALL_PATIENT`, only `Receptionist` has `CHECKIN_PATIENT`).

### C. Dynamic Polymorphism & Virtual Functions
- Virtual methods allow calling `user->hasPermission()` or `user->printDashboardSummary()` through a `std::shared_ptr<User>` pointer, resolving the correct role's implementation at runtime via the **vtable (virtual table)**.

### D. Operator Overloading
- **File**: [`Patient.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/models/Patient.hpp)
- **Comparison Operator (`operator<`)**:
  - Overloaded so that `std::priority_queue<Patient>` automatically orders patients by clinical urgency.
  - Includes a tie-breaker: if two patients have identical scores, the patient who arrived earlier (FIFO) gets priority.
- **Stream Insertion (`operator<<`)**:
  - Formats patient details cleanly for `std::cout << patient;`.

### E. RAII & Smart Pointers
- `std::unique_ptr<IPriorityScorer>`: Exclusive ownership of the scoring strategy with zero memory leak risk.
- `std::shared_ptr<User>`: Shared ownership of hospital staff objects across queues and audit logs.

---

## 4. Design Patterns & Algorithms

### A. Strategy Design Pattern
- **Files**: [`IPriorityScorer.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/engine/IPriorityScorer.hpp), [`ClinicalTriageScorer.hpp`](file:///Users/pratyushkumar/Documents/smart-hospital/backend/cpp_oop/engine/ClinicalTriageScorer.hpp)
- Enables swapping the triage scoring algorithm at runtime (e.g., `ClinicalTriageScorer` vs `PureFIFOScorer`) without modifying the `HospitalSystem` class (Open-Closed Principle).

### B. Starvation Prevention (Aging Algorithm)
- **Problem**: In a pure priority queue, a low-urgency patient might suffer starvation if critical patients keep arriving.
- **Solution**: Dynamic wait bonus:
  $$\text{Effective Score} = \min(\text{Urgency Score} + \lfloor \text{Wait Minutes} / 10 \rfloor, 100)$$
- As time elapses, waiting patients gradually advance to the front of the queue.

---

## 5. How to Compile and Run

From the root project directory:
```bash
# Navigate to backend and run
cd backend
make run
```
Or from the project root:
```bash
make -C backend run
```

---

## 6. Top Viva Questions & Answers

1. **Q: Why is a Binary Heap better than a Sorted Linked List for priority queues?**
   - **A**: A sorted linked list requires $O(N)$ time to insert a new patient. A Binary Heap performs both insertion and extraction in $O(\log N)$, making it much faster as hospital queues grow.
2. **Q: What is the purpose of virtual destructors in the base `User` class?**
   - **A**: If a base class pointer `User*` pointing to a derived `Doctor` object is deleted without a `virtual ~User()`, only the base class destructor executes, causing memory leaks for derived class members.
3. **Q: How does the system achieve $O(1)$ patient token search while maintaining $O(\log N)$ priority dispatch?**
   - **A**: By pairing a Max-Heap (`std::priority_queue`) for triage scheduling with a Hash Map (`std::unordered_map`) for token indexing.
4. **Q: How does your system prevent priority inversion and starvation?**
   - **A**: The system implements an aging algorithm where each patient earns $+1$ bonus point for every 10 minutes spent waiting in the queue.


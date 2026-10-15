/**
 * @file main.cpp
 * @brief Smart Hospital Management System
 * Comprehensive demonstration of:
 * - Data Structures in pure C (Binary Max-Heap, Doubly Linked List, Chaining Hash Table)
 * - OOP Concepts in C++ (Inheritance, Polymorphism, Encapsulation, Operator Overloading, Strategy Pattern, RAII)
 * - C++ STL (std::priority_queue, std::unordered_map, std::stack, std::shared_ptr)
 */

#include "c_ds/heap.h"
#include "c_ds/linked_list.h"
#include "c_ds/hash_table.h"

#include "cpp_oop/models/User.hpp"
#include "cpp_oop/models/Roles.hpp"
#include "cpp_oop/models/Patient.hpp"
#include "cpp_oop/engine/HospitalSystem.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

void printHeader(const std::string& title) {
    std::cout << "\n======================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "======================================================================\n";
}

// ----------------------------------------------------------------------
// PART 1: DEMO OF DATA STRUCTURES IMPLEMENTED IN PURE C
// ----------------------------------------------------------------------
void runPureCDataStructuresDemo() {
    printHeader("PART 1: DATA STRUCTURES IMPLEMENTED IN PURE C (Structs & Pointers)");

    std::cout << "\n[1.1] PURE C BINARY MAX-HEAP (Dynamic Priority Queue)\n";
    std::cout << "      Concept: Parent = (i-1)/2, Left = 2i+1, Right = 2i+2. O(log N) insert/extract.\n\n";

    C_MaxHeap* heap = heap_create(4);

    C_PatientRecord p1 = {"PT-201", "T-1001", "Asha Rao", 34, 45, 55, 50};
    C_PatientRecord p2 = {"PT-202", "T-1002", "Imran Qureshi", 61, 72, 6, 72};
    C_PatientRecord p3 = {"PT-203", "T-1003", "Fatima Sheikh", 28, 30, 80, 38};
    C_PatientRecord p4 = {"PT-204", "T-1004", "Vikram Nair", 45, 58, 22, 60};
    C_PatientRecord p5 = {"PT-205", "T-1005", "Leela Menon", 72, 85, 15, 86};

    std::cout << "-> Inserting 5 patient records into C Max-Heap...\n";
    heap_insert(heap, p1);
    heap_insert(heap, p2);
    heap_insert(heap, p3);
    heap_insert(heap, p4);
    heap_insert(heap, p5);

    heap_print(heap);

    C_PatientRecord topPatient;
    if (heap_peek(heap, &topPatient)) {
        std::cout << "\n-> O(1) Heap Peek: Highest priority is " << topPatient.name 
                  << " (" << topPatient.token << ") with Score: " << topPatient.effectiveScore << "\n";
    }

    std::cout << "\n-> Extracting highest priority patients from C Heap (Order of triage):\n";
    while (heap->size > 0) {
        C_PatientRecord next;
        heap_extract_max(heap, &next);
        std::cout << "   [Dispatched] Token: " << next.token << " | " << std::setw(15) << std::left << next.name 
                  << " | Urgency Score: " << next.effectiveScore << "\n";
    }
    heap_destroy(heap);

    // Doubly Linked List Demo
    std::cout << "\n[1.2] PURE C DOUBLY LINKED LIST (Patient Arrival Queue)\n";
    std::cout << "      Concept: Node with prev/next pointers. O(1) insertion at head/tail, O(1) node deletion.\n\n";

    C_DoublyLinkedList* list = list_create();
    list_append(list, p1);
    list_append(list, p2);
    list_append(list, p3);
    list_append(list, p4);

    list_print(list);

    std::cout << "\n-> Removing patient with Token T-1002 from middle of Doubly Linked List...\n";
    C_PatientRecord removed;
    if (list_remove_by_token(list, "T-1002", &removed)) {
        std::cout << "   Successfully removed: " << removed.name << " (" << removed.token << ")\n";
    }
    list_print(list);
    list_destroy(list);

    // Hash Table Demo
    std::cout << "\n[1.3] PURE C HASH TABLE (Token to Patient Lookup with Separate Chaining)\n";
    std::cout << "      Concept: DJB2 hash algorithm + linked bucket chains. Average O(1) search.\n\n";

    C_HashTable* table = hash_table_create();
    hash_table_put(table, "T-1001", p1);
    hash_table_put(table, "T-1002", p2);
    hash_table_put(table, "T-1003", p3);
    hash_table_put(table, "T-1004", p4);
    hash_table_put(table, "T-1005", p5);

    hash_table_print(table);

    std::cout << "\n-> Looking up Token 'T-1004' in C Hash Table (O(1))...\n";
    C_PatientRecord found;
    if (hash_table_get(table, "T-1004", &found)) {
        std::cout << "   FOUND: Patient " << found.name << " | Age: " << found.age 
                  << " | Score: " << found.effectiveScore << "\n";
    }
    hash_table_destroy(table);
}

// ----------------------------------------------------------------------
// PART 2: DEMO OF OOP CONCEPTS IMPLEMENTED IN C++
// ----------------------------------------------------------------------
void runCppOOPConceptsDemo() {
    printHeader("PART 2: OBJECT-ORIENTED PROGRAMMING (OOP) IN C++");

    std::cout << "\n[2.1] INHERITANCE & POLYMORPHISM (Role-Based Access Control)\n";
    std::cout << "      Concept: Abstract class User -> derived Doctor, Receptionist, Administrator.\n\n";

    std::vector<std::shared_ptr<User>> users;
    users.push_back(std::make_shared<Receptionist>("U-01", "P. Menon", "Front Desk 1"));
    users.push_back(std::make_shared<Doctor>("D-101", "Dr. A. Sharma", "Cardiology", "Room 302"));
    users.push_back(std::make_shared<Administrator>("ADM-01", "S. Kulkarni", "Director"));

    // Dynamic Polymorphic Call
    for (const auto& user : users) {
        user->printDashboardSummary();
        std::cout << "   - Can Check-in Patients? " 
                  << (user->hasPermission(Permission::CHECKIN_PATIENT) ? "YES" : "NO") << "\n";
        std::cout << "   - Can Call Patients?     " 
                  << (user->hasPermission(Permission::CALL_PATIENT) ? "YES" : "NO") << "\n";
    }

    std::cout << "\n[2.2] OPERATOR OVERLOADING & ENCAPSULATION (Patient Class)\n";
    std::cout << "      Concept: Overloaded operator< for priority comparison, and operator<< for stream formatting.\n\n";

    ClinicalVitals v1{110, 145, 94, 8, 22};
    Patient patientA("PT-1001", "T-101", "Ramesh Sen", 68, "M", "Cardiology", "D-101", v1, {"cardiac"}, 15);

    ClinicalVitals v2{78, 120, 99, 3, 5};
    Patient patientB("PT-1002", "T-102", "Sunita Devi", 25, "F", "General Medicine", "D-104", v2, {}, 60);

    // Stream operator overloading
    std::cout << "Patient A: " << patientA << "\n";
    std::cout << "Patient B: " << patientB << "\n";

    // Operator< comparison test
    std::cout << "\nComparison: Is Patient B < Patient A in priority? "
              << (patientB < patientA ? "YES (Patient A has higher clinical priority)" : "NO") << "\n";
}

// ----------------------------------------------------------------------
// PART 3: COMPLETE SMART HOSPITAL SYSTEM SIMULATION
// ----------------------------------------------------------------------
void runHospitalSystemSimulation() {
    printHeader("PART 3: FULL SMART HOSPITAL WORKFLOW (C++ STL Priority Queue + Map + Stack)");

    HospitalSystem hospital;

    auto receptionist = hospital.getStaff()[0]; // P. Menon
    auto doctor = hospital.getStaff()[1];       // Dr. A. Sharma

    std::cout << "\n-> [RECEPTIONIST] Checking in patients with triage vitals...\n";

    ClinicalVitals vit1{75, 120, 98, 4, 10};  // Mild
    std::string t1 = hospital.checkInPatient(receptionist, "Asha Rao", 34, "F", "General Medicine", "D-104", vit1, {"diabetic"}, 50);

    ClinicalVitals vit2{130, 165, 89, 9, 25}; // Critical Cardiac
    std::string t2 = hospital.checkInPatient(receptionist, "Imran Qureshi", 61, "M", "Cardiology", "D-101", vit2, {"cardiac"}, 5);

    ClinicalVitals vit3{82, 115, 97, 5, 8};   // Moderate
    std::string t3 = hospital.checkInPatient(receptionist, "Fatima Sheikh", 28, "F", "General Medicine", "D-104", vit3, {"pregnant"}, 45);

    ClinicalVitals vit4{140, 175, 85, 10, 25}; // Trauma / Critical
    std::string t4 = hospital.checkInPatient(receptionist, "Vikram Nair", 45, "M", "Emergency", "D-101", vit4, {}, 2);

    std::cout << "   Current Patients in Priority Queue: " << hospital.getQueueSize() << "\n";

    // Instant O(1) Token search using std::unordered_map
    std::cout << "\n-> [FAST LOOKUP] Searching Patient by Token 'T-1002' via std::unordered_map (O(1))...\n";
    Patient searched("","", "", 0, "", "", "", {}, {});
    if (hospital.findPatientByToken(t2, searched)) {
        std::cout << "   FOUND: " << searched << "\n";
    }

    // Doctor calling next critical patients (demonstrating Max-Heap dispatch)
    std::cout << "\n-> [DOCTOR CONSULTATION] Dr. Sharma calls the most urgent patients from Max-Heap:\n";
    Patient called("","", "", 0, "", "", "", {}, {});
    int turn = 1;
    while (hospital.callNextPatient(doctor, called)) {
        std::cout << "   [Call #" << turn++ << "] " << called.getName() 
                  << " (Token: " << called.getToken() << ") - Effective Urgency Score: " 
                  << called.getEffectiveScore() << "\n";
    }

    // Audit Stack Inspection (LIFO)
    hospital.printAuditStack(5);
}

int main() {
    std::cout << R"(
  ╔═══════════════════════════════════════════════════════════════════╗
  ║       SMART HOSPITAL MANAGEMENT SYSTEM — PBL ENGINE (C & C++)     ║
  ║  Data Structures: Max-Heap | Doubly Linked List | Hash Table      ║
  ║  OOP: Inheritance | Polymorphism | Encapsulation | Strategy | RAII║
  ╚═══════════════════════════════════════════════════════════════════╝
    )" << "\n";

    runPureCDataStructuresDemo();
    runCppOOPConceptsDemo();
    runHospitalSystemSimulation();

    printHeader("ALL DATA STRUCTURES & OOP CHECKS COMPLETED SUCCESSFULLY");
    return 0;
}


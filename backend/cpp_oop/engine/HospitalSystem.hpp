/**
 * @file HospitalSystem.hpp
 * @brief High-level Smart Hospital Engine combining C++ STL & OOP:
 * - std::priority_queue<Patient> (Binary Heap priority queue)
 * - std::unordered_map<string, Patient> (Hash Map for O(1) patient lookup)
 * - std::stack<AuditLogEntry> (Stack for LIFO Audit Trail & Undo)
 * - std::vector<shared_ptr<User>> (Polymorphic user directory)
 * - Smart Pointers (std::unique_ptr, std::shared_ptr) for RAII
 */

#pragma once

#include "../models/User.hpp"
#include "../models/Roles.hpp"
#include "../models/Patient.hpp"
#include "IPriorityScorer.hpp"
#include "ClinicalTriageScorer.hpp"

#include <queue>
#include <unordered_map>
#include <stack>
#include <vector>
#include <memory>
#include <iostream>
#include <stdexcept>
#include <sstream>

struct AuditLogEntry {
    std::string timestamp;
    std::string actor;
    std::string action;
};

class HospitalSystem {
private:
    // C++ STL Data Structures
    std::priority_queue<Patient> triageQueue;                     // Max-Heap: O(log N)
    std::unordered_map<std::string, Patient> patientRegistry;      // Hash Map: O(1)
    std::stack<AuditLogEntry> auditTrail;                          // Stack: LIFO
    std::vector<std::shared_ptr<User>> staffMembers;               // Polymorphic container
    
    // Strategy Pattern for Triage Scoring
    std::unique_ptr<IPriorityScorer> priorityScorer;
    ScoringConfig scoringConfig;

    int tokenCounter = 1000;

    std::string generateNextToken() {
        tokenCounter++;
        return "T-" + std::to_string(tokenCounter);
    }

    std::string getCurrentTimeStr() const {
        return "10:30 AM";
    }

public:
    HospitalSystem()
        : priorityScorer(std::make_unique<ClinicalTriageScorer>()) {
        seedInitialStaff();
    }

    void seedInitialStaff() {
        staffMembers.push_back(std::make_shared<Receptionist>("U-1", "P. Menon", "Desk-A"));
        staffMembers.push_back(std::make_shared<Doctor>("D-101", "Dr. A. Sharma", "Emergency Medicine", "Room 102"));
        staffMembers.push_back(std::make_shared<Doctor>("D-104", "Dr. R. Iyer", "General Medicine", "Room 105"));
        staffMembers.push_back(std::make_shared<Administrator>("ADM-01", "S. Kulkarni", "Director"));
    }

    // Role-Based Patient Check-in (Receptionist permission checked)
    std::string checkInPatient(const std::shared_ptr<User>& actor,
                               const std::string& name, int age,
                               const std::string& gender, const std::string& dept,
                               const std::string& docId, ClinicalVitals vitals,
                               const std::vector<std::string>& risks, int waitMins = 0) {
        if (!actor->hasPermission(Permission::CHECKIN_PATIENT)) {
            throw std::runtime_error("RBAC Denied: User does not have CHECKIN_PATIENT permission!");
        }

        std::string token = generateNextToken();
        std::string pId = "PT-" + std::to_string(20000 + (tokenCounter % 1000));
        Patient p(pId, token, name, age, gender, dept, docId, vitals, risks, waitMins);

        // Insert into both Priority Queue and Hash Map
        triageQueue.push(p);
        patientRegistry.insert({token, p});

        // Push to Audit Stack
        logAction(actor->getName(), "Checked in " + name + " (" + token + ") with initial score " + std::to_string(p.getEffectiveScore()));
        return token;
    }

    // Doctor calls the next highest-urgency patient from the Max-Heap
    bool callNextPatient(const std::shared_ptr<User>& actor, Patient& outPatient) {
        if (!actor->hasPermission(Permission::CALL_PATIENT)) {
            throw std::runtime_error("RBAC Denied: Only Doctors/Admins can call patients!");
        }

        if (triageQueue.empty()) {
            return false;
        }

        outPatient = triageQueue.top(); // O(1) peek
        triageQueue.pop();              // O(log N) extraction

        outPatient.setStatus(PatientStatus::IN_TREATMENT);
        patientRegistry[outPatient.getToken()] = outPatient;

        logAction(actor->getName(), "Called next critical patient: " + outPatient.getName() + " (" + outPatient.getToken() + ") [Score: " + std::to_string(outPatient.getEffectiveScore()) + "]");
        return true;
    }

    // O(1) lookup using std::unordered_map
    bool findPatientByToken(const std::string& token, Patient& outPatient) const {
        auto it = patientRegistry.find(token);
        if (it != patientRegistry.end()) {
            outPatient = it->second;
            return true;
        }
        return false;
    }

    // Advance time to demonstrate aging and starvation prevention
    void simulateTimeAdvance(int minutes) {
        std::vector<Patient> waitingPatients;
        while (!triageQueue.empty()) {
            Patient p = triageQueue.top();
            triageQueue.pop();
            p.addWaitTime(minutes);
            patientRegistry[p.getToken()] = p;
            waitingPatients.push_back(p);
        }

        for (const auto& p : waitingPatients) {
            triageQueue.push(p);
        }

        logAction("System", "Advanced simulation clock by " + std::to_string(minutes) + " mins (Wait bonus recalculated)");
    }

    void logAction(const std::string& who, const std::string& action) {
        auditTrail.push({getCurrentTimeStr(), who, action});
    }

    // Print the audit stack (LIFO order)
    void printAuditStack(int maxEntries = 5) {
        std::cout << "\n   ┌─── Audit Trail Stack (LIFO - Most Recent First) ───┐\n";
        std::stack<AuditLogEntry> temp = auditTrail;
        int count = 0;
        while (!temp.empty() && count < maxEntries) {
            const auto& entry = temp.top();
            std::cout << "   │ " << entry.timestamp << " | " << std::left << std::setw(18) << entry.actor
                      << " | " << entry.action << "\n";
            temp.pop();
            count++;
        }
        std::cout << "   └────────────────────────────────────────────────────┘\n";
    }

    size_t getQueueSize() const { return triageQueue.size(); }
    const std::vector<std::shared_ptr<User>>& getStaff() const { return staffMembers; }
};

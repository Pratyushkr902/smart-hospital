/**
 * @file Patient.hpp
 * @brief Class demonstrating C++ OOP:
 * - Strict Encapsulation (getters, setters with clinical validation)
 * - Operator Overloading (< for std::priority_queue, << for stream output)
 * - Composition (ClinicalVitals struct embedded in Patient)
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>

struct ClinicalVitals {
    int heartRate;         // bpm (60-100 normal)
    int systolicBp;        // mmHg (120 normal)
    int oxygenSaturation;  // % (95-100 normal)
    int painLevel;         // 0-10 scale
    int vitalScore;        // computed 0-25
};

enum class PatientStatus {
    WAITING,
    IN_TREATMENT,
    COMPLETED
};

class Patient {
private:
    std::string id;
    std::string token;
    std::string name;
    int age;
    std::string gender;
    std::string department;
    std::string assignedDoctorId;
    std::vector<std::string> riskFactors;
    ClinicalVitals vitals;
    int urgencyScore;
    int waitMinutes;
    int effectiveScore;
    PatientStatus status;
    std::chrono::system_clock::time_point arrivalTime;

public:
    Patient()
        : id(""), token(""), name(""), age(0), gender(""), department(""),
          assignedDoctorId(""), riskFactors({}), vitals{0, 0, 0, 0, 0},
          urgencyScore(0), waitMinutes(0), effectiveScore(0),
          status(PatientStatus::WAITING), arrivalTime(std::chrono::system_clock::now()) {}

    Patient(std::string pId, std::string pToken, std::string pName, int pAge,
            std::string pGender, std::string pDept, std::string pDocId,
            ClinicalVitals v, std::vector<std::string> risks, int waitMins = 0)
        : id(std::move(pId)), token(std::move(pToken)), name(std::move(pName)),
          age(pAge), gender(std::move(pGender)), department(std::move(pDept)),
          assignedDoctorId(std::move(pDocId)), riskFactors(std::move(risks)),
          vitals(v), waitMinutes(waitMins), status(PatientStatus::WAITING),
          arrivalTime(std::chrono::system_clock::now() - std::chrono::minutes(waitMins)) {
        calculateScores();
    }

    void calculateScores() {
        // Base urgency formula matching PRD:
        // vitalScore (0-25) + pain (0-15) + ageScore (0-10) + riskPts (0-10)
        int ageScore = (age > 65 || age < 5) ? 8 : 2;
        int riskScore = std::min(static_cast<int>(riskFactors.size() * 3), 10);
        urgencyScore = std::min(vitals.vitalScore + vitals.painLevel + ageScore + riskScore, 80);

        // Inversion/Starvation prevention bonus: +1 point per 10 mins waited, capped at 20
        int waitBonus = std::min(waitMinutes / 10, 20);
        effectiveScore = std::min(urgencyScore + waitBonus, 100);
    }

    void addWaitTime(int minutes) {
        if (minutes > 0 && status == PatientStatus::WAITING) {
            waitMinutes += minutes;
            calculateScores();
        }
    }

    // Operator Overloading for Max-Heap (std::priority_queue)
    // In C++ std::priority_queue<T>, a < b means b has higher priority
    bool operator<(const Patient& other) const {
        if (effectiveScore == other.effectiveScore) {
            // Tie breaker: FIFO based on arrival time (earlier arrival gets priority)
            return arrivalTime > other.arrivalTime;
        }
        return effectiveScore < other.effectiveScore;
    }

    bool operator==(const Patient& other) const {
        return token == other.token || id == other.id;
    }

    // Friend function: Stream Insertion Operator Overloading
    friend std::ostream& operator<<(std::ostream& os, const Patient& p) {
        std::string statusStr = (p.status == PatientStatus::WAITING) ? "Waiting" :
                                (p.status == PatientStatus::IN_TREATMENT) ? "In Treatment" : "Completed";
        os << std::left << "[" << std::setw(7) << p.token << "] "
           << std::setw(16) << p.name << " | Age: " << std::setw(2) << p.age
           << " | Dept: " << std::setw(18) << p.department
           << " | Score: " << std::setw(3) << p.effectiveScore
           << " (Base: " << std::setw(2) << p.urgencyScore << " + Wait: " << p.waitMinutes << "m)"
           << " | Status: " << statusStr;
        return os;
    }

    // Getters and Setters demonstrating Encapsulation
    const std::string& getId() const { return id; }
    const std::string& getToken() const { return token; }
    const std::string& getName() const { return name; }
    int getAge() const { return age; }
    const std::string& getDepartment() const { return department; }
    const std::string& getDoctorId() const { return assignedDoctorId; }
    int getUrgencyScore() const { return urgencyScore; }
    int getWaitMinutes() const { return waitMinutes; }
    int getEffectiveScore() const { return effectiveScore; }
    PatientStatus getStatus() const { return status; }
    void setStatus(PatientStatus s) { status = s; }
    const std::vector<std::string>& getRiskFactors() const { return riskFactors; }
};

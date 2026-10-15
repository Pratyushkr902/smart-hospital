/**
 * @file Roles.hpp
 * @brief Derived Classes demonstrating C++ OOP:
 * - Public Inheritance
 * - Method Overriding (override keyword)
 * - Dynamic Polymorphism
 */

#pragma once

#include "User.hpp"
#include <unordered_set>

// 1. Receptionist Role
class Receptionist : public User {
private:
    std::string deskId;

public:
    Receptionist(std::string userId, std::string userName, std::string desk)
        : User(std::move(userId), std::move(userName), Role::RECEPTIONIST), deskId(std::move(desk)) {}

    std::string getRoleName() const override { return "Receptionist"; }

    bool hasPermission(Permission permission) const override {
        return permission == Permission::CHECKIN_PATIENT || permission == Permission::VIEW_AUDIT_LOG;
    }

    void printDashboardSummary() const override {
        std::cout << "[Reception Portal] User: " << name << " (" << id << ") at Desk " << deskId << "\n";
    }

    const std::string& getDeskId() const { return deskId; }
};

// 2. Doctor Role
class Doctor : public User {
private:
    std::string department;
    std::string roomNumber;

public:
    Doctor(std::string userId, std::string userName, std::string dept, std::string room)
        : User(std::move(userId), std::move(userName), Role::DOCTOR),
          department(std::move(dept)), roomNumber(std::move(room)) {}

    std::string getRoleName() const override { return "Doctor"; }

    bool hasPermission(Permission permission) const override {
        return permission == Permission::CALL_PATIENT || permission == Permission::COMPLETE_CONSULTATION;
    }

    void printDashboardSummary() const override {
        std::cout << "[Doctor Portal] Dr. " << name << " | Dept: " << department << " | Room: " << roomNumber << "\n";
    }

    const std::string& getDepartment() const { return department; }
    const std::string& getRoomNumber() const { return roomNumber; }
};

// 3. Administrator Role
class Administrator : public User {
private:
    std::string adminLevel;

public:
    Administrator(std::string userId, std::string userName, std::string level = "SuperAdmin")
        : User(std::move(userId), std::move(userName), Role::ADMINISTRATOR), adminLevel(std::move(level)) {}

    std::string getRoleName() const override { return "Administrator"; }

    bool hasPermission(Permission) const override {
        return true; // Admins have full access
    }

    void printDashboardSummary() const override {
        std::cout << "[Admin Portal] Administrator: " << name << " (Level: " << adminLevel << ")\n";
    }
};

// 4. Patient User Role
class PatientUser : public User {
private:
    std::string token;

public:
    PatientUser(std::string userId, std::string userName, std::string patientToken)
        : User(std::move(userId), std::move(userName), Role::PATIENT), token(std::move(patientToken)) {}

    std::string getRoleName() const override { return "Patient"; }

    bool hasPermission(Permission permission) const override {
        return permission == Permission::TRACK_OWN_QUEUE;
    }

    void printDashboardSummary() const override {
        std::cout << "[Patient Portal] " << name << " | Token: " << token << "\n";
    }

    const std::string& getToken() const { return token; }
};


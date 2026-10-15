/**
 * @file User.hpp
 * @brief Abstract Base Class demonstrating C++ OOP:
 * - Data Abstraction & Encapsulation
 * - Pure Virtual Functions (Interface)
 * - Virtual Destructor (for safe polymorphic cleanup)
 * - Enum class & RBAC (Role-Based Access Control)
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>

enum class Role {
    RECEPTIONIST,
    DOCTOR,
    ADMINISTRATOR,
    PATIENT
};

enum class Permission {
    CHECKIN_PATIENT,
    CALL_PATIENT,
    COMPLETE_CONSULTATION,
    MODIFY_SCORING_CONFIG,
    VIEW_AUDIT_LOG,
    TRACK_OWN_QUEUE
};

class User {
protected:
    std::string id;
    std::string name;
    Role role;

public:
    User(std::string userId, std::string userName, Role userRole)
        : id(std::move(userId)), name(std::move(userName)), role(userRole) {}

    virtual ~User() = default; // Essential for polymorphic deletion

    // Pure virtual methods (making User an abstract class)
    virtual std::string getRoleName() const = 0;
    virtual bool hasPermission(Permission permission) const = 0;
    virtual void printDashboardSummary() const = 0;

    // Getters demonstrating encapsulation
    const std::string& getId() const { return id; }
    const std::string& getName() const { return name; }
    Role getRole() const { return role; }
};

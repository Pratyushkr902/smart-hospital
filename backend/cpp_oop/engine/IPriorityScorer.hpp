/**
 * @file IPriorityScorer.hpp
 * @brief Strategy Pattern Interface for Triage Priority Calculation
 * 
 * Demonstrates:
 * - Strategy Design Pattern
 * - Interface segregation
 * - Pure virtual functions
 */

#pragma once

#include "../models/Patient.hpp"

struct ScoringConfig {
    int vitalMax = 25;
    int painMax = 15;
    int ageMax = 10;
    int riskMax = 10;
    int waitInterval = 10; // minutes per bonus point
    int waitCap = 20;      // max bonus points
};

class IPriorityScorer {
public:
    virtual ~IPriorityScorer() = default;
    virtual int calculateScore(const Patient& patient, const ScoringConfig& config) const = 0;
    virtual std::string getStrategyName() const = 0;
};


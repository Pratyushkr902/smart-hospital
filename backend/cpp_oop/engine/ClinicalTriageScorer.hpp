/**
 * @file ClinicalTriageScorer.hpp
 * @brief Concrete Strategy implementation matching PRD Section 7
 */

#pragma once

#include "IPriorityScorer.hpp"
#include <algorithm>

class ClinicalTriageScorer : public IPriorityScorer {
public:
    std::string getStrategyName() const override {
        return "Clinical Multi-Factor Triage with Starvation Prevention";
    }

    int calculateScore(const Patient& patient, const ScoringConfig& config) const override {
        int base = patient.getUrgencyScore();
        int waitBonus = std::min(patient.getWaitMinutes() / config.waitInterval, config.waitCap);
        return std::min(base + waitBonus, 100);
    }
};

class PureFIFOScorer : public IPriorityScorer {
public:
    std::string getStrategyName() const override {
        return "Standard First-In-First-Out (FIFO) Baseline";
    }

    int calculateScore(const Patient& patient, const ScoringConfig&) const override {
        // Pure FIFO: score is purely based on how long patient waited
        return patient.getWaitMinutes();
    }
};


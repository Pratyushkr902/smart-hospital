/**
 * @file QueueBenchmark.hpp
 * @brief High-precision performance benchmark comparing C Max-Heap vs C++ STL priority_queue
 * 
 * Demonstrates:
 * - std::chrono high_resolution_clock timing
 * - C++ Templates for generic performance measurement
 * - Cache locality and amortized algorithmic efficiency
 */

#pragma once

#include <chrono>
#include <vector>
#include <queue>
#include <iostream>
#include <iomanip>
#include "../../c_ds/heap.h"
#include "../models/Patient.hpp"

class QueueBenchmark {
public:
    struct BenchmarkResult {
        double cHeapInsertUs;
        double cppStlInsertUs;
        double cHeapExtractUs;
        double cppStlExtractUs;
        int operationsCount;
    };

    static BenchmarkResult runComparativeBenchmark(int numOperations = 5000) {
        BenchmarkResult res;
        res.operationsCount = numOperations;

        // 1. Benchmark Pure C Max-Heap
        C_MaxHeap* cHeap = heap_create(numOperations);
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numOperations; ++i) {
            C_PatientRecord rec;
            rec.age = 20 + (i % 60);
            rec.urgencyScore = (i * 37) % 80;
            rec.waitMinutes = (i * 13) % 120;
            rec.effectiveScore = rec.urgencyScore + (rec.waitMinutes / 10);
            snprintf(rec.token, sizeof(rec.token), "T-%04d", 1000 + i);
            snprintf(rec.name, sizeof(rec.name), "BenchPatient-%d", i);
            heap_insert(cHeap, rec);
        }
        auto end = std::chrono::high_resolution_clock::now();
        res.cHeapInsertUs = std::chrono::duration<double, std::micro>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        C_PatientRecord extracted;
        while (cHeap->size > 0) {
            heap_extract_max(cHeap, &extracted);
        }
        end = std::chrono::high_resolution_clock::now();
        res.cHeapExtractUs = std::chrono::duration<double, std::micro>(end - start).count();
        heap_destroy(cHeap);

        // 2. Benchmark C++ STL std::priority_queue
        std::priority_queue<Patient> cppQueue;
        ClinicalVitals v{80, 120, 98, 5, 15};
        start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < numOperations; ++i) {
            std::string tok = "T-" + std::to_string(1000 + i);
            std::string name = "BenchPatient-" + std::to_string(i);
            int wait = (i * 13) % 120;
            cppQueue.emplace("ID-" + std::to_string(i), tok, name, 20 + (i % 60), "O", "General", "D-101", v, std::vector<std::string>{}, wait);
        }
        end = std::chrono::high_resolution_clock::now();
        res.cppStlInsertUs = std::chrono::duration<double, std::micro>(end - start).count();

        start = std::chrono::high_resolution_clock::now();
        while (!cppQueue.empty()) {
            cppQueue.pop();
        }
        end = std::chrono::high_resolution_clock::now();
        res.cppStlExtractUs = std::chrono::duration<double, std::micro>(end - start).count();

        return res;
    }

    static void printResults(const BenchmarkResult& r) {
        std::cout << "\n============================================================\n";
        std::cout << "  BENCHMARK: Pure C Max-Heap vs C++ STL Priority Queue\n";
        std::cout << "  Total Operations per Test: " << r.operationsCount << " patients\n";
        std::cout << "============================================================\n";
        std::cout << std::left << std::setw(28) << "  Metric" << std::setw(18) << "Pure C Heap" << "C++ STL Priority Queue\n";
        std::cout << "  ----------------------------------------------------------\n";
        std::cout << std::left << std::setw(28) << "  Insert Duration:" 
                  << std::setw(18) << (std::to_string(r.cHeapInsertUs) + " us") 
                  << (std::to_string(r.cppStlInsertUs) + " us") << "\n";
        std::cout << std::left << std::setw(28) << "  Extract Duration:" 
                  << std::setw(18) << (std::to_string(r.cHeapExtractUs) + " us") 
                  << (std::to_string(r.cppStlExtractUs) + " us") << "\n";
        std::cout << "============================================================\n";
    }
};


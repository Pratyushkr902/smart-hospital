/**
 * @file DatabaseManager.hpp
 * @brief Production SQLite3 Relational Persistence Layer for Smart Hospital
 * 
 * Features:
 * - RAII: Automatic database connection handle management
 * - Parameterized Prepared Statements (Zero SQL Injection)
 * - Relational Schema: patients, triage_vitals, doctors, audit_logs
 * - Rehydration: On server startup, reloads pending patients from SQLite directly
 *   into the C++ Priority Queue (Max-Heap) and Hash Map.
 */

#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include "../cpp_oop/models/Patient.hpp"

class DatabaseManager {
private:
    sqlite3* db = nullptr;
    std::string dbPath;
    std::mutex dbMutex;

    void executeRaw(const std::string& sql) {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::string errorStr = errMsg ? errMsg : "Unknown SQL error";
            sqlite3_free(errMsg);
            throw std::runtime_error("SQLite DDL error: " + errorStr);
        }
    }

public:
    explicit DatabaseManager(std::string path = "hospital.db") : dbPath(std::move(path)) {
        int rc = sqlite3_open(dbPath.c_str(), &db);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Cannot open SQLite database at: " + dbPath);
        }
        initSchema();
    }

    ~DatabaseManager() {
        if (db) {
            sqlite3_close(db);
            db = nullptr;
        }
    }

    // Non-copyable (RAII resource)
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    void initSchema() {
        std::lock_guard<std::mutex> lock(dbMutex);
        const std::string ddl = R"(
            CREATE TABLE IF NOT EXISTS doctors (
                id TEXT PRIMARY KEY,
                name TEXT NOT NULL,
                department TEXT NOT NULL,
                room TEXT NOT NULL
            );

            CREATE TABLE IF NOT EXISTS patients (
                id TEXT PRIMARY KEY,
                token TEXT UNIQUE NOT NULL,
                name TEXT NOT NULL,
                age INTEGER NOT NULL,
                gender TEXT NOT NULL,
                department TEXT NOT NULL,
                doctor_id TEXT NOT NULL,
                status TEXT NOT NULL,
                urgency_score INTEGER NOT NULL,
                wait_minutes INTEGER NOT NULL,
                effective_score INTEGER NOT NULL,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP
            );

            CREATE TABLE IF NOT EXISTS triage_vitals (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                patient_token TEXT NOT NULL,
                heart_rate INTEGER,
                systolic_bp INTEGER,
                spo2 INTEGER,
                pain_level INTEGER,
                vital_score INTEGER,
                FOREIGN KEY (patient_token) REFERENCES patients(token) ON DELETE CASCADE
            );

            CREATE TABLE IF NOT EXISTS audit_logs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp TEXT NOT NULL,
                actor TEXT NOT NULL,
                action TEXT NOT NULL
            );
        )";
        executeRaw(ddl);
        seedDoctorsIfEmpty();
    }

    void seedDoctorsIfEmpty() {
        sqlite3_stmt* stmt = nullptr;
        const char* countSql = "SELECT COUNT(*) FROM doctors;";
        if (sqlite3_prepare_v2(db, countSql, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int count = sqlite3_column_int(stmt, 0);
                if (count == 0) {
                    executeRaw(R"(
                        INSERT INTO doctors (id, name, department, room) VALUES
                        ('D-101', 'Dr. A. Sharma', 'Emergency Medicine', 'Room 102'),
                        ('D-104', 'Dr. R. Iyer', 'General Medicine', 'Room 105'),
                        ('D-107', 'Dr. N. Fernandes', 'Orthopedics', 'Room 201');
                    )");
                }
            }
        }
        sqlite3_finalize(stmt);
    }

    bool savePatient(const Patient& p, const ClinicalVitals& v) {
        std::lock_guard<std::mutex> lock(dbMutex);
        executeRaw("BEGIN TRANSACTION;");

        // 1. Insert patient
        const char* sqlPatient = R"(
            INSERT INTO patients (id, token, name, age, gender, department, doctor_id, status, urgency_score, wait_minutes, effective_score)
            VALUES (?, ?, ?, ?, ?, ?, ?, 'waiting', ?, ?, ?);
        )";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sqlPatient, -1, &stmt, nullptr) != SQLITE_OK) {
            executeRaw("ROLLBACK;");
            return false;
        }

        sqlite3_bind_text(stmt, 1, p.getId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, p.getToken().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, p.getName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, p.getAge());
        sqlite3_bind_text(stmt, 5, "O", -1, SQLITE_TRANSIENT); // Gender
        sqlite3_bind_text(stmt, 6, p.getDepartment().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, p.getDoctorId().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 8, p.getUrgencyScore());
        sqlite3_bind_int(stmt, 9, p.getWaitMinutes());
        sqlite3_bind_int(stmt, 10, p.getEffectiveScore());

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            executeRaw("ROLLBACK;");
            return false;
        }
        sqlite3_finalize(stmt);

        // 2. Insert triage vitals
        const char* sqlVitals = R"(
            INSERT INTO triage_vitals (patient_token, heart_rate, systolic_bp, spo2, pain_level, vital_score)
            VALUES (?, ?, ?, ?, ?, ?);
        )";
        if (sqlite3_prepare_v2(db, sqlVitals, -1, &stmt, nullptr) != SQLITE_OK) {
            executeRaw("ROLLBACK;");
            return false;
        }

        sqlite3_bind_text(stmt, 1, p.getToken().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, v.heartRate);
        sqlite3_bind_int(stmt, 3, v.systolicBp);
        sqlite3_bind_int(stmt, 4, v.oxygenSaturation);
        sqlite3_bind_int(stmt, 5, v.painLevel);
        sqlite3_bind_int(stmt, 6, v.vitalScore);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            executeRaw("ROLLBACK;");
            return false;
        }
        sqlite3_finalize(stmt);

        executeRaw("COMMIT;");
        return true;
    }

    bool updatePatientStatus(const std::string& token, const std::string& status) {
        std::lock_guard<std::mutex> lock(dbMutex);
        const char* sql = "UPDATE patients SET status = ? WHERE token = ?;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

        sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, token.c_str(), -1, SQLITE_TRANSIENT);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool logAudit(const std::string& timestamp, const std::string& actor, const std::string& action) {
        std::lock_guard<std::mutex> lock(dbMutex);
        const char* sql = "INSERT INTO audit_logs (timestamp, actor, action) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

        sqlite3_bind_text(stmt, 1, timestamp.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, actor.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, action.c_str(), -1, SQLITE_TRANSIENT);

        bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    std::vector<Patient> loadWaitingPatients() {
        std::lock_guard<std::mutex> lock(dbMutex);
        std::vector<Patient> list;
        const char* sql = R"(
            SELECT p.id, p.token, p.name, p.age, p.gender, p.department, p.doctor_id, p.wait_minutes,
                   COALESCE(v.heart_rate, 80), COALESCE(v.systolic_bp, 120),
                   COALESCE(v.spo2, 98), COALESCE(v.pain_level, 5), COALESCE(v.vital_score, 15)
            FROM patients p
            LEFT JOIN triage_vitals v ON p.token = v.patient_token
            WHERE p.status = 'waiting'
            ORDER BY p.effective_score DESC;
        )";

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                std::string token = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
                int age = sqlite3_column_int(stmt, 3);
                std::string gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
                std::string dept = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
                std::string docId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
                int waitMins = sqlite3_column_int(stmt, 7);

                ClinicalVitals v;
                v.heartRate = sqlite3_column_int(stmt, 8);
                v.systolicBp = sqlite3_column_int(stmt, 9);
                v.oxygenSaturation = sqlite3_column_int(stmt, 10);
                v.painLevel = sqlite3_column_int(stmt, 11);
                v.vitalScore = sqlite3_column_int(stmt, 12);

                list.emplace_back(id, token, name, age, gender, dept, docId, v, std::vector<std::string>{}, waitMins);
            }
        }
        sqlite3_finalize(stmt);
        return list;
    }

    int getTotalPatientsCount() {
        std::lock_guard<std::mutex> lock(dbMutex);
        sqlite3_stmt* stmt = nullptr;
        int count = 0;
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM patients;", -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                count = sqlite3_column_int(stmt, 0);
            }
        }
        sqlite3_finalize(stmt);
        return count;
    }
};

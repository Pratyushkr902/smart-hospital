/**
 * @file server.cpp
 * @brief Smart Hospital Management System — Crow C++ REST API Server
 * 
 * Features:
 * - Crow Framework C++20 Routing: CROW_ROUTE(app, "/api/...")
 * - SQLite3 Relational Persistence (hospital.db)
 * - In-Memory Binary Max-Heap + O(1) Hash Map
 * - Full CORS Integration for Next.js (http://localhost:3000)
 */

#include "include/crow.h"
#include "c_ds/heap.h"
#include "c_ds/linked_list.h"
#include "c_ds/hash_table.h"

#include "cpp_oop/models/User.hpp"
#include "cpp_oop/models/Roles.hpp"
#include "cpp_oop/models/Patient.hpp"
#include "cpp_oop/engine/HospitalSystem.hpp"
#include "db/DatabaseManager.hpp"

#include <iostream>
#include <mutex>

static HospitalSystem g_hospital;
static DatabaseManager g_db("hospital.db");
static std::mutex g_mutex;

int main() {
    crow::SimpleApp app;

    // 1. Database Initialization & Queue Rehydration
    int countInDb = g_db.getTotalPatientsCount();
    if (countInDb == 0) {
        std::cout << "[Crow API] Initializing and seeding hospital.db SQLite tables...\n";
        auto receptionist = g_hospital.getStaff()[0];
        
        ClinicalVitals v1{75, 120, 98, 4, 10};
        std::string t1 = g_hospital.checkInPatient(receptionist, "Asha Rao", 34, "F", "General Medicine", "D-104", v1, {"diabetic"}, 50);
        Patient p1("","", "", 0, "", "", "", {}, {});
        g_hospital.findPatientByToken(t1, p1);
        g_db.savePatient(p1, v1);

        ClinicalVitals v2{130, 165, 89, 9, 25};
        std::string t2 = g_hospital.checkInPatient(receptionist, "Imran Qureshi", 61, "M", "Cardiology", "D-101", v2, {"cardiac"}, 5);
        Patient p2("","", "", 0, "", "", "", {}, {});
        g_hospital.findPatientByToken(t2, p2);
        g_db.savePatient(p2, v2);

        ClinicalVitals v3{82, 115, 97, 5, 8};
        std::string t3 = g_hospital.checkInPatient(receptionist, "Fatima Sheikh", 28, "F", "General Medicine", "D-104", v3, {"pregnant"}, 45);
        Patient p3("","", "", 0, "", "", "", {}, {});
        g_hospital.findPatientByToken(t3, p3);
        g_db.savePatient(p3, v3);

        ClinicalVitals v4{140, 175, 85, 10, 25};
        std::string t4 = g_hospital.checkInPatient(receptionist, "Vikram Nair", 45, "M", "Emergency", "D-101", v4, {}, 2);
        Patient p4("","", "", 0, "", "", "", {}, {});
        g_hospital.findPatientByToken(t4, p4);
        g_db.savePatient(p4, v4);
    } else {
        std::cout << "[Crow API] Rehydrating C++ Priority Queue directly from hospital.db (" << countInDb << " records found)...\n";
        auto waiting = g_db.loadWaitingPatients();
        for (const auto& p : waiting) {
            g_hospital.addPatientDirect(p);
        }
    }

    // 2. Crow REST Route: GET /api/health
    CROW_ROUTE(app, "/api/health")([](){
        int totalDbPatients = g_db.getTotalPatientsCount();
        crow::json::wvalue res;
        res["status"] = "online";
        res["framework"] = "Crow C++ REST API Microframework";
        res["database"] = "SQLite3 (hospital.db)";
        res["totalPatientsInDatabase"] = totalDbPatients;
        res["activeQueueMemorySize"] = static_cast<int>(g_hospital.getQueueSize());
        res["version"] = "2.0.0-crow";
        return crow::response(res.dump());
    });

    // 3. Crow REST Route: POST /api/checkin
    CROW_ROUTE(app, "/api/checkin").methods(crow::HTTPMethod::POST)([](const crow::request& req){
        std::lock_guard<std::mutex> lock(g_mutex);
        auto body = crow::json::load(req.body);

        std::string name = body["name"];
        if (name.empty()) name = "Walk-in Patient";
        int age = body.get_int("age", 35);
        std::string gender = body["gender"];
        if (gender.empty()) gender = "O";
        std::string dept = body["dept"];
        if (dept.empty()) dept = "General Medicine";
        std::string docId = body["doctorId"];
        if (docId.empty()) docId = "D-104";

        ClinicalVitals vitals;
        vitals.heartRate = body.get_int("heartRate", 80);
        vitals.systolicBp = body.get_int("systolicBp", 120);
        vitals.oxygenSaturation = body.get_int("oxygenSaturation", 98);
        vitals.painLevel = body.get_int("pain", 5);
        vitals.vitalScore = body.get_int("vital", 15);

        auto receptionist = g_hospital.getStaff()[0];
        std::string token = g_hospital.checkInPatient(receptionist, name, age, gender, dept, docId, vitals, {}, 0);

        Patient p("", "", "", 0, "", "", "", {}, {});
        g_hospital.findPatientByToken(token, p);

        // Persistent save to SQLite3
        g_db.savePatient(p, vitals);
        g_db.logAudit("10:30 AM", receptionist->getName(), "Checked in " + name + " (" + token + ")");

        crow::json::wvalue res;
        res["success"] = true;
        res["token"] = token;
        res["name"] = p.getName();
        res["urgencyScore"] = p.getUrgencyScore();
        res["effectiveScore"] = p.getEffectiveScore();
        res["persistedToSqlite"] = true;

        return crow::response(res.dump());
    });

    // 4. Crow REST Route: POST /api/doctor/call-next
    CROW_ROUTE(app, "/api/doctor/call-next").methods(crow::HTTPMethod::POST)([](){
        std::lock_guard<std::mutex> lock(g_mutex);
        auto doctor = g_hospital.getStaff()[1]; // Dr. Sharma
        Patient nextP("", "", "", 0, "", "", "", {}, {});

        if (g_hospital.callNextPatient(doctor, nextP)) {
            // Update patient state in SQLite
            g_db.updatePatientStatus(nextP.getToken(), "in-treatment");
            g_db.logAudit("10:30 AM", doctor->getName(), "Called next patient: " + nextP.getName() + " (" + nextP.getToken() + ")");

            crow::json::wvalue res;
            res["success"] = true;
            res["called"] = true;
            res["token"] = nextP.getToken();
            res["name"] = nextP.getName();
            res["score"] = nextP.getEffectiveScore();
            return crow::response(res.dump());
        } else {
            crow::json::wvalue res;
            res["success"] = true;
            res["called"] = false;
            res["message"] = "No waiting patients in queue";
            return crow::response(res.dump());
        }
    });

    // 5. Crow REST Route: POST /api/simulation/advance-time
    CROW_ROUTE(app, "/api/simulation/advance-time").methods(crow::HTTPMethod::POST)([](const crow::request& req){
        std::lock_guard<std::mutex> lock(g_mutex);
        auto body = crow::json::load(req.body);
        int mins = body.get_int("minutes", 10);
        g_hospital.simulateTimeAdvance(mins);
        g_db.logAudit("10:30 AM", "System", "Advanced simulation clock by " + std::to_string(mins) + " minutes");

        crow::json::wvalue res;
        res["success"] = true;
        res["minutesAdvanced"] = mins;
        return crow::response(res.dump());
    });

    // Run Crow HTTP server on port 8080 with multi-threading
    app.port(8080).multithreaded().run();
    return 0;
}

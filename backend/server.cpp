/**
 * @file server.cpp
 * @brief High-performance C++ REST API HTTP Server with SQLite3 Relational Persistence
 * 
 * Features:
 * - Native POSIX Socket HTTP Server (Zero external networking dependencies)
 * - SQLite3 Relational Persistence (hospital.db)
 * - In-Memory C++ Priority Queue (Max-Heap) + O(1) Hash Map
 * - Full CORS support for Next.js frontend (http://localhost:3000)
 */

#include "c_ds/heap.h"
#include "c_ds/linked_list.h"
#include "c_ds/hash_table.h"

#include "cpp_oop/models/User.hpp"
#include "cpp_oop/models/Roles.hpp"
#include "cpp_oop/models/Patient.hpp"
#include "cpp_oop/engine/HospitalSystem.hpp"
#include "db/DatabaseManager.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#define PORT 8080
#define BUFFER_SIZE 65536

static HospitalSystem g_hospital;
static DatabaseManager g_db("hospital.db");
static std::mutex g_mutex;

std::string buildCorsHeaders(int statusCode = 200, const std::string& contentType = "application/json") {
    std::string statusMsg = (statusCode == 200) ? "200 OK" :
                            (statusCode == 204) ? "204 No Content" :
                            (statusCode == 400) ? "400 Bad Request" :
                            (statusCode == 404) ? "404 Not Found" : "500 Internal Server Error";
    std::ostringstream ss;
    ss << "HTTP/1.1 " << statusMsg << "\r\n";
    ss << "Access-Control-Allow-Origin: *\r\n";
    ss << "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n";
    ss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
    ss << "Content-Type: " << contentType << "\r\n";
    ss << "Connection: close\r\n";
    return ss.str();
}

std::string handleOptions() {
    return buildCorsHeaders(204) + "\r\n";
}

std::string handleHealth() {
    int totalDbPatients = g_db.getTotalPatientsCount();
    std::ostringstream json;
    json << "{\"status\":\"online\","
         << "\"engine\":\"C++20 Priority Queue & Posix HTTP\","
         << "\"database\":\"SQLite3 (hospital.db)\","
         << "\"totalPatientsInDatabase\":" << totalDbPatients << ","
         << "\"activeQueueMemorySize\":" << g_hospital.getQueueSize() << ","
         << "\"version\":\"1.1.0\"}";

    std::string body = json.str();
    std::ostringstream response;
    response << buildCorsHeaders(200) << "Content-Length: " << body.length() << "\r\n\r\n" << body;
    return response.str();
}

std::string extractJsonString(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":\"";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    pos += search.length();
    size_t end = json.find("\"", pos);
    if (end == std::string::npos) return "";
    return json.substr(pos, end - pos);
}

int extractJsonInt(const std::string& json, const std::string& key, int defaultVal = 0) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return defaultVal;
    pos += search.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == '\"')) pos++;
    size_t end = pos;
    while (end < json.length() && (isdigit(json[end]) || json[end] == '-')) end++;
    if (pos == end) return defaultVal;
    try {
        return std::stoi(json.substr(pos, end - pos));
    } catch (...) {
        return defaultVal;
    }
}

std::string handleCheckin(const std::string& body) {
    std::lock_guard<std::mutex> lock(g_mutex);
    std::string name = extractJsonString(body, "name");
    if (name.empty()) name = "Walk-in Patient";
    int age = extractJsonInt(body, "age", 35);
    std::string gender = extractJsonString(body, "gender");
    if (gender.empty()) gender = "O";
    std::string dept = extractJsonString(body, "dept");
    if (dept.empty()) dept = "General Medicine";
    std::string docId = extractJsonString(body, "doctorId");
    if (docId.empty()) docId = "D-104";

    ClinicalVitals vitals;
    vitals.heartRate = extractJsonInt(body, "heartRate", 80);
    vitals.systolicBp = extractJsonInt(body, "systolicBp", 120);
    vitals.oxygenSaturation = extractJsonInt(body, "oxygenSaturation", 98);
    vitals.painLevel = extractJsonInt(body, "pain", 5);
    vitals.vitalScore = extractJsonInt(body, "vital", 15);

    auto receptionist = g_hospital.getStaff()[0];
    std::string token = g_hospital.checkInPatient(receptionist, name, age, gender, dept, docId, vitals, {}, 0);

    Patient p("", "", "", 0, "", "", "", {}, {});
    g_hospital.findPatientByToken(token, p);

    // Save to SQLite Relational Database
    g_db.savePatient(p, vitals);
    g_db.logAudit("10:30 AM", receptionist->getName(), "Checked in " + name + " (" + token + ")");

    std::ostringstream json;
    json << "{\"success\":true,\"token\":\"" << token << "\","
         << "\"name\":\"" << p.getName() << "\","
         << "\"urgencyScore\":" << p.getUrgencyScore() << ","
         << "\"effectiveScore\":" << p.getEffectiveScore() << ","
         << "\"persistedToSqlite\":true}";

    std::string resBody = json.str();
    std::ostringstream response;
    response << buildCorsHeaders(200) << "Content-Length: " << resBody.length() << "\r\n\r\n" << resBody;
    return response.str();
}

std::string handleCallNext() {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto doctor = g_hospital.getStaff()[1]; // Dr. Sharma
    Patient nextP("", "", "", 0, "", "", "", {}, {});

    if (g_hospital.callNextPatient(doctor, nextP)) {
        // Update status in SQLite Database
        g_db.updatePatientStatus(nextP.getToken(), "in-treatment");
        g_db.logAudit("10:30 AM", doctor->getName(), "Called next patient: " + nextP.getName() + " (" + nextP.getToken() + ")");

        std::ostringstream json;
        json << "{\"success\":true,\"called\":true,"
             << "\"token\":\"" << nextP.getToken() << "\","
             << "\"name\":\"" << nextP.getName() << "\","
             << "\"score\":" << nextP.getEffectiveScore() << "}";
        std::string resBody = json.str();
        std::ostringstream response;
        response << buildCorsHeaders(200) << "Content-Length: " << resBody.length() << "\r\n\r\n" << resBody;
        return response.str();
    } else {
        std::string resBody = "{\"success\":true,\"called\":false,\"message\":\"No waiting patients in queue\"}";
        std::ostringstream response;
        response << buildCorsHeaders(200) << "Content-Length: " << resBody.length() << "\r\n\r\n" << resBody;
        return response.str();
    }
}

std::string handleAdvanceTime(const std::string& body) {
    std::lock_guard<std::mutex> lock(g_mutex);
    int mins = extractJsonInt(body, "minutes", 10);
    g_hospital.simulateTimeAdvance(mins);
    g_db.logAudit("10:30 AM", "System", "Advanced simulation clock by " + std::to_string(mins) + " minutes");

    std::string resBody = "{\"success\":true,\"minutesAdvanced\":" + std::to_string(mins) + "}";
    std::ostringstream response;
    response << buildCorsHeaders(200) << "Content-Length: " << resBody.length() << "\r\n\r\n" << resBody;
    return response.str();
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0) {
        close(clientSocket);
        return;
    }

    std::string request(buffer, bytesRead);
    std::istringstream reqStream(request);
    std::string method, path, httpVer;
    reqStream >> method >> path >> httpVer;

    std::string body;
    size_t bodyPos = request.find("\r\n\r\n");
    if (bodyPos != std::string::npos) {
        body = request.substr(bodyPos + 4);
    }

    std::string response;
    if (method == "OPTIONS") {
        response = handleOptions();
    } else if (method == "GET" && (path == "/api/health" || path == "/health")) {
        response = handleHealth();
    } else if (method == "POST" && path == "/api/checkin") {
        response = handleCheckin(body);
    } else if (method == "POST" && path == "/api/doctor/call-next") {
        response = handleCallNext();
    } else if (method == "POST" && path == "/api/simulation/advance-time") {
        response = handleAdvanceTime(body);
    } else {
        std::string notFound = R"({"error":"Endpoint not found","path":")" + path + R"("})";
        std::ostringstream ss;
        ss << buildCorsHeaders(404) << "Content-Length: " << notFound.length() << "\r\n\r\n" << notFound;
        response = ss.str();
    }

    send(clientSocket, response.c_str(), response.length(), 0);
    close(clientSocket);
}

int main() {
    int serverFd;
    struct sockaddr_in address;
    int opt = 1;

    // Database Initialization & Rehydration into C++ Priority Queue
    int countInDb = g_db.getTotalPatientsCount();
    if (countInDb == 0) {
        // Seed initial patients into both C++ engine and SQLite
        std::cout << "[Database] Initializing and seeding hospital.db SQLite tables...\n";
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
        std::cout << "[Database] Rehydrating C++ Priority Queue directly from hospital.db (" << countInDb << " records found)...\n";
        auto waiting = g_db.loadWaitingPatients();
        for (const auto& p : waiting) {
            g_hospital.addPatientDirect(p);
        }
    }

    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        return 1;
    }

    if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(serverFd, 16) < 0) {
        perror("Listen failed");
        return 1;
    }

    std::cout << "\n============================================================\n";
    std::cout << "  🏥 SMART HOSPITAL C++ REST API SERVER ONLINE\n";
    std::cout << "  Database: SQLite3 (hospital.db) [Tables: patients, vitals, logs]\n";
    std::cout << "  Listening on: http://localhost:" << PORT << "\n";
    std::cout << "  CORS Enabled for Next.js (http://localhost:3000)\n";
    std::cout << "============================================================\n";

    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            continue;
        }
        std::thread(handleClient, clientSocket).detach();
    }

    close(serverFd);
    return 0;
}

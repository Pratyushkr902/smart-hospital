/**
 * @file server.cpp
 * @brief High-performance C++ REST API HTTP Server for Smart Hospital
 * 
 * Features:
 * - Native POSIX Socket HTTP Server (Zero third-party library dependencies)
 * - Full CORS support for Next.js frontend (http://localhost:3000)
 * - REST Endpoints:
 *     GET  /api/health            -> Server health status
 *     GET  /api/queue             -> Current prioritized queue
 *     POST /api/checkin           -> Check in new patient with triage vitals
 *     POST /api/call-next         -> Doctor calls next critical patient (Max-Heap)
 *     POST /api/complete-consult  -> Doctor finishes consultation
 *     POST /api/advance-time      -> Advance time (aging/starvation bonus)
 *     GET  /api/audit-log         -> LIFO audit trail stack
 */

#include "c_ds/heap.h"
#include "c_ds/linked_list.h"
#include "c_ds/hash_table.h"

#include "cpp_oop/models/User.hpp"
#include "cpp_oop/models/Roles.hpp"
#include "cpp_oop/models/Patient.hpp"
#include "cpp_oop/engine/HospitalSystem.hpp"

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
static std::mutex g_mutex;

// Simple JSON response builders
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
    std::string body = R"({"status":"online","engine":"C++20 Priority Queue & Posix HTTP","version":"1.0.0"})";
    std::ostringstream response;
    response << buildCorsHeaders(200) << "Content-Length: " << body.length() << "\r\n\r\n" << body;
    return response.str();
}

// Simple JSON extraction helper for string values: "key":"value" or "key":123
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

    std::ostringstream json;
    json << "{\"success\":true,\"token\":\"" << token << "\","
         << "\"name\":\"" << p.getName() << "\","
         << "\"urgencyScore\":" << p.getUrgencyScore() << ","
         << "\"effectiveScore\":" << p.getEffectiveScore() << "}";

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

    // Extract body if POST
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

    // Seed initial patients in C++ Priority Queue
    {
        auto receptionist = g_hospital.getStaff()[0];
        g_hospital.checkInPatient(receptionist, "Asha Rao", 34, "F", "General Medicine", "D-104", {75, 120, 98, 4, 10}, {"diabetic"}, 50);
        g_hospital.checkInPatient(receptionist, "Imran Qureshi", 61, "M", "Cardiology", "D-101", {130, 165, 89, 9, 25}, {"cardiac"}, 5);
        g_hospital.checkInPatient(receptionist, "Fatima Sheikh", 28, "F", "General Medicine", "D-104", {82, 115, 97, 5, 8}, {"pregnant"}, 45);
        g_hospital.checkInPatient(receptionist, "Vikram Nair", 45, "M", "Emergency", "D-101", {140, 175, 85, 10, 25}, {}, 2);
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

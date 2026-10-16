/**
 * @file crow.h
 * @brief Crow C++ Microframework Header for Smart Hospital REST API
 * 
 * Provides Crow C++ routing, request/response models, JSON serialization,
 * and high-performance multi-threaded POSIX runtime without requiring external Boost.
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <iostream>
#include <memory>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>

namespace crow {

    enum class HTTPMethod {
        GET,
        POST,
        PUT,
        DELETE,
        OPTIONS
    };

    struct request {
        std::string raw_url;
        std::string url;
        std::string method_str;
        HTTPMethod method = HTTPMethod::GET;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    struct response {
        int code = 200;
        std::string content_type = "application/json";
        std::string body;

        response() = default;
        response(int statusCode) : code(statusCode) {}
        response(const std::string& b) : code(200), body(b) {}
        response(int statusCode, const std::string& b) : code(statusCode), body(b) {}

        void set_header(const std::string&, const std::string&) {}
    };

    namespace json {
        struct wvalue {
            std::ostringstream buffer;
            bool first = true;

            wvalue() {
                buffer << "{";
            }

            void add_field(const std::string& key, const std::string& val, bool isStr = true) {
                if (!first) buffer << ",";
                buffer << "\"" << key << "\":";
                if (isStr) buffer << "\"" << val << "\"";
                else buffer << val;
                first = false;
            }

            // Setter helpers
            struct FieldProxy {
                wvalue* parent;
                std::string key;

                void operator=(const std::string& val) { parent->add_field(key, val, true); }
                void operator=(const char* val) { parent->add_field(key, std::string(val), true); }
                void operator=(int val) { parent->add_field(key, std::to_string(val), false); }
                void operator=(double val) { parent->add_field(key, std::to_string(val), false); }
                void operator=(bool val) { parent->add_field(key, val ? "true" : "false", false); }
            };

            FieldProxy operator[](const std::string& key) {
                return FieldProxy{this, key};
            }

            std::string dump() const {
                return buffer.str() + "}";
            }

            operator response() const {
                return response(200, dump());
            }
        };

        struct rvalue {
            std::string raw;

            std::string operator[](const std::string& key) const {
                std::string search = "\"" + key + "\":\"";
                size_t pos = raw.find(search);
                if (pos == std::string::npos) return "";
                pos += search.length();
                size_t end = raw.find("\"", pos);
                if (end == std::string::npos) return "";
                return raw.substr(pos, end - pos);
            }

            int get_int(const std::string& key, int defaultVal = 0) const {
                std::string search = "\"" + key + "\":";
                size_t pos = raw.find(search);
                if (pos == std::string::npos) return defaultVal;
                pos += search.length();
                while (pos < raw.length() && (raw[pos] == ' ' || raw[pos] == '\"')) pos++;
                size_t end = pos;
                while (end < raw.length() && (isdigit(raw[end]) || raw[end] == '-')) end++;
                if (pos == end) return defaultVal;
                try {
                    return std::stoi(raw.substr(pos, end - pos));
                } catch (...) {
                    return defaultVal;
                }
            }
        };

        inline rvalue load(const std::string& str) {
            return rvalue{str};
        }
    }

    class RouteHandler {
    public:
        using Callback = std::function<response(const request&)>;
        std::string path;
        std::vector<HTTPMethod> allowed_methods;
        Callback handler;

        RouteHandler(std::string p) : path(std::move(p)) {
            allowed_methods.push_back(HTTPMethod::GET); // default
        }

        RouteHandler& methods(HTTPMethod m1) {
            allowed_methods.clear();
            allowed_methods.push_back(m1);
            return *this;
        }

        RouteHandler& methods(HTTPMethod m1, HTTPMethod m2) {
            allowed_methods.clear();
            allowed_methods.push_back(m1);
            allowed_methods.push_back(m2);
            return *this;
        }

        template <typename Func>
        void operator()(Func f) {
            if constexpr (std::is_invocable_v<Func, const request&>) {
                handler = [f](const request& req) -> response {
                    return f(req);
                };
            } else {
                handler = [f](const request&) -> response {
                    return f();
                };
            }
        }
    };

    class SimpleApp {
    private:
        int portNumber = 8080;
        bool isMultiThreaded = true;
        std::vector<std::shared_ptr<RouteHandler>> routes;

    public:
        SimpleApp() = default;

        std::shared_ptr<RouteHandler> new_route(const std::string& path) {
            auto r = std::make_shared<RouteHandler>(path);
            routes.push_back(r);
            return r;
        }

        SimpleApp& port(int p) {
            portNumber = p;
            return *this;
        }

        SimpleApp& multithreaded() {
            isMultiThreaded = true;
            return *this;
        }

        void run() {
            int serverFd;
            struct sockaddr_in address;
            int opt = 1;

            if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("Socket failed");
                return;
            }

            if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
                perror("setsockopt failed");
                return;
            }

            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(portNumber);

            if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
                perror("Bind failed");
                return;
            }

            if (listen(serverFd, 16) < 0) {
                perror("Listen failed");
                return;
            }

            std::cout << "\n============================================================\n";
            std::cout << "  🏥 SMART HOSPITAL C++ CROW REST API SERVER\n";
            std::cout << "  Framework: Crow Microframework (C++20)\n";
            std::cout << "  Listening on: http://localhost:" << portNumber << "\n";
            std::cout << "  CORS Enabled for Next.js (http://localhost:3000)\n";
            std::cout << "============================================================\n";

            while (true) {
                struct sockaddr_in clientAddr;
                socklen_t addrLen = sizeof(clientAddr);
                int clientSocket = accept(serverFd, (struct sockaddr*)&clientAddr, &addrLen);
                if (clientSocket < 0) continue;

                if (isMultiThreaded) {
                    std::thread([this, clientSocket]() {
                        this->handle_connection(clientSocket);
                    }).detach();
                } else {
                    handle_connection(clientSocket);
                }
            }

            close(serverFd);
        }

    private:
        void handle_connection(int clientSocket) {
            char buffer[65536];
            memset(buffer, 0, sizeof(buffer));

            ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
            if (bytesRead <= 0) {
                close(clientSocket);
                return;
            }

            std::string rawReq(buffer, bytesRead);
            std::istringstream reqStream(rawReq);
            std::string methodStr, pathStr, httpVer;
            reqStream >> methodStr >> pathStr >> httpVer;

            request req;
            req.url = pathStr;
            req.raw_url = pathStr;
            req.method_str = methodStr;
            if (methodStr == "GET") req.method = HTTPMethod::GET;
            else if (methodStr == "POST") req.method = HTTPMethod::POST;
            else if (methodStr == "OPTIONS") req.method = HTTPMethod::OPTIONS;

            size_t bodyPos = rawReq.find("\r\n\r\n");
            if (bodyPos != std::string::npos) {
                req.body = rawReq.substr(bodyPos + 4);
            }

            // Automatic CORS preflight
            if (req.method == HTTPMethod::OPTIONS) {
                std::string cors = "HTTP/1.1 204 No Content\r\n"
                                   "Access-Control-Allow-Origin: *\r\n"
                                   "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n"
                                   "Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
                                   "Connection: close\r\n\r\n";
                send(clientSocket, cors.c_str(), cors.length(), 0);
                close(clientSocket);
                return;
            }

            response res(404, R"({"error":"Endpoint not found"})");

            for (const auto& route : routes) {
                if (route->path == req.url) {
                    bool methodAllowed = false;
                    for (auto m : route->allowed_methods) {
                        if (m == req.method) {
                            methodAllowed = true;
                            break;
                        }
                    }
                    if (methodAllowed && route->handler) {
                        res = route->handler(req);
                        break;
                    }
                }
            }

            std::string statusMsg = (res.code == 200) ? "200 OK" : (res.code == 404) ? "404 Not Found" : "400 Bad Request";
            std::ostringstream ss;
            ss << "HTTP/1.1 " << statusMsg << "\r\n";
            ss << "Access-Control-Allow-Origin: *\r\n";
            ss << "Access-Control-Allow-Methods: GET, POST, OPTIONS, PUT, DELETE\r\n";
            ss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
            ss << "Content-Type: " << res.content_type << "\r\n";
            ss << "Content-Length: " << res.body.length() << "\r\n";
            ss << "Connection: close\r\n\r\n";
            ss << res.body;

            std::string out = ss.str();
            send(clientSocket, out.c_str(), out.length(), 0);
            close(clientSocket);
        }
    };

} // namespace crow

// Crow standard route macro
#define CROW_ROUTE(app, url) (*(app.new_route(url)))

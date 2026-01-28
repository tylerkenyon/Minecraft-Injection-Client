#include "Logger.h"
#include <sstream>
#include <iomanip>
#include <Windows.h>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string exePath(path);
    std::string logPath = exePath.substr(0, exePath.find_last_of("\\/")) + "\\minecraft_client.log";
    
    logFile.open(logPath, std::ios::app);
    if (logFile.is_open()) {
        log(INFO, "=== Logger initialized ===");
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        log(INFO, "=== Logger shutting down ===");
        logFile.close();
    }
}

void Logger::log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    std::string logMessage = "[" + getCurrentTime() + "] [" + levelToString(level) + "] " + message;
    
    if (logFile.is_open()) {
        logFile << logMessage << std::endl;
        logFile.flush();
    }
    
    // Also output to console if attached
    std::cout << logMessage << std::endl;
}

void Logger::info(const std::string& message) {
    log(INFO, message);
}

void Logger::warning(const std::string& message) {
    log(WARNING, message);
}

void Logger::error(const std::string& message) {
    log(ERROR, message);
}

void Logger::debug(const std::string& message) {
    log(DEBUG, message);
}

std::string Logger::getCurrentTime() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
}

std::string Logger::levelToString(Level level) {
    switch (level) {
        case INFO: return "INFO";
        case WARNING: return "WARN";
        case ERROR: return "ERROR";
        case DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

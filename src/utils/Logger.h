#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <mutex>
#include <ctime>

class Logger {
public:
    enum Level {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static Logger& getInstance();
    
    void log(Level level, const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

private:
    Logger();
    ~Logger();
    
    std::ofstream logFile;
    std::mutex logMutex;
    
    std::string getCurrentTime();
    std::string levelToString(Level level);
};

#define LOG_INFO(msg) Logger::getInstance().info(msg)
#define LOG_WARNING(msg) Logger::getInstance().warning(msg)
#define LOG_ERROR(msg) Logger::getInstance().error(msg)
#define LOG_DEBUG(msg) Logger::getInstance().debug(msg)

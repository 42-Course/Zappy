#include "services/Logger.hpp"
#include <filesystem>
#include <iostream>
#include <sys/stat.h>

namespace Zappy {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    // Create logs directory if it doesn't exist
    std::filesystem::create_directories("logs");
    
    // Open log files in append mode
    debugFile.open(DEBUG_LOG_FILE, std::ios::app);
    errorFile.open(ERROR_LOG_FILE, std::ios::app);
    
    if (!debugFile.is_open() || !errorFile.is_open()) {
        throw std::runtime_error("Failed to open log files");
    }
}

Logger::~Logger() {
    if (debugFile.is_open()) debugFile.close();
    if (errorFile.is_open()) errorFile.close();
}

void Logger::log(LogLevel level, const std::string& message) {
    std::string levelStr;
    bool isError = false;
    
    switch (level) {
        case LogLevel::DEBUG:
            levelStr = "DEBUG";
            break;
        case LogLevel::INFO:
            levelStr = "INFO";
            break;
        case LogLevel::WARNING:
            levelStr = "WARNING";
            isError = true;
            break;
        case LogLevel::ERROR:
            levelStr = "ERROR";
            isError = true;
            break;
        case LogLevel::FATAL:
            levelStr = "FATAL";
            isError = true;
            break;
    }
    
    std::stringstream ss;
    ss << getCurrentTimestamp() << " [" << levelStr << "] " << message << std::endl;
    writeToFile(ss.str(), isError);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    
    std::stringstream ss;
    ss << buffer << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::writeToFile(const std::string& message, bool isError) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    checkRotateFiles();
    
    if (isError) {
        errorFile << message;
        errorFile.flush();
    }
    
    debugFile << message;
    debugFile.flush();
}

void Logger::checkRotateFiles() {
    auto checkAndRotate = [](std::ofstream& file, const char* filename) {
        struct stat st;
        if (stat(filename, &st) == 0) {
            if ((size_t)st.st_size > MAX_FILE_SIZE) {
                // Close current file
                file.close();
                
                // Create backup filename with timestamp
                std::string backupName = std::string(filename) + "." + 
                    std::to_string(std::time(nullptr));
                
                // Rename current file to backup
                std::rename(filename, backupName.c_str());
                
                // Open new file
                file.open(filename, std::ios::app);
                if (!file.is_open()) {
                    throw std::runtime_error("Failed to rotate log file");
                }
            }
        }
    };
    
    checkAndRotate(debugFile, DEBUG_LOG_FILE);
    checkAndRotate(errorFile, ERROR_LOG_FILE);
}

} 
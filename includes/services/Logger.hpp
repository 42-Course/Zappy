#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <fstream>
#include <string>
#include <mutex>
#include <memory>
#include <sstream>
#include <ctime>

namespace Zappy {
    enum class LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    class Logger {
    public:
        static Logger& getInstance();
        
        void log(LogLevel level, const std::string& message);
        void debug(const std::string& message);
        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);
        void fatal(const std::string& message);

        // Delete copy constructor and assignment operator
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

    private:
        Logger();
        ~Logger();

        void writeToFile(const std::string& message, bool isError);
        std::string getCurrentTimestamp() const;
        void checkRotateFiles();

        std::ofstream debugFile;
        std::ofstream errorFile;
        std::mutex logMutex;
        
        static constexpr const char* DEBUG_LOG_FILE = "logs/debug.log";
        static constexpr const char* ERROR_LOG_FILE = "logs/error.log";
        static constexpr size_t MAX_FILE_SIZE = 50 * 1024 * 1024; // 50MB
    };
}

#endif // LOGGER_HPP 
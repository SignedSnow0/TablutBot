#include "Logger.h"

#include <iomanip>
static Logger *sLogger{nullptr};

#ifndef NO_LOG
void Logger::Init(const std::string &filePath) {
    sLogger = new Logger();
    sLogger->mFile = std::ofstream(filePath, std::ios::out);
}

void Logger::Shutdown() {
    sLogger->mFile.close();
    delete sLogger;
}

void Logger::Log(LogLevel level, const char *file, int line,
                 const std::string &message) {
    std::string levelStr;
    switch (level) {
    case LogLevel::DEBUG:
        levelStr = "DEBUG";
        break;
    case LogLevel::INFO:
        levelStr = "INFO";
        break;
    case LogLevel::WARNING:
        levelStr = "WARNING";
        break;
    case LogLevel::ERROR:
        levelStr = "ERROR";
        break;
    default:
        levelStr = "UNKNOWN";
        break;
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    sLogger->mFile << "[" << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << "] ["
                   << levelStr << "] (" << file << ":" << line << ") "
                   << message << std::endl;
}
#else
void Logger::Init(const std::string &filePath) {}

void Logger::Shutdown() {}

void Logger::Log(LogLevel level, const char *file, int line,
                 const std::string &message) {}
#endif // !NO_LOG

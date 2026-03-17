#pragma once
#include <format>
#include <fstream>
#include <string>

#ifndef NO_LOG
/*
 * Macro used to log a message with DEBUG level.
 *
 * This macro captures the file name and line number where it is invoked,
 * and formats the message using std::format.
 */
#ifdef DEBUG
#define LOG_DEBUG(...)                                                         \
    ::Logger::Log(::LogLevel::DEBUG, __FILE__, __LINE__,                       \
                  std::format(__VA_ARGS__))
#else
#define LOG_DEBUG(...)
#endif // DEBUG

/*
 * Macro used to log a message with INFO level.
 *
 * This macro captures the file name and line number where it is invoked,
 * and formats the message using std::format.
 */
#define LOG_INFO(...)                                                          \
    ::Logger::Log(::LogLevel::INFO, __FILE__, __LINE__,                        \
                  std::format(__VA_ARGS__))
/*
 * Macro used to log a message with WARNING level.
 *
 * This macro captures the file name and line number where it is invoked,
 * and formats the message using std::format.
 */
#define LOG_WARNING(...)                                                       \
    ::Logger::Log(::LogLevel::WARNING, __FILE__, __LINE__,                     \
                  std::format(__VA_ARGS__))
/*
 * Macro used to log a message with ERROR level.
 *
 * This macro captures the file name and line number where it is invoked,
 * and formats the message using std::format.
 */
#define LOG_ERROR(...)                                                         \
    ::Logger::Log(::LogLevel::ERROR, __FILE__, __LINE__,                       \
                  std::format(__VA_ARGS__))
#else
#define LOG_DEBUG(...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#endif

/*
 * Enumeration of log levels.
 */
enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

/*
 * Logger class for logging messages to a file with different severity
 * levels.
 *
 * This class implements a singleton pattern to ensure a single instance,
 * which can be accessed globally.
 * The user can choose the output log file by calling the Create method before
 * any log operations, by default it logs to "vulkan-app.log".
 */
class Logger {
public:
    Logger(const Logger &) = delete;
    Logger operator=(const Logger &) = delete;

    static void Init(const std::string &filePath);
    static void Shutdown();

    /*
     * Logs a message with the specified log level, file name, and line
     * number.
     *
     * @param level The log level of the message.
     * @param file The name of the source file where the log is generated.
     * @param line The line number in the source file where the log is
     * generated.
     * @param message The log message.
     */
    static void Log(LogLevel level, const char *file, int line,
                    const std::string &message);

private:
    Logger() = default;

    std::ofstream mFile;
};

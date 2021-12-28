#ifndef LOG_HH
#define LOG_HH

#include <iostream>
#include <string>

#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cstring>

using std::string;

namespace logger {

class Logger {
public:
    static Logger & get();
    void append(string header, string content, string time = "");
    
private:
    Logger();
    ~Logger();

    bool _enable;
};

inline string va_list_to_string(const char *format, ...)
{
    char buf[4096];

    va_list list;
    va_start(list, format);
    vsnprintf(buf, 4096, format, list);
    va_end(list);

    return string(buf);
}

inline string GetCurrentTime() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return string(buffer);
}

#define log_trace(...)                                                         \
logger::Logger::get().append(" TRACE ",                                        \
logger::va_list_to_string(__VA_ARGS__)                                         \
        .append(" ")                                                           \
        .append(std::string(__FILE__))                                         \
        .append("-")                                                           \
        .append(std::to_string(__LINE__)))
#define log_debug(...)                                                         \
    logger::Logger::get().append(" DEBUG ",                                    \
      logger::va_list_to_string(__VA_ARGS__)                                   \
        .append(" ")                                                           \
        .append(std::string(__FILE__))                                         \
        .append("-")                                                           \
        .append(std::to_string(__LINE__)))
#define log_warn(...)                                                          \
    logger::Logger::get().append(" WARN  ",                                    \
      logger::va_list_to_string(__VA_ARGS__)                                   \
        .append(" ")                                                           \
        .append(std::string(__FILE__))                                         \
        .append("-")                                                           \
        .append(std::to_string(__LINE__)))
#define log_info(...)                                                          \
    logger::Logger::get().append(" INFO  ",                                    \
      logger::va_list_to_string(__VA_ARGS__)                                   \
        .append(" ")                                                           \
        .append(std::string(__FILE__))                                         \
        .append("-")                                                           \
        .append(std::to_string(__LINE__)))
#define log_error(...)                                                         \
    logger::Logger::get().append(" ERROR ",                                    \
      logger::va_list_to_string(__VA_ARGS__)                                   \
        .append(" ")                                                           \
        .append(std::string(__FILE__))                                         \
        .append("-")                                                           \
        .append(std::to_string(__LINE__)))
#define log_fatal(...)                                                         \
    logger::Logger::get().append(" FATAL ",                                    \
      logger::va_list_to_string(__VA_ARGS__)                                   \
        .append(" ")                                                           \
        .append(std::string(__FILE__))                                         \
        .append("-")                                                           \
        .append(std::to_string(__LINE__)))
}
#endif
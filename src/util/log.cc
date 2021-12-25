#include "log.hh"

using namespace logger;

Logger::Logger() {

}

Logger::~Logger() {

}

void Logger::append(string header, string content, string time) {
    string str;

    if (time == "") {
        time = GetCurrentTime();
    }

    str.append(time).append(" |").append(header).append("| ").append(content).append("\n");
    fprintf(stdout, "%s", str.c_str());
}

Logger & Logger::get() {
    static Logger logger;
    return logger;
}

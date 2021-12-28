#include "log.hh"

using namespace logger;

Logger::Logger():_enable(false) {
    char *myEnv = getenv("GG_LOG");
    if (myEnv != NULL) {
        if (strcmp(myEnv, "1") == 0) {
            this->_enable = true;
            printf("gg log -> [enabled]\n");
        }
    } else {
        printf("gg log -> [disabled]\n");
    }
}

Logger::~Logger() {

}

void Logger::append(string header, string content, string time) {
    string str;

    if (this->_enable) {
        if (time == "") {
            time = GetCurrentTime();
        }

        str.append(time).append(" |").append(header).append("| ").append(content).append("\n");
        fprintf(stdout, "%s", str.c_str());
    }
}

Logger & Logger::get() {
    static Logger logger;
    return logger;
}

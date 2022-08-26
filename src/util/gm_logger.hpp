#pragma once

#include <ctime>
#include <string>

namespace game {
    enum LOG_TYPES {
        LOG_INFO,
        LOG_MSG,
        LOG_WARN,
        LOG_ERR,
        LOG_FATAL
    };
    class Logger {
        public:
            static constexpr const char* LOG_TYPE_STRINGS[] = {
                "INFO",
                "MSG",
                "WARN",
                "ERR",
                "FATAL"
            };

            static void init(const std::string& logPath, const std::string& crashPath);
            static void setPaths(const std::string& logPath, const std::string& crashPath);
            static void logMsg(const int logType, const std::string& message);
            [[noreturn]] static void crash(const std::string& message);

            static std::string logPath;
            static std::string crashPath;
    };
}

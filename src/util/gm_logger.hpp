#pragma once

#include <ctime>
#include <thread>
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

            // Functions
            static void init(const std::string& logPath, const std::string& crashPath);
            static void setPaths(const std::string& logPath, const std::string& crashPath);
            static void log(const int logType, const std::string& message);
            static void logSync(const int logType, const std::string& message, const std::thread::id& threadId);
            [[noreturn]] static void crash(const std::string& message);

        private:
            // Variables
            static std::string logPath_;
            static std::string crashPath_;
    };
}

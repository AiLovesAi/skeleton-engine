#pragma once

#include "../string/gm_utf8.hpp"

#include <cstring>
#include <thread>

namespace game {
    enum LOG_TYPES {
        LOG_INFO,
        LOG_MSG,
        LOG_WARN,
        LOG_ERR,
        LOG_FATAL,
    };

    class Logger {
        public:
            static constexpr const char* LOG_TYPE_STRINGS[] = {
                "INFO",
                "MSG",
                "WARN",
                "ERR",
                "FATAL",
            };

            // Functions
            static void init(const UTF8Str& logPath, const UTF8Str& crashPath);
            static void setPaths(const UTF8Str& logPath, const UTF8Str& crashPath);
            static inline void log(const int logType, const char*__restrict__ message) {
                log(logType, UTF8Str{static_cast<int64_t>(std::strlen(message)), std::shared_ptr<const char>(message, [](const char*){})});
            }
            static void log(const int logType, const UTF8Str& message);

            static inline void logSync(const int logType, const char*__restrict__ message, const std::thread::id& threadId) {
                logSync(logType, UTF8Str{static_cast<int64_t>(std::strlen(message)), std::shared_ptr<const char>(message, [](const char*){})}, threadId);
            }
            static inline void logSync(const int logType, const UTF8Str& message, const std::thread::id& threadId) {
                logSync_(logType, message, threadId);
            }
            
            [[noreturn]] static inline void crash(const char*__restrict__ message) {
                crash(UTF8Str{static_cast<int64_t>(std::strlen(message)), std::shared_ptr<const char>(message, [](const char*){})});
            }
            [[noreturn]] static void crash(const UTF8Str& message);

        private:
            // Functions
            static void logSync_(const int logType, const UTF8Str& message, const std::thread::id& threadId);
            
            // Variables
            static UTF8Str _logPath;
            static UTF8Str _crashPath;
    };
}

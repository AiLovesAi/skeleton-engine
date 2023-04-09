#include "gm_logger.hpp"

#include "gm_file.hpp"
#include "../../gm_core.hpp"
#include "../../headers/string.hpp"
#include "../../system/gm_system.hpp"
#include "../../system/gm_threads.hpp"

#include <atomic>
#include <csignal>
#include <cstdio>
#include <future>
#include <ctime>
#include <chrono>
#include <mutex>
#include <random>
#include <sys/time.h>

namespace game {
    static std::mutex mtx_;
    static std::atomic<bool> crashed_ = false;
    UTF8Str Logger::_logPath = UTF8Str{"latest.log", sizeof("latest.log") - 1};
    UTF8Str Logger::_crashPath = UTF8Str{"crash.log", sizeof("crash.log") - 1};
    void signalHandler(int signum);

    void Logger::init(const UTF8Str& logPath, const UTF8Str& crashPath) {
        setPaths(logPath, crashPath);
        
        // Set signal handlers
        std::signal(SIGINT, signalHandler);
        std::signal(SIGILL, signalHandler);
        std::signal(SIGSEGV, signalHandler);
        std::signal(SIGFPE, signalHandler);
        std::signal(SIGABRT, signalHandler);
        std::signal(SIGTERM, signalHandler);
    }

    void Logger::setPaths(const UTF8Str& logPath, const UTF8Str& crashPath) {
        mtx_.lock();

        int64_t logPathLen = File::executableDir().length() + logPath.length();
        char* logPathStr = static_cast<char*>(std::malloc(logPathLen + 1));
        std::memcpy(logPathStr, File::executableDir().get(), File::executableDir().length());
        std::memcpy(logPathStr + File::executableDir().length(), logPath.get(), logPath.length());
        logPathStr[logPathLen] = '\0';

        int64_t crashPathLen = File::executableDir().length() + crashPath.length();
        char* crashPathStr = static_cast<char*>(std::malloc(crashPathLen + 1));
        std::memcpy(crashPathStr, File::executableDir().get(), File::executableDir().length());
        std::memcpy(crashPathStr + File::executableDir().length(), crashPath.get(), crashPath.length());
        crashPathStr[crashPathLen] = '\0';
        
        Logger::_logPath = UTF8Str{logPathLen, std::shared_ptr<const char>(logPathStr, std::free)};
        Logger::_crashPath = UTF8Str{crashPathLen, std::shared_ptr<const char>(crashPathStr, std::free)};
        File::ensureParentDir(Logger::_logPath);
        File::ensureParentDir(Logger::_crashPath);

        FILE* file = std::fopen(_logPath.get(), "wb");
        std::fclose(file);

        mtx_.unlock();
    }

    void signalHandler(int signum) {
        switch (signum) {
            case SIGINT:
                Logger::crash("(SIGINT) Received interrupt signal.");
                break;
            case SIGILL:
                Logger::crash("(SIGILL) Illegal processor instruction.");
                break;
            case SIGFPE:
                Logger::crash("(SIGFPE) Floating point exception caused by overflow/underflow or division by zero.");
                break;
            case SIGSEGV:
                Logger::crash("(SIGSEGV) Attempted to read/write memory whose address was not allocated or accessible.");
                break;
            case SIGABRT:
                Logger::crash("(SIGABRT) Abort signal was raised.");
                break;
            case SIGTERM:
                Logger::log(LOG_INFO, "(SIGTERM) Received termination signal. Closing.");
                exit(EXIT_SUCCESS);
                break;
            default:
            Logger::crash("Unknown signal was raised.");
            break;
        }
    }

    void Logger::log(const int logType, const UTF8Str& message) {
        auto task = std::async(std::launch::async, logSync_, logType, message, std::this_thread::get_id());
    }

    void Logger::logSync_(const int logType, const UTF8Str& message, const std::thread::id& threadId) {
        // Get time
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        time_t time = static_cast<time_t>(tv.tv_sec);
        std::tm* now = std::localtime(&time);

        // Set log message
        // [HH::MM:SS+UUU] [THREAD/TYPE]: MESSAGE
        UTF8Str msg = FormatString::formatString("[%02d:%02d:%02d+%06u] [%s/%s]: %s\n",
            now->tm_hour, now->tm_min, now->tm_sec, tv.tv_usec, // Time
            Threads::threadName(threadId).get(), LOG_TYPE_STRINGS[logType], // Thread
            message.get()
        );
        
        // Lock and write to file
        mtx_.lock();
        FILE* file = std::fopen(_logPath.get(), "ab");
        std::fwrite(msg.get(), 1, msg.length(), file);

        // Close and unlock
        std::fclose(file);
        mtx_.unlock();

        // Log to console
        if (logType == LOG_INFO || logType == LOG_MSG) std::puts(msg.get());
        else std::perror(msg.get());
    }

    [[noreturn]] void Logger::crash(const UTF8Str& message) {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        
        if (!crashed_) {
            UTF8Str msg = FormatString::formatString(
                // Crash report
                "---- Crash Report ----\n"
                "Time: %d/%d/%d %d:%02d %s\n" // M/D/YYYY H:MM XM
                "Description: %s\n\n"
                "--- System Details ---\n"
                "Operating System: %s\n"
                "Physical Memory: %lu Bytes\n"
                "CPU: %s\n"
                "CPU Threads: %u\n"
                "Graphics Device: %s\n"
                "Crashing Thread: %s\n",
                
                // What happened and when
                (now->tm_mon + 1), now->tm_mday, (now->tm_year - 100), // Date
                (now->tm_hour % 12), now->tm_min, ((now->tm_hour < 12) ? "AM" : "PM"), // Time
                message.get(), // Description

                // Hardware details
                System::OS().get(),
                System::physicalMemory(),
                System::CPU().get(),
                System::cpuThreadCount(),
                System::GPU().get(),
                Threads::threadName(std::this_thread::get_id()).get()
            );
            std::perror(msg.get());

            // Lock and write file
            mtx_.lock();
            FILE* file = std::fopen(_crashPath.get(), "wb");
            std::fwrite(msg.get(), 1, msg.length(), file);
            
            // Close and unlock
            std::fclose(file);
            mtx_.unlock();
        }
        
        crashed_ = true;
        Core::running = false;
        throw std::runtime_error(message.get());
    }
}

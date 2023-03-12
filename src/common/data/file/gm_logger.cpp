#include "gm_logger.hpp"

#include "gm_file.hpp"
#include "../../gm_core.hpp"
#include "../../system/gm_system.hpp"
#include "../../system/gm_threads.hpp"

#include <atomic>
#include <csignal>
#include <future>
#include <fstream>
#include <iostream>
#include <ctime>
#include <chrono>
#include <mutex>
#include <random>
#include <sstream>
#include <sys/time.h>

namespace game {
    static std::mutex mtx_;
    static std::atomic<bool> crashed_ = false;
    UTF8Str Logger::logPath_ = UTF8Str{sizeof("latest.log") - 1, std::shared_ptr<const char>("latest.log", [](const char*){})};
    UTF8Str Logger::crashPath_ = UTF8Str{sizeof("crash.log") - 1, std::shared_ptr<const char>("crash.log", [](const char*){})};
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
        logPathStr[crashPathLen] = '\0';
        
        Logger::logPath_ = UTF8Str{logPathLen, std::shared_ptr<const char>(logPathStr, std::free)};
        Logger::crashPath_ = UTF8Str{crashPathLen, std::shared_ptr<const char>(crashPathStr, std::free)};
        File::ensureParentDir(Logger::logPath_);
        File::ensureParentDir(Logger::crashPath_);

        std::ofstream file;
        file.open(Logger::logPath_.get(), std::ios::out | std::ios::binary | std::ios::trunc);
        file.close();

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
                Logger::crash("(SIGSEGV) Attempted to read/write memory whose address was not allocated.");
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
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        time_t time = static_cast<time_t>(tv.tv_sec);
        std::tm* now = std::localtime(&time);

        char fmt[64], timeBuffer[64];
        strftime(fmt, sizeof(fmt), "[%H:%M:%S+%%06u]", now);
        snprintf(timeBuffer, sizeof(timeBuffer), fmt, tv.tv_usec);

        mtx_.lock();

        std::stringstream msg;
        // [HH::MM:SS+UUU] [THREAD/TYPE]: MESSAGE
        msg << timeBuffer
            << " ["
            << Threads::threadName(threadId) << "/" << LOG_TYPE_STRINGS[logType] << "]: "
            << message.get() << "\n";

        std::ofstream file;
        file.open(logPath_.get(), std::ios::out | std::ios::binary | std::ios::app);
        file << msg.str();
        file.close();
        if (logType == LOG_INFO || logType == LOG_MSG) std::cout << msg.str();
        else std::cerr << msg.str();

        mtx_.unlock();
    }

    [[noreturn]] void Logger::crash(const UTF8Str& message) {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);
        
        if (!crashed_) {
            std::stringstream msg;
            msg << "---- Crash Report ----\n";
            msg << "Time: " << (now->tm_mon + 1) << "/" << now->tm_mday << "/" << (now->tm_year - 100) << " "
                << (now->tm_hour % 12) << ((now->tm_min < 10) ? ":0" : ":") << now->tm_min << " " << ((now->tm_hour < 12) ? "AM\n" : "PM\n");
            msg << "Description: " << message.get() << "\n\n";
            msg << "--- System Details ---\n";
            msg << "Operating System: " << System::OS().get() << "\n";
            msg << "Physical Memory: " << System::physicalMemory() << "B\n";
            msg << "CPU: " << System::CPU().get() << "\n";
            msg << "CPU Threads: " << System::cpuThreadCount() << "\n";
            msg << "Graphics Device: " << System::GPU().get() << "\n";
            msg << "Crashing Thread: " << Threads::threadName(std::this_thread::get_id()) << "\n";
            std::cerr << msg.str();

            mtx_.lock();
            std::ofstream file;
            file.open(crashPath_.get(), std::ios::out | std::ios::binary | std::ios::trunc);
            file << msg.str();
            file.close();
            mtx_.unlock();
        }
        
        crashed_ = true;
        Core::running = false;
        throw std::runtime_error(message.get());
    }
}

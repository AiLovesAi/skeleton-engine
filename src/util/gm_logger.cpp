#include "gm_logger.hpp"

#include "../gm_game.hpp"
#include "gm_file.hpp"

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
    std::string Logger::logPath_ = "latest.log";
    std::string Logger::crashPath_ = "crash.log";
    void signalHandler(int signum);

    void Logger::init(const std::string& logPath, const std::string& crashPath) {
        setPaths(logPath, crashPath);
        
        // Set signal handlers
        std::signal(SIGINT, signalHandler);
        std::signal(SIGILL, signalHandler);
        std::signal(SIGSEGV, signalHandler);
        std::signal(SIGFPE, signalHandler);
        std::signal(SIGABRT, signalHandler);
        std::signal(SIGTERM, signalHandler);
    }

    void Logger::setPaths(const std::string& logPath, const std::string& crashPath) {
        mtx_.lock();

        Logger::logPath_ = Game::executableDir + logPath;
        Logger::crashPath_ = Game::executableDir + crashPath;
        File::ensureParentDir(Logger::logPath_);
        File::ensureParentDir(Logger::crashPath_);

        std::ofstream file;
        file.open(Logger::logPath_, std::ios::out | std::ios::binary | std::ios::trunc);
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

    void Logger::log(const int logType, const std::string& message) {
        auto task = std::async(std::launch::async, logSync, logType, message, std::this_thread::get_id());
    }

    void Logger::logSync(const int logType, const std::string& message, const std::thread::id& threadId) {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        time_t time = static_cast<time_t>(tv.tv_sec);
        std::tm* now = std::localtime(&time);

        char fmt[64], timeBuffer[64];
        strftime(fmt, sizeof(fmt), "[%H:%M:%S+%%06u]", now);
        snprintf(timeBuffer, sizeof(timeBuffer), fmt, tv.tv_usec);

        std::string threadName = Game::gameThreads.contains(threadId) ? Game::gameThreads.find(threadId)->second : "Async";

        mtx_.lock();

        std::stringstream msg;
        // [HH::MM:SS+UUU] [THREAD/TYPE]: MESSAGE
        msg << timeBuffer
            << " ["
            << threadName << "/" << LOG_TYPE_STRINGS[logType] << "]: "
            << message << "\n";

        std::ofstream file;
        file.open(logPath_, std::ios::out | std::ios::binary | std::ios::app);
        file << msg.str();
        file.close();
        if (logType == LOG_INFO || logType == LOG_MSG) std::cout << msg.str();
        else std::cerr << msg.str();

        mtx_.unlock();
    }

    [[noreturn]] void Logger::crash(const std::string& message) {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        std::stringstream msg;
        msg << "---- Crash Report ----\n";
        msg << "Time: " << now->tm_mon << "/" << now->tm_mday << "/" << (now->tm_year - 30) << " "
            << (now->tm_hour % 12) << ((now->tm_min < 10) ? ":0" : ":") << now->tm_min << " " << ((now->tm_hour < 12) ? "AM\n" : "PM\n");
        msg << "Description: " << message << "\n\n";
        msg << "-- System Details --\nDetails:\n";
        msg << "Operating System: " << Game::OSStr << "\n";
        msg << "CPU: " << Game::CPUStr << "\n";
        msg << "Graphics device: " << Game::graphicsDeviceName << "\n";
        msg << "Thread: " << Game::gameThreads.find(std::this_thread::get_id())->second << "\n";
        std::cerr << msg.str();

        mtx_.lock();
        std::ofstream file;
        file.open(crashPath_, std::ios::out | std::ios::binary | std::ios::trunc);
        file << msg.str();
        file.close();
        mtx_.unlock();

        Game::running = false;
        throw std::runtime_error(message);
    }
}

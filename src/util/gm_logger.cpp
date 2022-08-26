#include "gm_logger.hpp"

#include "../gm_game.hpp"
#include "gm_file.hpp"

#include <csignal>
#include <fstream>
#include <thread>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>

namespace game {
    static std::mutex mtx;
    std::string Logger::logPath = "latest.log";
    std::string Logger::crashPath = "crash.log";

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
        mtx.lock();

        Logger::logPath = Game::executableDir + logPath;
        Logger::crashPath = Game::executableDir + crashPath;
        File::ensureParentDir(Logger::logPath);
        File::ensureParentDir(Logger::crashPath);

        std::ofstream file;
        file.open(Logger::logPath, std::ios::out | std::ios::binary | std::ios::trunc);
        file.close();

        mtx.unlock();
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
                Logger::logMsg(LOG_INFO, "(SIGTERM) Received termination signal. Closing.");
                exit(EXIT_SUCCESS);
                break;
            default:
            Logger::crash("Unknown signal was raised.");
            break;
        }
    }

    void Logger::logMsg(const int logType, const std::string& message) {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        mtx.lock();

        std::stringstream msg;
        msg << ((now->tm_hour < 10) ? "[0" : "[") << now->tm_hour << ((now->tm_min < 10) ? ":0" : ":") << now->tm_min << ((now->tm_sec < 10) ? ":0" : ":") << now->tm_sec << "] ["
         << Game::gameThreads.find(std::this_thread::get_id())->second << "/" << LOG_TYPE_STRINGS[logType] << "]: "
         << message << "\n";

        std::ofstream file;
        file.open(logPath, std::ios::out | std::ios::binary | std::ios::app);
        file << msg.str();
        file.close();
        if (logType == LOG_INFO || logType == LOG_MSG) std::cout << msg.str();
        else std::cerr << msg.str();

        mtx.unlock();
    }

    [[noreturn]] void Logger::crash(const std::string& message) {
        std::time_t t = std::time(0);
        std::tm* now = std::localtime(&t);

        int hour = now->tm_hour % 12;
        if (hour == 0) hour = 12;

        std::stringstream msg;
        msg << "---- Crash Report ----\n";
        msg << "Time: " << now->tm_mon << "/" << now->tm_mday << "/" << (now->tm_year - 30) << " "
            << (now->tm_hour % 12) << ((now->tm_min < 10) ? ":0" : ":") << now->tm_min << " " << ((now->tm_hour < 12) ? "AM\n" : "PM\n");
        msg << "Description: " << message << "\n\n";
        msg << File::getStack() << "\n\n";
        msg << "-- System Details --\nDetails:\n";
        msg << "Operating System: " << Game::OSStr << "\n";
        msg << "CPU: " << Game::CPUStr << "\n";
        msg << "Graphics device: " << Game::graphicsDeviceName << "\n";
        msg << "Thread: " << Game::gameThreads.find(std::this_thread::get_id())->second << "\n";
        std::cerr << msg.str();

        mtx.lock();
        std::ofstream file;
        file.open(crashPath, std::ios::out | std::ios::binary | std::ios::trunc);
        file << msg.str();
        file.close();
        mtx.unlock();

        Game::running = false;
        throw std::runtime_error(message);
    }
}

#include "gm_logger.hpp"

#include "../gm_game.hpp"
#include "../graphics/gm_graphics_device.hpp"
#include "gm_file.hpp"

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

    void Logger::setPaths(std::string logPath, std::string crashPath) {
        logPath = logPath;
        crashPath = crashPath;
        File::ensureParentDir(logPath);
        File::ensureParentDir(crashPath);

        std::ofstream file;
        file.open(logPath, std::ios::out | std::ios::binary | std::ios::trunc);
        file.close();
    }

    void Logger::logMsg(int logType, std::string message) {
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

    [[noreturn]] void Logger::crash(std::string message) {
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
        msg << "Operating System: " << File::OSStr << "\n";
        msg << "CPU: " << File::CPUStr << "\n";
        msg << "Graphics device: " << GraphicsDevice::graphicsDeviceName << "\n";
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

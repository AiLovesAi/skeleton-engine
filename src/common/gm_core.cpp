#include "gm_core.hpp"

#include "data/gm_file.hpp"
#include "logger/gm_logger.hpp"
#include "system/gm_system.hpp"
#include "system/gm_threads.hpp"

#include <ctime>
#include <random>
#include <sstream>

namespace game {
    std::string Core::TITLE = Core::EMPTYSTR;
    std::string Core::VERSION = Core::EMPTYSTR;
    std::atomic<bool> Core::running = false;

    void Core::init(const std::string& logFile, const std::string& crashFile) {
        Threads::registerThread(std::this_thread::get_id(), "Main");
        std::srand(std::time(0));

        System::init();
        File::init();
        Logger::init(logFile, crashFile);


        std::stringstream msg;
        msg << "Hardware details:\n";
        msg << "\tCPU: " << System::CPU() << "\n";
        msg << "\tCPU threads: " << System::cpuThreadCount() << "\n";
        msg << "\tPhysical memory: " << System::physicalMemory() << "B";
        Logger::log(LOG_INFO, msg.str());
    }
}

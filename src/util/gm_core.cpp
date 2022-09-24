#include "gm_core.hpp"

#include "file/gm_file.hpp"
#include "logger/gm_logger.hpp"
#include "system/gm_system.hpp"
#include "system/gm_threads.hpp"

#include <ctime>
#include <random>

namespace game {
    std::string Core::TITLE = Core::EMPTYSTR;
    std::string Core::VERSION = Core::EMPTYSTR;
    std::atomic<bool> Core::running = false;

    void Core::init() {
        Threads::registerThread(std::this_thread::get_id(), "Main");
        std::srand(std::time(0));

        System::init();
        File::init();
        Logger::init("../logs/latest.log", "../logs/crash.txt");

        Logger::log(LOG_INFO, "Using CPU: " + System::CPU());
    }
}

#include "gm_core.hpp"

#include "data/file/gm_file.hpp"
#include "data/file/gm_logger.hpp"
#include "system/gm_system.hpp"
#include "system/gm_threads.hpp"

#include <ctime>
#include <random>
#include <sstream>

namespace game {
    const UTF8Str Core::EMPTYSTR = UTF8Str{sizeof("NULL") - 1, std::shared_ptr<const char>("NULL", [](const char*){})};
    UTF8Str Core::TITLE = Core::EMPTYSTR;
    UTF8Str Core::VERSION = Core::EMPTYSTR;
    std::atomic<bool> Core::running = false;

    void Core::init(const char* logFile, const char* crashFile) {
        Threads::registerThread(std::this_thread::get_id(), "Main");
        std::srand(std::time(0));

        System::init();
        File::init();
        Logger::init(
            UTF8Str{static_cast<int64_t>(std::strlen(logFile)), std::shared_ptr<const char>(logFile, [](const char*){})},
            UTF8Str{static_cast<int64_t>(std::strlen(crashFile)), std::shared_ptr<const char>(crashFile, [](const char*){})}
        );

        std::stringstream msg;
        msg << "Hardware details:\n";
        msg << "\tCPU: " << System::CPU().get() << "\n";
        msg << "\tCPU threads: " << System::cpuThreadCount() << "\n";
        msg << "\tPhysical memory: " << System::physicalMemory() << "B\n";
        msg << "\tOperating system: " << System::OS().get();
        Logger::log(LOG_INFO, msg.str());
    }
}

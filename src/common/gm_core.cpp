#include "gm_core.hpp"

#include "headers/file.hpp"
#include "headers/string.hpp"
#include "system/gm_system.hpp"
#include "system/gm_threads.hpp"

#include <ctime>
#include <random>

namespace game {
    UTF8Str Core::TITLE = EMPTY_STR;
    UTF8Str Core::VERSION = EMPTY_STR;
    std::atomic<bool> Core::running = false;

    void Core::init(const char*__restrict__ logFile, const char*__restrict__ crashFile) {
        Threads::registerThread(std::this_thread::get_id(), UTF8Str{"Main", sizeof("Main") - 1});
        std::srand(std::time(0));

        System::init();
        File::init();
        Logger::init(
            UTF8Str{static_cast<int64_t>(std::strlen(logFile)), std::shared_ptr<const char>(logFile, [](const char*){})},
            UTF8Str{static_cast<int64_t>(std::strlen(crashFile)), std::shared_ptr<const char>(crashFile, [](const char*){})}
        );

        UTF8Str msg = FormatString::formatString(
            "Hardware details:\n"
            "\tCPU: %s\n"
            "\tCPU threads: %d\n"
            "\tPhysical memory: %luB\n"
            "\tOperating system: %s",

            System::CPU().get(),
            System::cpuThreadCount(),
            System::physicalMemory(),
            System::OS().get()
        );
        Logger::log(LOG_INFO, msg.get());
    }
}

#include "gm_threads.hpp"

namespace game {
    std::unordered_map<std::thread::id, std::string> Threads::threads_;
}

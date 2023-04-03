#include "gm_threads.hpp"

namespace game {
    std::unordered_map<std::thread::id, UTF8Str> Threads::_threads;
}

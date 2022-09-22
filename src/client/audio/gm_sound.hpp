#pragma once

#include <string>

namespace game {
    class Sound {
        public:
            Sound(const std::string& id);
            ~Sound();
        private:
            bool stream_ = false;
            char* data_ = nullptr; // NOTE: Used for file name if stream_ is true.
            int bitRate_ = 0;
            size_t size_ = 0;
    };
}

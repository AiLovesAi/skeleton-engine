#pragma once

#include <string>

namespace game {
    class Sound {
        public:
            Sound(const std::string& id);
            ~Sound();
        private:
            bool _stream = false;
            char* _data = nullptr; // NOTE: Used for file name if _stream is true.
            int bitRate_ = 0;
            size_t _size = 0;
    };
}

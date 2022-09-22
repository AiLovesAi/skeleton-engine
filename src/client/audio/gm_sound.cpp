#include "gm_sound.hpp"

#include <fstream>

namespace game {
    Sound::Sound(const std::string& id) {
        // TODO Find "minecraft:soundid" in soundId file
        std::string file = "FINDME";

        // TODO Load data into buffer and update size_

        data_ = new char[size_];
    }

    Sound::~Sound() {
        if (data_) delete[] data_;
    }
}

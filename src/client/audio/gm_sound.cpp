#include "gm_sound.hpp"

#include <fstream>

namespace game {
    Sound::Sound(const std::string& id) {
        // TODO Find "minecraft:soundid" in soundId file
        std::string file = "FINDME";

        // TODO Load data into buffer and update size_

        _data = new char[_size];
    }

    Sound::~Sound() {
        if (_data) delete[] _data;
    }
}

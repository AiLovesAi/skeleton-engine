#pragma once

#include "gm_sound_instance.hpp"

#include <unordered_map>
#include <string>

namespace game {
    class Audio {
        public:
            // Constructors
            Audio(const EntityPool& entityPool) : entityPool_{entityPool} {}

            // Functions
            void loadSounds();
            Sound* getSound(const std::string& id);

            SoundInstance* playSound(const std::string& id);
        private:
            EntityPool entityPool_;
            SoundPool instancePool_{entityPool_, 16};
            std::unordered_map<std::string, Sound> sounds_;
    };
}

#pragma once

#include "gm_sound_instance.hpp"

#include <unordered_map>
#include <string>

namespace game {
    class Audio {
        public:
            // Functions
            void loadSounds();
            Sound* getSound(const std::string& id);

            SoundInstance* playSound(const std::string& id);
        private:
            SoundInstancePool instancePool_;
            std::unordered_map<std::string, Sound> sounds_;
    };
}

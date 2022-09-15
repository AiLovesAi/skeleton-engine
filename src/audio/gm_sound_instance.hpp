#pragma once

#include "gm_sound.hpp"
#include "../game/components/gm_physics_component.hpp"

namespace game {
    class SoundInstance {
        public:
            // Functions
            void init();

            Sound* sound() const { return sound_; }
            PhysicsComponent* physics() const { return physics_; }
            float pitch() const { return pitch_; }
            float volume() const { return volume_; }
            float position() const { return position_; }

        private:
            // Variables
            Sound* sound_ = nullptr;
            PhysicsComponent* physics_ = nullptr;

            float pitch_ = 1.f;
            float volume_ = 1.f;
            size_t position_ = 0; // Position in sound data
    };

    class SoundInstancePool {
        public:
            SoundInstance* createObject();
            void destroyObject(const int index);
        private:
            static constexpr int POOL_SIZE = 1024;
            SoundInstance pool_[POOL_SIZE];
            int numSounds_ = 0;
    };
}

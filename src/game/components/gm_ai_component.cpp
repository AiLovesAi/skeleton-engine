#include "gm_ai_component.hpp"

namespace game {
    // AIComponentPool //
    AIComponent* AIComponentPool::createObject() {
        // If there is no room in the pool, the caller will have to either
        // create a new one or have nothing created.
        if (numComponents_ >= POOL_SIZE) return nullptr;

        AIComponent* newComponent = &pool_[numComponents_++];
        newComponent->init();
        
        return newComponent;
    }

    void AIComponentPool::destroyObject(const int index) {
        // Swap with last active object and deactivate
        AIComponent temp = pool_[--numComponents_];
        pool_[numComponents_] = pool_[index];
        pool_[index] = temp;
    }

    void AIComponentPool::updateComponents() {
        for (int i = 0; i < numComponents_; i++) {
            if (pool_[i].update()) destroyObject(i);
        }
    }

    // AIComponent //
    void AIComponent::init() {

    }

    bool AIComponent::update() {
        return false;
    }
}

#include "gm_physics_component.hpp"

namespace game {
    // PhysicsComponentPool //
    PhysicsComponent* PhysicsComponentPool::createObject() {
        // If there is no room in the pool, the caller will have to either
        // create a new one or have nothing created.
        if (numComponents_ >= POOL_SIZE) return nullptr;

        PhysicsComponent* newComponent = &pool_[numComponents_++];
        newComponent->init();
        
        return newComponent;
    }

    void PhysicsComponentPool::destroyObject(const int index) {
        // Swap with last active object and deactivate
        PhysicsComponent temp = pool_[--numComponents_];
        pool_[numComponents_] = pool_[index];
        pool_[index] = temp;
    }

    void PhysicsComponentPool::updateComponents() {
        for (int i = 0; i < numComponents_; i++) {
            if (pool_[i].update()) destroyObject(i);
        }
    }

    // PhysicsComponent //
    void PhysicsComponent::init() {

    }

    bool PhysicsComponent::update() {
        return false;
    }
}

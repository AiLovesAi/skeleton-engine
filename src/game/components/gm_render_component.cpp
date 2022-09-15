#include "gm_render_component.hpp"

namespace game {
    // RenderComponentPool //
    RenderComponent* RenderComponentPool::createObject() {
        // If there is no room in the pool, the caller will have to either
        // create a new one or have nothing created.
        if (numComponents_ >= POOL_SIZE) return nullptr;

        RenderComponent* newComponent = &pool_[numComponents_++];
        newComponent->init();
        
        return newComponent;
    }

    void RenderComponentPool::destroyObject(const int index) {
        // Swap with last active object and deactivate
        RenderComponent temp = pool_[--numComponents_];
        pool_[numComponents_] = pool_[index];
        pool_[index] = temp;
    }

    void RenderComponentPool::renderComponents() {
        for (int i = 0; i < numComponents_; i++) {
            if (pool_[i].render()) destroyObject(i);
        }
    }

    // RenderComponent //
    void RenderComponent::init() {

    }

    bool RenderComponent::render() {
        return false;
    }
}

#include "gm_render_component.hpp"

namespace game {
    // RenderComponentPool //
    RenderComponent* RenderComponentPool::createComponent() {
        // If there is no room in the pool, the caller will have to either
        // create a new one or have nothing created.
        if (numComponents_ >= POOL_SIZE) return nullptr;

        RenderComponent* newComponent = &pool_[numComponents_];
        newComponent->init();
        
        numComponents_++;
        return newComponent;
    }

    void RenderComponentPool::destroyComponent(const int index) {
        // Swap with last active component and deactivate
        numComponents_--;
        RenderComponent temp = pool_[numComponents_];
        pool_[numComponents_] = pool_[index];
        pool_[index] = temp;
    }

    void RenderComponentPool::renderComponents() {
        for (int i = 0; i < numComponents_; i++) {
            if (pool_[i].render()) destroyComponent(i);
        }
    }

    // RenderComponent //
    void RenderComponent::init() {

    }

    bool RenderComponent::render() {
        return false;
    }
}

#pragma once

namespace game {
    class RenderComponent {
        public:
            // Constructors
            RenderComponent();
            ~RenderComponent();

            RenderComponent(const RenderComponent&) = delete;
            RenderComponent& operator=(const RenderComponent&) = delete;
            RenderComponent(RenderComponent&&) = default;
            RenderComponent& operator=(RenderComponent&&) = default;
    };
}

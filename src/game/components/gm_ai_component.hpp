#pragma once

namespace game {
    class AIComponent {
        public:
            // Constructors
            AIComponent();
            ~AIComponent();

            AIComponent(const AIComponent&) = delete;
            AIComponent& operator=(const AIComponent&) = delete;
            AIComponent(AIComponent&&) = default;
            AIComponent& operator=(AIComponent&&) = default;
    };
}

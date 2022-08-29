#pragma once

namespace game {
    class Button {
        public:
            // Constructors
            Button(float x, float y, float width, float height, void(*action)()) : x{x}, y{y}, width{width}, height{height}, action{action} {}

            void press() { action(); }

            float x;
            float y;
            float width;
            float height;
            void (*action)();
    };
}

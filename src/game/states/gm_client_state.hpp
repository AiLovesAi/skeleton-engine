#pragma once

#include "gm_game_state.hpp"
#include "../entities/gm_entity.hpp"
#include "../components/gm_render_component.hpp"
#include "../world/gm_world.hpp"
#include "../sides/gm_client.hpp"
#include "../../graphics/vulkan/gm_renderer.hpp"
#include "../../graphics/game/gm_camera.hpp"

namespace game {
    class ClientState : public GameState {
        public:
            // Constructors
            ClientState();
            ~ClientState();

            // Functions
            virtual void update() override;
            virtual void render(const double lag) override;

        private:
            // Variables
            EntityPool entityPool_;
            RenderPool renderPool_{entityPool_};

            World world_{entityPool_};

            Camera camera_{
                entityPool_.create(),
                world_,
                Client::instance().window(),
                45.f,
                std::numeric_limits<float>::min(),
                100.0f
            };
    };
}

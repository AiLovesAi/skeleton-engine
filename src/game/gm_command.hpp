#pragma once

namespace game {
    class Command {
        public:
            virtual ~Command();
            virtual void execute() = 0;
    };
}

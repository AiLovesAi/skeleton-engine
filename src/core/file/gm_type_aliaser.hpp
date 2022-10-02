#pragma once

namespace game {
    template <typename tFrom, typename tTo>
    class TypeAliaser {
        public:
            // Constructors
            TypeAliaser(tFrom inFromValue): mAsFromType(inFromValue) {}

            // Functions
            tTo& get() {return mAsToType;}
            
            // Variables
            union {
                tFrom mAsFromType;
                tTo mAsToType;
            };
    };
}

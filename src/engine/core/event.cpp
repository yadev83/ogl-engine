#include "event.hpp"

namespace Engine::Core {
    /**
     * @brief Un GlobalEventDispatcher est mis à disposition pour émettre et écouter des évènements sans contexte spécifique.
     */
    inline EventDispatcher GlobalEventDispatcher;
}
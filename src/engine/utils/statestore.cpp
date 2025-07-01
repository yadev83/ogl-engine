#include "statestore.hpp"

namespace Engine::Utils {
    bool StateStore::HasKey(const std::string& key) const {
        return mStore.find(key) != mStore.end();
    }
}
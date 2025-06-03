#include "core/Tile.hpp"
#include <algorithm>

namespace Zappy {
    void Tile::addResource(Resource* resource) {
        if (resource) {
            resources_.push_back(resource);
        }
    }

    void Tile::removeResource(Resource* resource) {
        if (!resource) return;
        auto it = std::find(resources_.begin(), resources_.end(), resource);
        if (it != resources_.end()) {
            resources_.erase(it);
        }
    }
} 
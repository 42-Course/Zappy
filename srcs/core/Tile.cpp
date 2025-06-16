#include "core/Tile.hpp"
#include "core/Resource.hpp"
#include <algorithm>

namespace Zappy {
    Tile::Tile() = default;

    Tile::~Tile() {
        for (const auto& resource : resources_) {
            notify([this, resource](IObserver* obs) {
                obs->onResourceRemoved(this, resource->getType());
            });
        }
    }

    void Tile::notifyResourceAdded(ResourceType type) const {
        notify([this, type](IObserver* obs) {
            obs->onResourceAdded(this, type);
        });
    }

    void Tile::notifyResourceRemoved(ResourceType type) const {
        notify([this, type](IObserver* obs) {
            obs->onResourceRemoved(this, type);
        });
    }

    void Tile::addResource(Resource* resource) {
        if (resource) {
            resources_.push_back(resource);
            notifyResourceAdded(resource->getType());
        }
    }

    void Tile::removeResource(Resource* resource) {
        if (!resource) return;
        auto it = std::find(resources_.begin(), resources_.end(), resource);
        if (it != resources_.end()) {
            ResourceType type = (*it)->getType();
            resources_.erase(it);
            notifyResourceRemoved(type);
        }
    }

    int Tile::getResourceCount(ResourceType type) const {
        return std::count_if(resources_.begin(), resources_.end(),
            [type](const Resource* r) { return r->getType() == type; });
    }
} 
#include "core/Tile.hpp"
#include "core/Resource.hpp"
#include <algorithm>

namespace Zappy {
    Tile::Tile() = default;

    Tile::~Tile() {
        // Notify observers that tile is being destroyed
        for (auto observer : observers_) {
            for (const auto& resource : resources_) {
                observer->onResourceRemoved(this, resource->getType());
            }
        }
    }

    void Tile::attach(Observer* observer) {
        if (observer) {
            observers_.push_back(observer);
        }
    }

    void Tile::detach(Observer* observer) {
        if (observer) {
            observers_.erase(
                std::remove(observers_.begin(), observers_.end(), observer),
                observers_.end()
            );
        }
    }

    void Tile::notifyResourceAdded(ResourceType type) const {
        for (auto observer : observers_) {
            observer->onResourceAdded(this, type);
        }
    }

    void Tile::notifyResourceRemoved(ResourceType type) const {
        for (auto observer : observers_) {
            observer->onResourceRemoved(this, type);
        }
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
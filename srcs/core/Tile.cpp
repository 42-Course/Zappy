#include "core/Tile.hpp"
#include "core/Resource.hpp"
#include "core/utils.hpp"
#include <algorithm>
#include <sstream>

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
    
    std::vector<int> Tile::getAllResourceCounts() const {
        std::vector<int> counts;
        counts.reserve(static_cast<int>(ResourceType::THYSTAME) + 1);
        for (int i = 0; i <= static_cast<int>(ResourceType::THYSTAME); ++i) {
            counts.push_back(getResourceCount(static_cast<ResourceType>(i)));
        }
        return counts;
    }

    std::string Tile::getResourceCountsAsString() const {
        std::stringstream ss;
        const auto counts = getAllResourceCounts();
        for (size_t i = 0; i < counts.size(); ++i) {
            if (i > 0) ss << " ";
            ss << counts[i];
        }
        return ss.str();
    }

    std::string Tile::resourcesToString() const {
        std::vector<std::string> items;

        for (const auto& res : getResources()) {
            items.push_back(res->getName());
        }

        return join(items, " ");
    }


    std::string Tile::toBctString(int x, int y) const {
        std::stringstream ss;

        ss << "bct " << x << " " << y << " " << getResourceCountsAsString() << "\n";
        return ss.str();
    }

} 
#include "core/Tile.hpp"
#include "core/IObserver.hpp"
#include "core/utils.hpp"
#include <sstream>
#include <random>

namespace Zappy {

    Tile::Tile() {
        initializeRandomResources();
    }

    void Tile::addResource(ResourceType type, int count) {
        resources_[type] += count;
        notifyResourceAdded(type);
    }

    void Tile::removeResource(ResourceType type, int count) {
        auto& current = resources_[type];
        if (count >= current)
            current = 0;
        else
            current -= count;
        notifyResourceRemoved(type);
    }

    int Tile::getResourceCount(ResourceType type) const {
        auto it = resources_.find(type);
        return it != resources_.end() ? it->second : 0;
    }

    std::unordered_map<ResourceType, int> Tile::getAllResourceCounts() const {
        return resources_;
    }

    std::string Tile::getResourceCountsAsString() const {
        std::ostringstream oss;
        for (int i = 0; i <= static_cast<int>(ResourceType::THYSTAME); ++i) {
            auto type = static_cast<ResourceType>(i);
            oss << getResourceCount(type);
            if (i != static_cast<int>(ResourceType::THYSTAME)) oss << " ";
        }
        return oss.str();
    }

    std::string Tile::toBctString(int x, int y) const {
        std::ostringstream oss;
        oss << "bct " << x << " " << y << " " << getResourceCountsAsString() << "\n";
        return oss.str();
    }

    std::string Tile::resourcesToString() const {
        std::vector<std::string> items;
        for (const auto& [type, count] : resources_) {
            for (int i = 0; i < count; ++i)
                items.push_back(Resource::typeToString(type));
        }
        return join(items, " ");
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

    void Tile::initializeRandomResources() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 5); // adjust range as needed

        for (int i = 0; i <= static_cast<int>(ResourceType::THYSTAME); ++i) {
            ResourceType type = static_cast<ResourceType>(i);
            resources_[type] = dist(gen);
        }
    }

}

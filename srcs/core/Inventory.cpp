#include "core/Inventory.hpp"
#include "core/Resource.hpp"

namespace Zappy {
    Inventory::Inventory() {
        // Initialize all resource counts to 0
        for (int type = static_cast<int>(ResourceType::FOOD); 
             type <= static_cast<int>(ResourceType::THYSTAME); 
             type++) {
            resources_[static_cast<ResourceType>(type)] = 0;
        }
    }

    bool Inventory::add(Resource* resource) {
        if (!resource) return false;
        resources_[resource->getType()]++;
        return true;
    }

    bool Inventory::remove(Resource* resource) {
        if (!resource) return false;
        auto it = resources_.find(resource->getType());
        if (it != resources_.end() && it->second > 0) {
            it->second--;
            return true;
        }
        return false;
    }

    int Inventory::getCount(ResourceType type) const {
        auto it = resources_.find(type);
        return it != resources_.end() ? it->second : 0;
    }
} 
#include "core/Inventory.hpp"
// #include "core/Resource.hpp"

namespace Zappy {
    Inventory::Inventory() {
        // Initialize with empty resource counts
        contents_["food"] = 0;
        contents_["linemate"] = 0;
        contents_["deraumere"] = 0;
        contents_["sibur"] = 0;
        contents_["mendiane"] = 0;
        contents_["phiras"] = 0;
        contents_["thystame"] = 0;
    }

    Inventory::~Inventory() = default;

    // void Inventory::addResource(Resource* resource) {
    //     if (!resource) return;
    //     contents_[resource->getType()]++;
    // }

    // bool Inventory::removeResource(Resource* resource) {
    //     if (!resource) return false;
        
    //     auto& count = contents_[resource->getType()];
    //     if (count > 0) {
    //         count--;
    //         return true;
    //     }
    //     return false;
    // }

    int Inventory::getResourceCount(const std::string& type) const {
        auto it = contents_.find(type);
        if (it != contents_.end()) {
            return it->second;
        }
        return 0;
    }

    bool Inventory::hasResources(const std::map<std::string, int>& requirements) const {
        for (const auto& [type, count] : requirements) {
            if (getResourceCount(type) < count) {
                return false;
            }
        }
        return true;
    }
} 
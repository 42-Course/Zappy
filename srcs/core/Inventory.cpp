#include "core/Inventory.hpp"
// #include "core/Resource.hpp"

namespace Zappy {
    Inventory::Inventory() {
        // Initialize all resources to 0
        contents_[std::string("food")] = 0;
        contents_[std::string("linemate")] = 0;
        contents_[std::string("deraumere")] = 0;
        contents_[std::string("sibur")] = 0;
        contents_[std::string("mendiane")] = 0;
        contents_[std::string("phiras")] = 0;
        contents_[std::string("thystame")] = 0;
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

    void Inventory::add(const std::string& resource, int amount) {
        if (contents_.find(resource) != contents_.end()) {
            contents_[resource] += amount;
        }
    }

    void Inventory::remove(const std::string& resource, int amount) {
        if (contents_.find(resource) != contents_.end()) {
            contents_[resource] = std::max(0, contents_[resource] - amount);
        }
    }

    int Inventory::get(const std::string& resource) const {
        auto it = contents_.find(resource);
        return it != contents_.end() ? it->second : 0;
    }

    void Inventory::set(const std::string& resource, int amount) {
        if (contents_.find(resource) != contents_.end()) {
            contents_[resource] = std::max(0, amount);
        }
    }

    const std::map<std::string, int>& Inventory::getContents() const {
        return contents_;
    }
} 
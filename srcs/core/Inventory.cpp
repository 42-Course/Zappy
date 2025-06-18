#include "core/Inventory.hpp"
#include "core/Resource.hpp"
#include <sstream>

namespace Zappy {
    Inventory::Inventory() {
        // Initialize all resource counts to 0
        for (int type = static_cast<int>(ResourceType::FOOD); 
             type <= static_cast<int>(ResourceType::THYSTAME); 
             type++) {
            resources_[static_cast<ResourceType>(type)] = 0;
        }
    }

    bool Inventory::add(ResourceType type, int amount) {
        if (amount <= 0) return false;
        resources_[type] += amount;
        return true;
    }

    bool Inventory::remove(ResourceType type, int amount) {
        auto& count = resources_[type];
        if (amount <= 0 || count < amount) return false;
        count -= amount;
        return true;
    }

    int Inventory::getCount(ResourceType type) const {
        auto it = resources_.find(type);
        return it != resources_.end() ? it->second : 0;
    }

    std::string Inventory::toInventoryString() const {
        std::stringstream ss;
        ss << "[";
        ss << "food " << getFood() << " ";
        ss << "linemate " << getLinemate() << " ";
        ss << "deraumere " << getDeraumere() << " ";
        ss << "sibur " << getSibur() << " ";
        ss << "mendiane " << getMendiane() << " ";
        ss << "phiras " << getPhiras() << " ";
        ss << "thystame " << getThystame();
        ss << "]\n";
        return ss.str();
    }
} 
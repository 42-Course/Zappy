#pragma once

#include <map>
#include <memory>
#include "Resource.hpp"

namespace Zappy {
    class Inventory {
    public:
        Inventory();
        ~Inventory() = default;

        // Resource management
        bool add(Resource* resource);
        bool remove(Resource* resource);
        int getCount(ResourceType type) const;

        // Resource getters
        int getFood() const { return getCount(ResourceType::FOOD); }
        int getLinemate() const { return getCount(ResourceType::LINEMATE); }
        int getDeraumere() const { return getCount(ResourceType::DERAUMERE); }
        int getSibur() const { return getCount(ResourceType::SIBUR); }
        int getMendiane() const { return getCount(ResourceType::MENDIANE); }
        int getPhiras() const { return getCount(ResourceType::PHIRAS); }
        int getThystame() const { return getCount(ResourceType::THYSTAME); }

    private:
        std::map<ResourceType, int> resources_;
    };
} 
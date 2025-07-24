#pragma once

#include <unordered_map>
#include <string>
#include "Resource.hpp"
#include "Subject.hpp"

namespace Zappy {
    class Tile : public Subject {
    public:
        Tile();
        ~Tile() = default;

        // Resource operations
        void addResource(ResourceType type, int count = 1);
        void removeResource(ResourceType type, int count = 1);
        int getResourceCount(ResourceType type) const;
        std::unordered_map<ResourceType, int> getAllResourceCounts() const;
        std::string getResourceCountsAsString() const;
        std::string toBctString(int x, int y) const;
        std::string resourcesToString() const;

    private:
        std::unordered_map<ResourceType, int> resources_;
        void initializeRandomResources();
    };
}

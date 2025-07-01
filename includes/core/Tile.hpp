#pragma once

#include <vector>
#include "Resource.hpp"
#include "IObserver.hpp"
#include "Subject.hpp"

namespace Zappy {
    class Tile : public Subject {
    public:
        Tile();
        ~Tile();

        // Resource operations
        void addResource(Resource* resource);
        void removeResource(Resource* resource);
        int getResourceCount(ResourceType type) const;
        const std::vector<Resource*>& getResources() const { return resources_; }
        std::vector<int> getAllResourceCounts() const;
        std::string getResourceCountsAsString() const;
        std::string toBctString(int x, int y) const;
        std::string resourcesToString() const;

    protected:
        void notifyResourceAdded(ResourceType type) const;
        void notifyResourceRemoved(ResourceType type) const;

    private:
        std::vector<Resource*> resources_;  // Non-owning pointers, resources owned by World
    };
} 
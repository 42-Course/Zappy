#pragma once

#include <vector>
#include "Resource.hpp"
#include "Observer.hpp"

namespace Zappy {
    class Tile {
    public:
        Tile();
        ~Tile();

        // Resource operations
        void addResource(Resource* resource);
        void removeResource(Resource* resource);
        int getResourceCount(ResourceType type) const;
        const std::vector<Resource*>& getResources() const { return resources_; }

        // Observer pattern
        void attach(Observer* observer);
        void detach(Observer* observer);

    protected:
        void notifyResourceAdded(ResourceType type) const;
        void notifyResourceRemoved(ResourceType type) const;

    private:
        std::vector<Resource*> resources_;  // Non-owning pointers, resources owned by World
        std::vector<Observer*> observers_;  // Non-owning pointers
    };
} 
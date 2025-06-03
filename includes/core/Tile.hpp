#pragma once

#include <vector>

namespace Zappy {
    class Resource;

    class Tile {
    public:
        Tile() = default;
        ~Tile() = default;

        // Resource operations
        void addResource(Resource* resource);
        void removeResource(Resource* resource);
        const std::vector<Resource*>& getResources() const { return resources_; }

    private:
        std::vector<Resource*> resources_; // Non-owning pointers, resources owned by World
    };
} 
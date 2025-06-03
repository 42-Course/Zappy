#pragma once

#include <vector>
#include <memory>

namespace Zappy {
    // Forward declarations
    class Tile;
    class Resource;

    class Map {
    public:
        Map(int width, int height);
        ~Map();

        // Map dimensions
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

        // Tile operations
        Tile* getTile(int x, int y);
        const Tile* getTile(int x, int y) const;
        
        // Resource operations
        void addResource(int x, int y, Resource* resource);
        void removeResource(int x, int y, Resource* resource);
        
        // Map updates
        void update();
        void generateResources();

    private:
        int width_;
        int height_;
        std::vector<std::unique_ptr<Tile>> tiles_;
    };
} 
#pragma once

#include <vector>
#include <memory>
#include <functional>

namespace Zappy {
    // Forward declarations
    class Tile;
    class Resource;

    class Map {
    public:
        Map(int width, int height, bool infinite);
        ~Map();

        // Map dimensions and properties
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        bool isInfinite() const { return infinite_; }
        void setInfinite(bool infinite) { infinite_ = infinite; }

        // Coordinate normalization
        void normalizeCoordinates(int& x, int& y) const;
        bool isValidCoordinate(int x, int y) const;

        // Tile operations
        Tile* getTile(int x, int y);
        const Tile* getTile(int x, int y) const;
        
        // Resource operations
        void addResource(int x, int y, Resource* resource);
        void removeResource(int x, int y, Resource* resource);
        
        // Map updates and expansion
        void update();
        void generateResources();
        void expand(int newWidth, int newHeight);
        void expandToInclude(int x, int y);

        // Event callbacks
        using MapUpdateCallback = std::function<void(int x, int y, const Tile* tile)>;
        void setUpdateCallback(MapUpdateCallback callback) { updateCallback_ = callback; }

    private:
        int width_;
        int height_;
        bool infinite_;
        std::vector<std::unique_ptr<Tile>> tiles_;
        MapUpdateCallback updateCallback_;

        void resizeTiles(int newWidth, int newHeight);
        void notifyUpdate(int x, int y, const Tile* tile);
    };
} 
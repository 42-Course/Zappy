#include "core/Map.hpp"
#include "core/Tile.hpp"
#include <stdexcept>
#include <algorithm>

namespace Zappy {
    Map::Map(int width, int height) 
        : width_(width)
        , height_(height) {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Map dimensions must be positive");
        }

        // Initialize tiles
        tiles_.reserve(width * height);
        for (int i = 0; i < width * height; ++i) {
            tiles_.push_back(std::make_unique<Tile>());
        }
    }

    Map::~Map() = default;

    Tile* Map::getTile(int x, int y) {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return nullptr;
        }
        return tiles_[y * width_ + x].get();
    }

    const Tile* Map::getTile(int x, int y) const {
        if (x < 0 || x >= width_ || y < 0 || y >= height_) {
            return nullptr;
        }
        return tiles_[y * width_ + x].get();
    }

    void Map::addResource(int x, int y, Resource* resource) {
        Tile* tile = getTile(x, y);
        if (tile && resource) {
            tile->addResource(resource);
        }
    }

    void Map::removeResource(int x, int y, Resource* resource) {
        Tile* tile = getTile(x, y);
        if (tile && resource) {
            tile->removeResource(resource);
        }
    }

    void Map::update() {
        // Update map state if needed
        // For example, resource regeneration timers
    }

    void Map::generateResources() {
        // TODO: Implement resource generation based on game rules
        // This should create new resources at random positions
        // following the specified resource density rules
    }
} 
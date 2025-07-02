#include "core/Map.hpp"
#include "core/Tile.hpp"
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace Zappy {
    Map::Map(int width, int height, bool infinite) 
        : width_(width)
        , height_(height)
        , infinite_(infinite) {
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

    void Map::normalizeCoordinates(int& x, int& y) const {
        if (!infinite_) {
            return;  // No normalization needed for finite maps
        }

        // Handle wrapping for infinite maps
        if (x < 0) {
            x = width_ - (std::abs(x) % width_);
        } else if (x >= width_) {
            x = x % width_;
        }

        if (y < 0) {
            y = height_ - (std::abs(y) % height_);
        } else if (y >= height_) {
            y = y % height_;
        }
    }

    bool Map::isValidCoordinate(int x, int y) const {
        if (infinite_) {
            return true;  // All coordinates are valid in infinite mode
        }
        return x >= 0 && x < width_ && y >= 0 && y < height_;
    }

    Tile* Map::getTile(int x, int y) {
        if (!isValidCoordinate(x, y)) {
            return nullptr;
        }
        normalizeCoordinates(x, y);
        return tiles_[y * width_ + x].get();
    }

    const Tile* Map::getTile(int x, int y) const {
        if (!isValidCoordinate(x, y)) {
            return nullptr;
        }
        int nx = x, ny = y;
        normalizeCoordinates(nx, ny);
        return tiles_[ny * width_ + nx].get();
    }

    void Map::expand(int newWidth, int newHeight) {
        if (newWidth <= width_ && newHeight <= height_) {
            return;  // No expansion needed
        }

        resizeTiles(newWidth, newHeight);
        width_ = newWidth;
        height_ = newHeight;
    }

    void Map::expandToInclude(int x, int y) {
        if (infinite_) {
            return;  // No expansion needed for infinite maps
        }

        int newWidth = std::max(width_, x + 1);
        int newHeight = std::max(height_, y + 1);
        expand(newWidth, newHeight);
    }

    void Map::resizeTiles(int newWidth, int newHeight) {
        std::vector<std::unique_ptr<Tile>> newTiles;
        newTiles.reserve(newWidth * newHeight);

        // Create new tiles
        for (int y = 0; y < newHeight; ++y) {
            for (int x = 0; x < newWidth; ++x) {
                if (x < width_ && y < height_) {
                    // Move existing tile
                    newTiles.push_back(std::move(tiles_[y * width_ + x]));
                } else {
                    // Create new tile
                    newTiles.push_back(std::make_unique<Tile>());
                }
            }
        }

        tiles_ = std::move(newTiles);
    }

    void Map::addResource(int x, int y, ResourceType type) {
        Tile* tile = getTile(x, y);
        if (tile) {
            tile->addResource(type);
        }
    }

    void Map::removeResource(int x, int y, ResourceType type) {
        Tile* tile = getTile(x, y);
        if (tile) {
            tile->removeResource(type);
        }
    }

    std::string Map::toMszString() const {
        return "msz " + std::to_string(width_) + " " + std::to_string(height_) + "\n";
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
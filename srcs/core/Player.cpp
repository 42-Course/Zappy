#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/Inventory.hpp"
#include <stdexcept>

namespace Zappy {
    Player::Player(int id, Team* team)
        : id_(id)
        , team_(team)
        , level_(1)
        , x_(0)
        , y_(0)
        , direction_(Direction::NORTH)
        , food_(10)  // Start with 10 food units
        , inventory_(std::make_unique<Inventory>()) {
        if (!team) {
            throw std::invalid_argument("Player must belong to a team");
        }
    }

    Player::~Player() {
        if (team_) {
            team_->removePlayer(this);
        }
    }

    void Player::setPosition(int x, int y) {
        // TODO: Add map bounds checking when World reference is available
        x_ = x;
        y_ = y;
    }

    void Player::setDirection(Direction dir) {
        direction_ = dir;
    }

    void Player::moveForward() {
        // Update position based on current direction
        switch (direction_) {
            case Direction::NORTH:
                y_--;
                break;
            case Direction::SOUTH:
                y_++;
                break;
            case Direction::EAST:
                x_++;
                break;
            case Direction::WEST:
                x_--;
                break;
        }
        // TODO: Add map wrapping when World reference is available
    }

    void Player::turnLeft() {
        switch (direction_) {
            case Direction::NORTH:
                direction_ = Direction::WEST;
                break;
            case Direction::WEST:
                direction_ = Direction::SOUTH;
                break;
            case Direction::SOUTH:
                direction_ = Direction::EAST;
                break;
            case Direction::EAST:
                direction_ = Direction::NORTH;
                break;
        }
    }

    void Player::turnRight() {
        switch (direction_) {
            case Direction::NORTH:
                direction_ = Direction::EAST;
                break;
            case Direction::EAST:
                direction_ = Direction::SOUTH;
                break;
            case Direction::SOUTH:
                direction_ = Direction::WEST;
                break;
            case Direction::WEST:
                direction_ = Direction::NORTH;
                break;
        }
    }

    void Player::look() {
        // TODO: Implement vision cone based on direction
        // This will need World reference to check tiles
    }

    void Player::inventory() {
        // TODO: Return inventory contents
        // This will be implemented once Inventory class is complete
    }

    void Player::take(Resource* resource) {
        if (!resource) return;
        // TODO: Add resource to inventory
        // This will be implemented once Resource and Inventory classes are complete
    }

    void Player::drop(Resource* resource) {
        if (!resource) return;
        // TODO: Remove resource from inventory and add to current tile
        // This will be implemented once Resource and Inventory classes are complete
    }

    void Player::broadcast(const std::string& message) {
        if (message.empty()) return;
        // TODO: Implement broadcast through World/NetworkManager
    }

    bool Player::startIncantation() {
        // TODO: Check elevation requirements and start incantation
        // This will need World reference to check tile resources and nearby players
        return false;
    }

    void Player::fork() {
        // TODO: Implement player reproduction
        // This will need World reference to create new player
    }

    void Player::update() {
        // Decrease food every tick
        if (food_ > 0) {
            food_--;
        }

        // TODO: Handle other time-based updates
        // - Check incantation progress
        // - Update action cooldowns
    }
} 
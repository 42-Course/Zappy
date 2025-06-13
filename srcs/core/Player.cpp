#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/Resource.hpp"
#include "core/Inventory.hpp"
#include <algorithm>

namespace Zappy {
    Player::Player(int id, Team& team, int x, int y, Direction direction)
        : id_(id)
        , team_(team)
        , x_(x)
        , y_(y)
        , direction_(direction)
        , level_(1)
        , inventory_()
        , alive_(true)
    {}

    Player::~Player() {
        // Notify observers that player is being destroyed
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerDied(this);
            }
        }
    }

    void Player::attach(Observer* observer) {
        if (observer) {
            observers_.push_back(observer);
        }
    }

    void Player::detach(Observer* observer) {
        if (observer) {
            observers_.erase(
                std::remove(observers_.begin(), observers_.end(), observer),
                observers_.end()
            );
        }
    }

    void Player::notifyMoved() const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerMoved(this);
            }
        }
    }

    void Player::notifyTurned() const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerTurned(this);
            }
        }
    }

    void Player::notifyLevelUp() const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerLevelUp(this);
            }
        }
    }

    void Player::notifyInventoryChanged() const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerInventoryChanged(this);
            }
        }
    }

    void Player::notifyDied() const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerDied(this);
            }
        }
    }

    void Player::setPosition(int x, int y) {
        if (!alive_) return;
        x_ = x;
        y_ = y;
        notifyMoved();
    }

    void Player::setDirection(Direction direction) {
        if (!alive_) return;
        direction_ = direction;
        notifyTurned();
    }

    void Player::levelUp() {
        if (!alive_) return;
        level_++;
        notifyLevelUp();
    }

    void Player::addResource(ResourceType type, int amount) {
        if (!alive_ || amount <= 0) return;
        
        // Create a single resource and add it multiple times
        Resource resource(type);
        for (int i = 0; i < amount; i++) {
            if (!inventory_.add(&resource)) {
                break; // Stop if we can't add more
            }
        }
        notifyInventoryChanged();
    }

    void Player::removeResource(ResourceType type, int amount) {
        if (!alive_ || amount <= 0) return;
        
        // Create a single resource and remove it multiple times
        Resource resource(type);
        for (int i = 0; i < amount; i++) {
            if (!inventory_.remove(&resource)) {
                break; // Stop if we can't remove more
            }
        }
        notifyInventoryChanged();
    }

    void Player::setInventory(const Inventory& inventory) {
        if (!alive_) return;
        inventory_ = inventory;
        notifyInventoryChanged();
    }

    void Player::die() {
        if (!alive_) return;
        alive_ = false;
        notifyDied();
    }

    // void Player::look() {
    //     // TODO: Implement vision cone based on direction
    //     // This will need World reference to check tiles
    // }
    
    void Player::take(Resource* resource) {
        if (!alive_ || !resource) return;
        
        if (inventory_.add(resource)) {
            notifyInventoryChanged();
        }
    }

    void Player::drop(Resource* resource) {
        if (!alive_ || !resource) return;
        
        if (inventory_.remove(resource)) {
            notifyInventoryChanged();
        }
    }

    void Player::broadcast(const std::string& message) {
        if (!alive_ || message.empty()) return;
        
        // TODO: Implement broadcast through World/NetworkManager
        // - Send message to all players in range
        // - Handle message routing
    }

    bool Player::startIncantation() {
        if (!alive_) return false;
        
        // TODO: Implement elevation requirements check
        // - Check if player has required resources
        // - Check if other players are present and at correct level
        // - Check if tile has required resources
        return false;
    }

    void Player::fork() {
        if (!alive_) return;
        
        // TODO: Implement player reproduction
        // - Create new player in same team
        // - Place on same tile
        // - Initialize with basic resources
    }

    void Player::update() {
        if (!alive_) return;
        
        // TODO: Implement time-based updates
        // - Check food level
        // - Update action cooldowns
        // - Check for level up conditions
        // - Handle ongoing actions
    }
} 
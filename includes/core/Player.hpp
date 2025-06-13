#pragma once

#include <string>
#include <vector>
#include "Resource.hpp"
#include "Inventory.hpp"
#include "Observer.hpp"

namespace Zappy {
    class Team;  // Forward declaration

    enum class Direction {
        NORTH = 1,
        EAST = 2,
        SOUTH = 3,
        WEST = 4
    };

    class Player {
    public:
        Player(int id, Team& team, int x, int y, Direction direction);
        ~Player();

        // Getters (all const)
        int getId() const { return id_; }
        const Team& getTeam() const { return team_; }
        int getX() const { return x_; }
        int getY() const { return y_; }
        Direction getDirection() const { return direction_; }
        int getLevel() const { return level_; }
        const Inventory& getInventory() const { return inventory_; }
        bool isAlive() const { return alive_; }

        // Movement
        void setPosition(int x, int y);
        void setDirection(Direction direction);

        // Level management
        void levelUp();

        // Inventory management
        void addResource(ResourceType type, int amount);
        void removeResource(ResourceType type, int amount);
        void setInventory(const Inventory& inventory);
        void take(Resource* resource);
        void drop(Resource* resource);

        // Game mechanics
        bool startIncantation();
        void fork();
        void broadcast(const std::string& message);

        // Life management
        void die();

        // State updates
        void update();

        // Observer pattern
        void attach(Observer* observer);
        void detach(Observer* observer);

    protected:
        void notifyMoved() const;
        void notifyTurned() const;
        void notifyLevelUp() const;
        void notifyInventoryChanged() const;
        void notifyDied() const;

    private:
        int id_;
        Team& team_;  // Reference to team, owned by World
        int x_;
        int y_;
        Direction direction_;
        int level_;
        Inventory inventory_;
        bool alive_;
        std::vector<Observer*> observers_;  // Non-owning pointers
    };
} 
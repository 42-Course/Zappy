#pragma once

#include <string>
#include <vector>
#include <queue>
#include "commands/ICommand.hpp"
#include "Resource.hpp"
#include "Inventory.hpp"
#include "IObserver.hpp"
#include "Subject.hpp"

namespace Zappy {
    class Team;  // Forward declaration

    enum class Direction {
        NORTH = 1,
        EAST = 2,
        SOUTH = 3,
        WEST = 4
    };

    class Player : public Subject {
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
        std::pair<int, int> getPosition() const;
        void setPosition(int x, int y);
        void setDirection(Direction direction);

        // Level management
        void levelUp();

        // Inventory management
        void addResource(ResourceType type, int amount = 1);
        bool removeResource(ResourceType type, int amount = 1);

        // Game mechanics
        bool startIncantation();
        void fork();
        void broadcast(const std::string& message);

        // Command handling
        bool canQueueCommand() const { return commandQueue_.size() < 10; }
        void enqueueCommand(std::unique_ptr<ICommand> command);

        // Life management
        void die();

        // State updates
        void update();

        // Serialization helpers for network responses
        std::string toPnwString() const;  // Player new
        std::string toPpoString() const;  // Position
        std::string toPlvString() const;  // Level
        std::string toPinString() const;  // Inventory
        std::string toPexString() const;  // Expel
        std::string toPbcString(const std::string& message) const;  // Broadcast

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
        int ticksSinceLastFood_;
        int currentCommandTicks_;
        std::queue<std::unique_ptr<ICommand>> commandQueue_;
    };
} 
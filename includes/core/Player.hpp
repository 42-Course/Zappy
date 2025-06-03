#pragma once

#include <string>
#include <memory>
#include <vector>
#include "Direction.hpp"

namespace Zappy {
    class Team;
    class Resource;
    class Inventory;

    class Player {
    public:
        Player(int id, Team* team);
        ~Player();

        // Basic info
        int getId() const { return id_; }
        Team* getTeam() const { return team_; }
        int getLevel() const { return level_; }
        
        // Position and orientation
        int getX() const { return x_; }
        int getY() const { return y_; }
        Direction getDirection() const { return direction_; }
        void setPosition(int x, int y);
        void setDirection(Direction dir);
        
        // Movement
        void moveForward();
        void turnLeft();
        void turnRight();
        
        // Actions
        void look();
        void inventory();
        void take(Resource* resource);
        void drop(Resource* resource);
        void broadcast(const std::string& message);
        bool startIncantation();
        void fork();
        
        // State
        bool isAlive() const { return food_ > 0; }
        void update();

    private:
        int id_;
        Team* team_;  // Non-owning pointer, team owned by World
        int level_;
        int x_, y_;
        Direction direction_;
        int food_;
        std::unique_ptr<Inventory> inventory_;
    };
} 
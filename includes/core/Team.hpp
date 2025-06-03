#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Zappy {
    class Player;

    class Team {
    public:
        Team(const std::string& name, int maxPlayers);
        ~Team();

        // Team info
        const std::string& getName() const { return name_; }
        int getMaxPlayers() const { return maxPlayers_; }
        int getCurrentPlayers() const { return currentPlayers_; }
        
        // Player management
        bool canAddPlayer() const { return currentPlayers_ < maxPlayers_; }
        void addPlayer(Player* player);
        void removePlayer(Player* player);
        
        // Team state
        bool hasWon() const;
        void update();

    private:
        std::string name_;
        int maxPlayers_;
        int currentPlayers_;
        std::vector<Player*> players_; // Non-owning pointers, players owned by World
    };
} 
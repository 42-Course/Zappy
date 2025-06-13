#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Observer.hpp"

namespace Zappy {
    class Player;

    class Team {
    public:
        Team(const std::string& name, int maxPlayers);
        ~Team();

        // Basic info
        const std::string& getName() const { return name_; }
        int getMaxPlayers() const { return maxPlayers_; }
        int getCurrentPlayers() const { return players_.size(); }
        bool canAddPlayer() const { return players_.size() < maxPlayers_; }
        
        // Player management
        void addPlayer(Player* player);
        void removePlayer(Player* player);
        void removePlayer(Player* player) const;  // Const version for const Team references
        const std::vector<Player*>& getPlayers() const { return players_; }
        
        // Observer pattern
        void attach(Observer* observer);
        void detach(Observer* observer);

        // Game mechanics
        bool hasWon() const;
        void update();

    protected:
        void notifyPlayerAdded(const Player* player) const;
        void notifyPlayerRemoved(const Player* player) const;
        void notifyWinCondition() const;

    private:
        std::string name_;
        size_t maxPlayers_;
        mutable std::vector<Player*> players_;  // Non-owning pointers, players owned by World
        std::vector<Observer*> observers_;  // Non-owning pointers
    };
} 
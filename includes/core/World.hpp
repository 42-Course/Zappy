#pragma once

#include <memory>
#include <vector>
#include <map>
#include "Map.hpp"
#include "Team.hpp"
#include "Player.hpp"

namespace Zappy {
    class World {
    public:
        World(int width, int height);
        ~World();

        // Map operations
        const Map& getMap() const { return *map_; }
        Map& getMap() { return *map_; }
        
        // Team operations
        bool addTeam(const std::string& name, int maxPlayers);
        Team* getTeam(const std::string& name);
        const std::vector<Team*>& getTeams() const { return teams_; }
        
        // Player operations
        Player* createPlayer(const std::string& teamName);
        Player* getPlayer(int id);
        void removePlayer(int id);
        const std::map<int, std::unique_ptr<Player>>& getPlayers() const { return players_; }

        // World state updates
        void update();
        
    private:
        std::unique_ptr<Map> map_;
        std::vector<Team*> teams_;
        std::map<int, std::unique_ptr<Player>> players_;
        int nextPlayerId_;
    };
} 
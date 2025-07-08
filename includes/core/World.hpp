#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "core/Map.hpp"
#include "core/Player.hpp"
#include "core/Team.hpp"

namespace Zappy {
    class World {
    public:
        World(int width, int height, bool infiniteMap = false);
        ~World() = default;

        // Map operations
        Map& getMap() const { return *map_; }
        Map& getMap() { return *map_; }
        bool isInfiniteMap() const { return map_->isInfinite(); }
        void setInfiniteMap(bool infinite) { map_->setInfinite(infinite); }
        
        // Team operations
        bool addTeam(const std::string& name, int maxPlayers);
        Team* getTeam(const std::string& name);
        const std::vector<std::unique_ptr<Team>>& getTeams() const { return teams_; }
        
        // Player operations
        Player* createPlayer(const std::string& teamName);
        Player* getPlayer(int id);
        void removePlayer(int id);
        const std::map<int, std::unique_ptr<Player>>& getPlayers() const { return players_; }

        // World state updates
        void update();
        
    private:
        std::unique_ptr<Map> map_;
        std::vector<std::unique_ptr<Team>> teams_;
        std::map<int, std::unique_ptr<Player>> players_;
        int nextPlayerId_;

    };
} 
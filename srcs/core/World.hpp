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
        ~World();

        // Team management
        bool addTeam(const std::string& name, int maxPlayers);
        Team* getTeam(const std::string& name);
        const std::vector<std::unique_ptr<Team>>& getTeams() const { return teams_; }

        // Player management
        Player* createPlayer(const std::string& teamName);
        Player* getPlayer(int id);
        void removePlayer(int id);
        const std::map<int, std::unique_ptr<Player>>& getPlayers() const { return players_; }

        // Map access
        Map* getMap() { return map_.get(); }
        const Map* getMap() const { return map_.get(); }

        // Game loop
        void update();

        // Callback setters
        using PlayerUpdateCallback = std::function<void(int, const Player*)>;
        using TeamUpdateCallback = std::function<void(const std::string&, const Team*)>;
        using MapUpdateCallback = std::function<void(int, int, const Tile*)>;

        void setPlayerUpdateCallback(PlayerUpdateCallback callback) { playerUpdateCallback_ = std::move(callback); }
        void setTeamUpdateCallback(TeamUpdateCallback callback) { teamUpdateCallback_ = std::move(callback); }
        void setMapUpdateCallback(MapUpdateCallback callback) { mapUpdateCallback_ = std::move(callback); }

    private:
        void notifyPlayerUpdate(int playerId, const Player* player);
        void notifyTeamUpdate(const std::string& teamName, const Team* team);
        void notifyMapUpdate(int x, int y, const Tile* tile);

        std::unique_ptr<Map> map_;
        std::vector<std::unique_ptr<Team>> teams_;
        std::map<int, std::unique_ptr<Player>> players_;
        int nextPlayerId_;

        PlayerUpdateCallback playerUpdateCallback_;
        TeamUpdateCallback teamUpdateCallback_;
        MapUpdateCallback mapUpdateCallback_;
    };
} 
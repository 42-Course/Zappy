#pragma once

#include <string>
#include <memory>

namespace Zappy {
    class World;

    class Spectator {
    public:
        explicit Spectator(int id);
        ~Spectator();

        // Basic info
        int getId() const { return id_; }
        
        // World observation
        void setWorld(World* world) { world_ = world; }
        World* getWorld() const { return world_; }
        
        // Map queries
        std::string getMapSize() const;
        std::string getTileContent(int x, int y) const;
        std::string getMapContent() const;
        
        // Team queries
        std::string getTeamNames() const;
        
        // Player queries
        std::string getPlayers() const;
        std::string getPlayerPosition(int playerId) const;
        std::string getPlayerLevel(int playerId) const;
        std::string getPlayerInventory(int playerId) const;
        
        // Time unit queries
        std::string getTimeUnit() const;
        bool setTimeUnit(int timeUnit);

    private:
        int id_;
        World* world_;  // Non-owning pointer, world owned by Engine
    };
} 
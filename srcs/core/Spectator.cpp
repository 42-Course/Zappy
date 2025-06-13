#include "core/Spectator.hpp"
#include "core/World.hpp"
#include "core/Map.hpp"
#include "core/Team.hpp"
#include "core/Player.hpp"
#include <sstream>

namespace Zappy {
    Spectator::Spectator(int id) : id_(id), world_(nullptr) {}
    
    Spectator::~Spectator() = default;
    
    std::string Spectator::getMapSize() const {
        if (!world_) return "msz -1 -1\n";
        const auto& map = world_->getMap();
        return "msz " + std::to_string(map.getWidth()) + " " + std::to_string(map.getHeight()) + "\n";
    }
    
    std::string Spectator::getTileContent(int x, int y) const {
        if (!world_) return "bct " + std::to_string(x) + " " + std::to_string(y) + " 0 0 0 0 0 0 0\n";
        // TODO: Implement actual resource counting from tile
        return "bct " + std::to_string(x) + " " + std::to_string(y) + " 0 0 0 0 0 0 0\n";
    }
    
    std::string Spectator::getMapContent() const {
        if (!world_) return "mct\n";
        std::stringstream ss;
        const auto& map = world_->getMap();
        for (int y = 0; y < map.getHeight(); ++y) {
            for (int x = 0; x < map.getWidth(); ++x) {
                ss << getTileContent(x, y);
            }
        }
        return ss.str();
    }
    
    std::string Spectator::getTeamNames() const {
        if (!world_) return "tna\n";
        std::stringstream ss;
        ss << "tna";
        for (const auto& team : world_->getTeams()) {
            ss << " " << team->getName();
        }
        ss << "\n";
        return ss.str();
    }
    
    std::string Spectator::getPlayers() const {
        if (!world_) return "pls\n";
        std::stringstream ss;
        ss << "pls";
        for (const auto& [id, player] : world_->getPlayers()) {
            ss << " " << id;
        }
        ss << "\n";
        return ss.str();
    }
    
    std::string Spectator::getPlayerPosition(int playerId) const {
        if (!world_) return "ppo " + std::to_string(playerId) + " -1 -1 0\n";
        const auto* player = world_->getPlayer(playerId);
        if (!player) return "ppo " + std::to_string(playerId) + " -1 -1 0\n";
        return "ppo " + std::to_string(playerId) + " " + 
               std::to_string(player->getX()) + " " + 
               std::to_string(player->getY()) + " " + 
               std::to_string(static_cast<int>(player->getDirection())) + "\n";
    }
    
    std::string Spectator::getPlayerLevel(int playerId) const {
        if (!world_) return "plv " + std::to_string(playerId) + " 0\n";
        const auto* player = world_->getPlayer(playerId);
        if (!player) return "plv " + std::to_string(playerId) + " 0\n";
        return "plv " + std::to_string(playerId) + " " + std::to_string(player->getLevel()) + "\n";
    }
    
    std::string Spectator::getPlayerInventory(int playerId) const {
        if (!world_) return "pin " + std::to_string(playerId) + " -1 -1 0 0 0 0 0 0 0\n";
        const auto* player = world_->getPlayer(playerId);
        if (!player) return "pin " + std::to_string(playerId) + " -1 -1 0 0 0 0 0 0 0\n";
        // TODO: Implement actual inventory querying
        return "pin " + std::to_string(playerId) + " " + 
               std::to_string(player->getX()) + " " + 
               std::to_string(player->getY()) + " 0 0 0 0 0 0 0\n";
    }
    
    std::string Spectator::getTimeUnit() const {
        // TODO: Implement time unit querying from GameLoop
        return "sgt 100\n";
    }
    
    bool Spectator::setTimeUnit(int timeUnit) {
        if (timeUnit <= 0) return false;
        // TODO: Implement time unit setting in GameLoop
        return true;
    }
} 
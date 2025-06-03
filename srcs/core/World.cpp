#include "core/World.hpp"
#include <stdexcept>

namespace Zappy {
    World::World(int width, int height)
        : map_(std::make_unique<Map>(width, height))
        , nextPlayerId_(1) {
    }

    World::~World() {
        // Clean up teams
        for (auto team : teams_) {
            delete team;
        }
        teams_.clear();
    }

    bool World::addTeam(const std::string& name, int maxPlayers) {
        // Check if team already exists
        for (const auto& team : teams_) {
            if (team->getName() == name) {
                return false;
            }
        }
        teams_.push_back(new Team(name, maxPlayers));
        return true;
    }

    Team* World::getTeam(const std::string& name) {
        for (auto team : teams_) {
            if (team->getName() == name) {
                return team;
            }
        }
        return nullptr;
    }

    Player* World::createPlayer(const std::string& teamName) {
        Team* team = getTeam(teamName);
        if (!team) {
            return nullptr;
        }

        // Create new player
        auto player = std::make_unique<Player>(nextPlayerId_++, team);
        Player* playerPtr = player.get();
        
        // Add to players map
        players_[playerPtr->getId()] = std::move(player);
        
        return playerPtr;
    }

    Player* World::getPlayer(int id) {
        auto it = players_.find(id);
        return it != players_.end() ? it->second.get() : nullptr;
    }

    void World::removePlayer(int id) {
        players_.erase(id);
    }

    void World::update() {
        // Update map resources
        map_->update();

        // Update all players
        for (auto& [id, player] : players_) {
            player->update();
        }
    }
} 
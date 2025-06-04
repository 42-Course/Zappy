#include "core/Team.hpp"
#include "core/Player.hpp"
#include <algorithm>
#include <stdexcept>

namespace Zappy {
    Team::Team(const std::string& name, int maxPlayers)
        : name_(name)
        , maxPlayers_(maxPlayers)
        , currentPlayers_(0) {
        if (maxPlayers <= 0) {
            throw std::invalid_argument("Max players must be positive");
        }
    }

    Team::~Team() = default;

    void Team::addPlayer(Player* player) {
        if (!player) return;
        
        // Check if player is already in team
        if (std::find(players_.begin(), players_.end(), player) != players_.end()) {
            return;
        }

        // Check if team is full
        if (currentPlayers_ >= maxPlayers_) {
            throw std::runtime_error("Team is full");
        }

        players_.push_back(player);
        currentPlayers_++;
    }

    void Team::removePlayer(Player* player) {
        if (!player) return;

        auto it = std::find(players_.begin(), players_.end(), player);
        if (it != players_.end()) {
            players_.erase(it);
            currentPlayers_--;
        }
    }

    bool Team::hasWon() const {
        // A team wins when it has 6 players at level 8
        int levelEightPlayers = 0;
        for (const auto* player : players_) {
            if (player->getLevel() >= 8) {
                levelEightPlayers++;
                if (levelEightPlayers >= 6) {
                    return true;
                }
            }
        }
        return false;
    }

    void Team::update() {
        // Remove any disconnected players
        auto it = std::remove_if(players_.begin(), players_.end(),
            [](const Player* player) {
                return !player->isAlive();
            });
        
        if (it != players_.end()) {
            currentPlayers_ -= std::distance(it, players_.end());
            players_.erase(it, players_.end());
        }

        // Check win condition
        if (hasWon()) {
            // TODO: Notify game engine of win condition
        }
    }
} 
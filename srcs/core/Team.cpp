#include "core/Team.hpp"
#include "core/Player.hpp"
#include <algorithm>
#include <stdexcept>

namespace Zappy {
    Team::Team(const std::string& name, int maxPlayers)
        : name_(name)
        , maxPlayers_(maxPlayers) {
        if (maxPlayers <= 0) {
            throw std::invalid_argument("Max players must be positive");
        }
    }

    Team::~Team() {

    }

    void Team::notifyPlayerAdded(const Player* player) const {
        notify([player](IObserver* obs) {
            obs->onPlayerAdded(player);
        });
    }

    void Team::notifyPlayerRemoved(const Player* player) const {
        notify([player](IObserver* obs) {
            obs->onPlayerRemoved(player);
        });
    }

    void Team::notifyWinCondition() const {
        notify([this](IObserver* obs) {
            obs->onTeamWon(this);
        });
    }

    void Team::addPlayer(Player* player) {
        if (player && canAddPlayer()) {
            players_.push_back(player);
            notifyPlayerAdded(player);
        }
    }

    void Team::removePlayer(Player* player) {
        if (player) {
            players_.erase(
                std::remove(players_.begin(), players_.end(), player),
                players_.end()
            );
            notifyPlayerRemoved(player);
        }
    }

    void Team::removePlayer(Player* player) const {
        if (player) {
            players_.erase(
                std::remove(players_.begin(), players_.end(), player),
                players_.end()
            );
            notifyPlayerRemoved(player);
        }
    }

    bool Team::hasWon() const {
        // A team wins when it has 6 players at level 8
        int levelEightPlayers = 0;
        for (const auto* player : players_) {
            if (player && player->getLevel() >= 8) {
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
                return player && !player->isAlive();
            });
        
        if (it != players_.end()) {
            players_.erase(it, players_.end());
        }

        // TODO: notify player removed

        // Check win condition
        if (hasWon()) {
            notifyWinCondition();
        }
    }
} 
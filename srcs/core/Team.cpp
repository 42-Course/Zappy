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
        // Notify observers that team is being destroyed
        for (auto observer : observers_) {
            if (observer) {
                // Notify about each player being removed
                for (const auto* player : players_) {
                    if (player) {
                        observer->onPlayerRemoved(player);
                    }
                }
            }
        }
    }

    void Team::attach(Observer* observer) {
        if (observer) {
            observers_.push_back(observer);
        }
    }

    void Team::detach(Observer* observer) {
        if (observer) {
            observers_.erase(
                std::remove(observers_.begin(), observers_.end(), observer),
                observers_.end()
            );
        }
    }

    void Team::notifyPlayerAdded(const Player* player) const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerAdded(player);
            }
        }
    }

    void Team::notifyPlayerRemoved(const Player* player) const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onPlayerRemoved(player);
            }
        }
    }

    void Team::notifyWinCondition() const {
        for (auto observer : observers_) {
            if (observer) {
                observer->onTeamWon(this);
            }
        }
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

        // Check win condition
        if (hasWon()) {
            notifyWinCondition();
        }
    }
} 
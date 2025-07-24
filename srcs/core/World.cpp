#include "core/World.hpp"
#include <stdexcept>

namespace Zappy {
World::World(int width, int height, bool infiniteMap) : nextPlayerId_(1) {
  map_ = std::make_unique<Map>(width, height, infiniteMap);
}

bool World::addTeam(const std::string &name, int maxPlayers) {
  if (getTeam(name)) {
    return false; // Team already exists
  }

  auto team = std::make_unique<Team>(name, maxPlayers);
  teams_.push_back(std::move(team));
  return true;
}

Team *World::getTeam(const std::string &name) {
  for (const auto &team : teams_) {
    if (team->getName() == name) {
      return team.get();
    }
  }
  return nullptr;
}

Player *World::createPlayer(const std::string &teamName) {
  Team *team = getTeam(teamName);
  if (!team) {
    return nullptr;
  }

  if (!team->canAddPlayer()) {
    return nullptr;
  }

  // Get a random starting position
  int x               = rand() % map_->getWidth();
  int y               = rand() % map_->getHeight();
  Direction direction = static_cast<Direction>((rand() % 4) + 1); // Random direction

  auto player                  = std::make_unique<Player>(nextPlayerId_++, *team, x, y, direction);
  Player *playerPtr            = player.get();
  players_[playerPtr->getId()] = std::move(player);
  team->addPlayer(playerPtr);

  return playerPtr;
}

Player *World::getPlayer(int id) {
  auto it = players_.find(id);
  return it != players_.end() ? it->second.get() : nullptr;
}

void World::removePlayer(int id) {
  auto it = players_.find(id);
  if (it != players_.end()) {
    Player *player = it->second.get();
    player->getTeam().removePlayer(player);
    players_.erase(it);
  }
}

void World::update() {
  // Update all game entities
  for (auto &[id, player] : players_) {
    player->update();
  }

  for (auto &team : teams_) {
    team->update();
  }

  map_->update();
}
} // namespace Zappy
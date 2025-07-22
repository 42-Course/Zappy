#include "services/SpectatorBroadcaster.hpp"
#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/Tile.hpp"
#include <sstream>

namespace Zappy {

    SpectatorBroadcaster::SpectatorBroadcaster(const ClientManager& clients)
        : clientManager_(clients) {}

    void SpectatorBroadcaster::broadcast(const std::string& message, bool add_to_history) {
        for (const auto& [fd, client] : clientManager_.getClients()) {
            if (client->getType() == ClientConnection::Type::Spectator) {
                client->sendData(message);
            }
        }
        if (add_to_history)
            history_.push_back(message);
    }

    const std::vector<std::string> SpectatorBroadcaster::getHistory() const {
        return history_;
    }

    void SpectatorBroadcaster::onPlayerMoved(const Player* player) {
        if (!player) return;
        std::string msg = "ppo " + std::to_string(player->getId()) + " " +
                          std::to_string(player->getX()) + " " +
                          std::to_string(player->getY()) + " " +
                          std::to_string(static_cast<int>(player->getDirection())) + "\n";
        broadcast(msg, true);
    }

    void SpectatorBroadcaster::onPlayerTurned(const Player* player) {
        onPlayerMoved(player);
    }

    void SpectatorBroadcaster::onPlayerLevelUp(const Player* player) {
        if (!player) return;
        std::string msg = "plv " + std::to_string(player->getId()) + " " +
                          std::to_string(player->getLevel()) + "\n";
        broadcast(msg, true);
    }

    void SpectatorBroadcaster::onPlayerInventoryChanged(const Player* player) {
        if (!player) return;
        const auto& inv = player->getInventory();
        std::string msg = "pin " + std::to_string(player->getId()) + " " +
                          std::to_string(player->getX()) + " " +
                          std::to_string(player->getY()) + " " +
                          std::to_string(inv.getFood()) + " " +
                          std::to_string(inv.getLinemate()) + " " +
                          std::to_string(inv.getDeraumere()) + " " +
                          std::to_string(inv.getSibur()) + " " +
                          std::to_string(inv.getMendiane()) + " " +
                          std::to_string(inv.getPhiras()) + " " +
                          std::to_string(inv.getThystame()) + "\n";
        broadcast(msg, true);
    }

    void SpectatorBroadcaster::onPlayerDied(const Player* player) {
        if (!player) return;
        broadcast("pdi " + std::to_string(player->getId()) + "\n", true);
    }

    void SpectatorBroadcaster::onPlayerAdded(const Player* player) {
        if (!player) return;
        std::stringstream ss;
        ss << "pnw " << player->getId() << " "
           << player->getX() << " "
           << player->getY() << " "
           << static_cast<int>(player->getDirection()) << " "
           << player->getLevel() << " "
           << player->getTeam().getName() << "\n";
        broadcast(ss.str(), true);
    }

    void SpectatorBroadcaster::onPlayerRemoved(const Player* player) {
        if (!player) return;
        broadcast("pdi " + std::to_string(player->getId()) + "\n", true);
    }

    void SpectatorBroadcaster::onTeamWon(const Team* team) {
        if (!team) return;
        broadcast("seg " + team->getName() + "\n", true);
    }

    std::string SpectatorBroadcaster::formatTileMessage(const Tile* tile, int x, int y) {
        if (!tile) return "";
        std::stringstream ss;
        ss << "bct " << x << " " << y << " "
           << tile->getResourceCount(ResourceType::FOOD) << " "
           << tile->getResourceCount(ResourceType::LINEMATE) << " "
           << tile->getResourceCount(ResourceType::DERAUMERE) << " "
           << tile->getResourceCount(ResourceType::SIBUR) << " "
           << tile->getResourceCount(ResourceType::MENDIANE) << " "
           << tile->getResourceCount(ResourceType::PHIRAS) << " "
           << tile->getResourceCount(ResourceType::THYSTAME) << "\n";
        return ss.str();
    }

    void SpectatorBroadcaster::onResourceAdded(const Tile* tile, ResourceType) {
        broadcast(formatTileMessage(tile, 0, 0), true); // TODO: get actual x/y
    }

    void SpectatorBroadcaster::onResourceRemoved(const Tile* tile, ResourceType) {
        broadcast(formatTileMessage(tile, 0, 0), true); // TODO: get actual x/y
    }
}

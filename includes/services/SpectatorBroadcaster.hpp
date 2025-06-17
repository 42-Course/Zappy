#pragma once

#include "core/IObserver.hpp"
#include "net/ClientManager.hpp"
#include <string>

namespace Zappy {
    class SpectatorBroadcaster : public IObserver {
    public:
        explicit SpectatorBroadcaster(const ClientManager& clients);

        // IObserver overrides
        void onPlayerMoved(const Player* player) override;
        void onPlayerTurned(const Player* player) override;
        void onPlayerLevelUp(const Player* player) override;
        void onPlayerInventoryChanged(const Player* player) override;
        void onPlayerDied(const Player* player) override;
        void onPlayerAdded(const Player* player) override;
        void onPlayerRemoved(const Player* player) override;
        void onTeamWon(const Team* team) override;
        void onResourceAdded(const Tile* tile, ResourceType type) override;
        void onResourceRemoved(const Tile* tile, ResourceType type) override;
        void broadcast(const std::string& message);

    private:
        const ClientManager& clientManager_; // Owned by NetworkManager
        std::string formatTileMessage(const Tile* tile, int x, int y);
    };
}

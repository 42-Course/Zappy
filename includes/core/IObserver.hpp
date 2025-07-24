#pragma once
#include "core/Resource.hpp"

namespace Zappy {
    class Player;
    class Tile;
    class Team;

    class IObserver {
    public:
        virtual ~IObserver() = default;

        // Player notifications
        virtual void onPlayerMoved(const Player* player) = 0;
        virtual void onPlayerTurned(const Player* player) = 0;
        virtual void onPlayerLevelUp(const Player* player) = 0;
        virtual void onPlayerInventoryChanged(const Player* player) = 0;
        virtual void onPlayerDied(const Player* player) = 0;
        virtual void onPlayerDropedResource(const Player* player, ResourceType type) = 0;
        virtual void onPlayerTookResource(const Player* player, ResourceType type) = 0;

        // Team notifications
        virtual void onPlayerAdded(const Player* player) = 0;
        virtual void onPlayerRemoved(const Player* player) = 0;
        virtual void onTeamWon(const Team* team) = 0;

        // Tile notifications
        virtual void onResourceAdded(const Tile* tile, ResourceType type) = 0;
        virtual void onResourceRemoved(const Tile* tile, ResourceType type) = 0;
    };
} 
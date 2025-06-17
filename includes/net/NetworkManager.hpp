#pragma once

#include <memory>
#include <map>
#include <vector>
#include <set>
#include <sys/epoll.h>
#include "net/Socket.hpp"
#include "ClientConnection.hpp"
#include "CommandRouter.hpp"
#include "EventLoop.hpp"
#include "ClientManager.hpp"
#include "commands/Command.hpp"
#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/Tile.hpp"
#include "core/IObserver.hpp"
#include "core/World.hpp"

namespace Zappy {
    class NetworkManager : public IObserver {
    public:
        NetworkManager(World& world, int playerPort, int spectatorPort);
        ~NetworkManager() = default;

        void start();
        void stop();
        void update();

        // Client Manager
        const ClientManager& getClientManager() const { return clientManager_; }
        size_t connectedClientsSize() const;

        // Port getters
        int getPlayerPort() const { return playerPort_; }
        int getSpectatorPort() const { return spectatorPort_; }
        
        // Socket fd getters
        int getPlayerServerFd() const { return playerSocket_.getFd(); }
        int getSpectatorServerFd() const { return spectatorSocket_.getFd(); }

        // Command handling
        void registerCommandHandler(const std::string& command, CommandHandler handler);
        void registerServerCommandHandler(const std::string& command, CommandHandler handler);
        // void handleCommand(const std::string& command, ClientConnection* client);

        // Command introspection
        std::vector<std::string> getServerCommandNames() const;
        std::unique_ptr<Command> createServerCommand(const std::string& command, 
                                                   const std::vector<std::string>& tokens = {},
                                                   ClientConnection* client = nullptr) const;

        // Observer interface implementation
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

    private:
        void acceptNewConnection(int serverFd, ClientConnection::Type type);
        void handleClientEvent(int clientFd, uint32_t events);
        void handleStdinCommand();
        void cleanup();
        void broadcastToSpectators(const std::string& message);
        void sendInitialStateToSpectator(ClientConnection* spectator);

        World& world_;  // Reference to World
        Socket playerSocket_;
        Socket spectatorSocket_;
        int playerPort_;
        int spectatorPort_;
        std::unique_ptr<CommandRouter> commandRouter_;
        std::unique_ptr<CommandRouter> serverCommandRouter_;
        


        EventLoop eventLoop_;
        ClientManager clientManager_;
        // Socket playerSocket_;
        // Socket spectatorSocket_;

        static const int MAX_EVENTS = 64;
        bool running_;
    };
} 
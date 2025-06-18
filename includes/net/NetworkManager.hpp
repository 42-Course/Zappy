#pragma once

#include <memory>
#include <map>
#include <vector>
#include <set>
#include <sys/epoll.h>
#include "net/Socket.hpp"
#include "ClientConnection.hpp"
#include "EventLoop.hpp"
#include "ClientManager.hpp"
#include "commands/ICommand.hpp"
#include "commands/Command.hpp"
#include "commands/CommandDispatcher.hpp"
#include "services/SpectatorBroadcaster.hpp"
#include "core/GameLoop.hpp"
#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/Tile.hpp"
#include "core/World.hpp"

namespace Zappy {
    class NetworkManager {
    public:
        NetworkManager(World& world, GameLoop& gameLoop, int playerPort, int spectatorPort);
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
        void registerPlayerCommand(const std::string& command, CommandHandler handler);
        void registerSpectatorCommand(const std::string& command, CommandHandler handler);
        void registerServerCommand(const std::string& command, CommandHandler handler);

        // Command introspection
        std::vector<std::string> getServerCommandNames() const;
        std::unique_ptr<ICommand> createServerCommand(const std::string& command, 
                                                   const std::vector<std::string>& tokens = {},
                                                   ClientConnection* client = nullptr) const;


    private:
        void acceptNewConnection(int serverFd, ClientConnection::Type type);
        void handleClientEvent(int clientFd, uint32_t events);
        void handleStdinCommand();
        void cleanup();
        void sendInitialStateToSpectator(ClientConnection* spectator);
        bool tryRegisterPlayer(ClientConnection* client, const std::string& teamName);
        
        World& world_;  // Reference to World
        GameLoop& gameLoop_;  // Reference to GameLoop
        Socket playerSocket_;
        Socket spectatorSocket_;
        int playerPort_;
        int spectatorPort_;

        EventLoop eventLoop_;
        ClientManager clientManager_;
        SpectatorBroadcaster broadcaster_;
        CommandDispatcher playerDispatcher_;
        CommandDispatcher spectatorDispatcher_;
        CommandDispatcher serverDispatcher_; 

        static const int MAX_EVENTS = 64;
        bool running_;
    };
} 
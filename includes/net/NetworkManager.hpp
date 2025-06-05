#pragma once

#include <memory>
#include <map>
#include <vector>
#include <sys/epoll.h>
#include "net/Socket.hpp"
#include "ClientConnection.hpp"
#include "CommandRouter.hpp"

namespace Zappy {
    class NetworkManager {
    public:
        NetworkManager(int playerPort, int spectatorPort);
        ~NetworkManager();

        void start();
        void stop();
        void update();

        // Connection management
        void addClient(std::unique_ptr<ClientConnection> client);
        void removeClient(int clientId);
        ClientConnection* getClient(int clientId);
        const ClientConnection* getClient(int clientId) const;
        const std::map<int, std::unique_ptr<ClientConnection>>& getClients() const { return clients_; }

        // Port getters
        int getPlayerPort() const { return playerPort_; }
        int getSpectatorPort() const { return spectatorPort_; }
        
        // Socket fd getters
        int getPlayerServerFd() const { return playerSocket_.getFd(); }
        int getSpectatorServerFd() const { return spectatorSocket_.getFd(); }

        // Command handling
        void registerCommandHandler(const std::string& command, CommandHandler handler);
        void registerServerCommandHandler(const std::string& command, CommandHandler handler);
        void handleCommand(const std::string& command, ClientConnection* client);

    private:
        void initializeEpoll();
        void acceptNewConnections();
        void handleClientData();
        void handleStdinCommand();
        void cleanup();

        int epollFd_;
        Socket playerSocket_;
        Socket spectatorSocket_;
        int playerPort_;
        int spectatorPort_;
        std::vector<epoll_event> events_;
        std::map<int, std::unique_ptr<ClientConnection>> clients_;
        std::unique_ptr<CommandRouter> commandRouter_;
        std::unique_ptr<CommandRouter> serverCommandRouter_;
        
        static const int MAX_EVENTS = 64;
        bool running_;
        
        std::string stdinBuffer_;
    };
} 
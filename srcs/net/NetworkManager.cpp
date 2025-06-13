#include "net/NetworkManager.hpp"
#include "commands/Command.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <iostream>
#include "core/Player.hpp"
#include "core/Team.hpp"
#include "core/Tile.hpp"
#include <sstream>
#include "net/CommandRouter.hpp"

namespace Zappy {
    // NetworkManager implementation
    NetworkManager::NetworkManager(World& world, int playerPort, int spectatorPort)
        : world_(world)
        , epollFd_(-1)
        , playerSocket_(AF_INET, SOCK_STREAM, 0)
        , spectatorSocket_(AF_INET, SOCK_STREAM, 0)
        , playerPort_(playerPort)
        , spectatorPort_(spectatorPort)
        , commandRouter_(std::make_unique<CommandRouter>())
        , serverCommandRouter_(std::make_unique<CommandRouter>())
        , running_(false) {
        
        try {
            struct sockaddr_in playerAddr;
            playerAddr.sin_family = AF_INET;
            playerAddr.sin_port = htons(playerPort);
            playerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            
            if (bind(playerSocket_.getFd(), (struct sockaddr*)&playerAddr, sizeof(playerAddr)) == -1) {
                throw std::runtime_error("Failed to bind player socket");
            }
            
            if (listen(playerSocket_.getFd(), SOMAXCONN) == -1) {
                throw std::runtime_error("Failed to listen on player socket");
            }
            
            struct sockaddr_in spectatorAddr;
            spectatorAddr.sin_family = AF_INET;
            spectatorAddr.sin_port = htons(spectatorPort);
            spectatorAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            
            if (bind(spectatorSocket_.getFd(), (struct sockaddr*)&spectatorAddr, sizeof(spectatorAddr)) == -1) {
                throw std::runtime_error("Failed to bind spectator socket");
            }
            
            if (listen(spectatorSocket_.getFd(), SOMAXCONN) == -1) {
                throw std::runtime_error("Failed to listen on spectator socket");
            }
            
            initializeEpoll();
            
            // Add server sockets to epoll
            epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            
            ev.data.fd = getPlayerServerFd();
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, getPlayerServerFd(), &ev) == -1) {
                throw std::runtime_error("Failed to add player server to epoll");
            }
            
            ev.data.fd = getSpectatorServerFd();
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, getSpectatorServerFd(), &ev) == -1) {
                throw std::runtime_error("Failed to add spectator server to epoll");
            }

            // Add stdin to epoll
            ev.data.fd = STDIN_FILENO;
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
                throw std::runtime_error("Failed to add stdin to epoll");
            }
        } catch (...) {
            std::cerr << "NetworkManager constructor failed" << std::endl;
            cleanup();
            throw;
        }
    }

    NetworkManager::~NetworkManager() {
        cleanup();
    }

    void NetworkManager::cleanup() {
        if (epollFd_ != -1) close(epollFd_);
        epollFd_ = -1;
    }

    void NetworkManager::initializeEpoll() {
        epollFd_ = epoll_create1(0);
        if (epollFd_ == -1) {
            throw std::runtime_error("Failed to create epoll instance");
        }
    }

    void NetworkManager::start() {
        running_ = true;
    }

    void NetworkManager::stop() {
        running_ = false;
    }

    void NetworkManager::update() {
        if (!running_) return;

        events_.resize(MAX_EVENTS);
        int nfds = epoll_wait(epollFd_, events_.data(), MAX_EVENTS, 0);
        if (nfds == -1) {
            if (errno != EINTR) {
                throw std::runtime_error("epoll_wait failed");
            }
            return;
        }

        for (int n = 0; n < nfds; ++n) {
            if (events_[n].data.fd == STDIN_FILENO) {
                handleStdinCommand();
            } else if (events_[n].data.fd == getPlayerServerFd() || events_[n].data.fd == getSpectatorServerFd()) {
                acceptNewConnections();
                std::cout << "acceptNewConnections" << std::endl;
            } else {
                handleClientData();
                std::cout << "handleClientData" << std::endl;
            }
        }
    }

    void NetworkManager::acceptNewConnections() {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        // Accept connections on both server sockets
        for (int serverFd : {getPlayerServerFd(), getSpectatorServerFd()}) {
            while (running_) {
                int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);
                
                if (clientFd == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        throw std::runtime_error("Accept failed");
                    }
                    break;
                }
                
                // Set client socket to non-blocking
                Socket::setNonBlocking(clientFd);
                
                // Create new client connection
                auto client = std::make_unique<ClientConnection>(clientFd);
                
                // Set initial client type based on which server accepted it
                if (serverFd == getPlayerServerFd()) {
                    client->setType(ClientConnection::Type::Player);
                } else {
                    client->setType(ClientConnection::Type::Spectator);
                    // Send initial state to new spectator
                    sendInitialStateToSpectator(client.get());
                }
                
                // Add to epoll
                epoll_event ev;
                ev.events = EPOLLIN | EPOLLOUT | EPOLLET;  // Edge-triggered
                ev.data.fd = clientFd;
                
                if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, clientFd, &ev) == -1) {
                    throw std::runtime_error("Failed to add client to epoll");
                }
                
                // Store client connection
                clients_[clientFd] = std::move(client);
            }
        }
    }

    void NetworkManager::sendTeamInfoToSpectator(ClientConnection* spectator) {
        if (!spectator || spectator->getType() != ClientConnection::Type::Spectator) {
            return;
        }

        // Send team names
        for (const auto* team : teams_) {
            if (team) {
                std::stringstream ss;
                ss << "tna " << team->getName() << "\n";
                spectator->sendData(ss.str());
            }
        }
    }

    void NetworkManager::sendInitialStateToSpectator(ClientConnection* spectator) {
        if (!spectator || spectator->getType() != ClientConnection::Type::Spectator) {
            return;
        }

        // Send map size
        const Map& map = world_.getMap();
        std::stringstream ss;
        ss << "msz " << map.getWidth() << " " << map.getHeight() << "\n";
        spectator->sendData(ss.str());

        // Send team info
        sendTeamInfoToSpectator(spectator);

        // Send current state of all tiles
        for (int y = 0; y < map.getHeight(); ++y) {
            for (int x = 0; x < map.getWidth(); ++x) {
                const Tile* tile = map.getTile(x, y);
                if (tile) {
                    handleMapUpdate(x, y, tile);
                }
            }
        }

        // Send current state of all players
        for (const auto& [id, player] : world_.getPlayers()) {
            if (player) {
                onPlayerAdded(player.get());
            }
        }

        // Mark spectator as having received team info
        spectatorsWithTeamInfo_.insert(spectator->getFd());
    }

    void NetworkManager::handleClientData() {
        for (const auto& event : events_) {
            if (event.data.fd == getPlayerServerFd() || event.data.fd == getSpectatorServerFd()) {
                continue;  // Skip server sockets
            }
            
            auto it = clients_.find(event.data.fd);
            if (it == clients_.end()) {
                continue;  // Client not found
            }
            
            auto& client = it->second;
            
            // Handle readable events
            if (event.events & EPOLLIN) {
                if (!client->readData()) {
                    removeClient(event.data.fd);
                    continue;
                }
                
                // Process any complete commands
                while (client->hasCompleteCommand()) {
                    std::string commandLine = client->getNextCommand();
                    std::string commandName = CommandRouter::getCommandName(commandLine);

                    // Check if client can execute this command
                    if (!client->canExecuteCommand(commandName)) {
                        if (client->getType() == ClientConnection::Type::Player && 
                            client->getState() == ClientConnection::State::UNREGISTERED) {
                            client->sendData("ko\n");  // Invalid command for unregistered player
                        }
                        continue;
                    }

                    // Route to appropriate command handler based on client type
                    std::unique_ptr<Command> command;
                    if (client->getType() == ClientConnection::Type::Spectator) {
                        command = commandRouter_->routeCommand(commandLine, client.get());
                    } else if (client->getType() == ClientConnection::Type::Player) {
                        command = commandRouter_->routeCommand(commandLine, client.get());
                    }

                    if (command) {
                        command->execute();
                    } else {
                        client->sendData("ko\n");  // Unknown command
                    }
                }
            }
            
            // Handle writable events if needed
            if (event.events & EPOLLOUT) {
                // Handle any pending writes
            }
            
            // Handle error conditions
            if (event.events & (EPOLLERR | EPOLLHUP)) {
                removeClient(event.data.fd);
            }
        }
    }

    void NetworkManager::addClient(std::unique_ptr<ClientConnection> client) {
        clients_[client->getFd()] = std::move(client);
    }

    void NetworkManager::removeClient(int clientId) {
        auto it = clients_.find(clientId);
        if (it != clients_.end()) {
            // Remove from spectators with team info if it was a spectator
            if (it->second->getType() == ClientConnection::Type::Spectator) {
                spectatorsWithTeamInfo_.erase(clientId);
            }
            epoll_ctl(epollFd_, EPOLL_CTL_DEL, clientId, nullptr);
            clients_.erase(it);
        }
    }

    ClientConnection* NetworkManager::getClient(int clientId) {
        auto it = clients_.find(clientId);
        return it != clients_.end() ? it->second.get() : nullptr;
    }

    const ClientConnection* NetworkManager::getClient(int clientId) const {
        auto it = clients_.find(clientId);
        return it != clients_.end() ? it->second.get() : nullptr;
    }

    void NetworkManager::registerCommandHandler(const std::string& command, CommandHandler handler) {
        commandRouter_->registerHandler(command, handler);
    }

    void NetworkManager::handleCommand(const std::string& command, ClientConnection* client) {
        auto cmd = commandRouter_->routeCommand(command, client);
        if (cmd) {
            cmd->execute();
        }
    }

    void NetworkManager::handleStdinCommand() {
        char buf[1024];
        ssize_t bytesRead = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        
        if (bytesRead > 0) {
            buf[bytesRead] = '\0';
            stdinBuffer_ += buf;
            
            // Process complete lines
            size_t pos;
            while ((pos = stdinBuffer_.find('\n')) != std::string::npos) {
                std::string cmd = stdinBuffer_.substr(0, pos);
                if (!cmd.empty() && cmd.back() == '\r') {
                    cmd.pop_back();
                }
                
                if (!cmd.empty()) {
                    auto command = serverCommandRouter_->routeCommand(cmd, nullptr);
                    if (command) {
                        command->execute();
                    }
                    // Prompt after command execution
                    std::cout << "$> " << std::flush;
                }
                
                stdinBuffer_.erase(0, pos + 1);
            }
        }
    }

    void NetworkManager::registerServerCommandHandler(const std::string& command, CommandHandler handler) {
        serverCommandRouter_->registerHandler(command, handler);
    }

    std::vector<std::string> NetworkManager::getServerCommandNames() const {
        return serverCommandRouter_->getCommandNames();
    }

    std::unique_ptr<Command> NetworkManager::createServerCommand(const std::string& command,
                                                               const std::vector<std::string>& tokens,
                                                               ClientConnection* client) const {
        return serverCommandRouter_->createCommand(command, tokens, client);
    }

    void NetworkManager::broadcastToSpectators(const std::string& message) {
        for (const auto& [fd, client] : clients_) {
            if (client->getType() == ClientConnection::Type::Spectator) {
                client->sendData(message);
            }
        }
    }

    void NetworkManager::onPlayerMoved(const Player* player) {
        if (!player) return;
        std::string ppo = "ppo " + std::to_string(player->getId()) + " " +
                         std::to_string(player->getX()) + " " +
                         std::to_string(player->getY()) + " " +
                         std::to_string(static_cast<int>(player->getDirection())) + "\n";
        broadcastToSpectators(ppo);
    }

    void NetworkManager::onPlayerTurned(const Player* player) {
        if (!player) return;
        std::string ppo = "ppo " + std::to_string(player->getId()) + " " +
                         std::to_string(player->getX()) + " " +
                         std::to_string(player->getY()) + " " +
                         std::to_string(static_cast<int>(player->getDirection())) + "\n";
        broadcastToSpectators(ppo);
    }

    void NetworkManager::onPlayerLevelUp(const Player* player) {
        if (!player) return;
        std::string plv = "plv " + std::to_string(player->getId()) + " " +
                         std::to_string(player->getLevel()) + "\n";
        broadcastToSpectators(plv);
    }

    void NetworkManager::onPlayerInventoryChanged(const Player* player) {
        if (!player) return;
        const auto& inventory = player->getInventory();
        std::string pin = "pin " + std::to_string(player->getId()) + " " +
                         std::to_string(player->getX()) + " " +
                         std::to_string(player->getY()) + " " +
                         std::to_string(inventory.getFood()) + " " +
                         std::to_string(inventory.getLinemate()) + " " +
                         std::to_string(inventory.getDeraumere()) + " " +
                         std::to_string(inventory.getSibur()) + " " +
                         std::to_string(inventory.getMendiane()) + " " +
                         std::to_string(inventory.getPhiras()) + " " +
                         std::to_string(inventory.getThystame()) + "\n";
        broadcastToSpectators(pin);
    }

    void NetworkManager::onPlayerDied(const Player* player) {
        if (!player) return;
        broadcastToSpectators("pdi " + std::to_string(player->getId()) + "\n");
    }

    void NetworkManager::onResourceAdded(const Tile* tile, ResourceType type) {
        if (!tile) return;
        (void)type;
        // TODO: Get tile coordinates from World
        int x = 0, y = 0;  // Placeholder
        std::string bct = "bct " + std::to_string(x) + " " + std::to_string(y) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::FOOD)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::LINEMATE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::DERAUMERE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::SIBUR)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::MENDIANE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::PHIRAS)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::THYSTAME)) + "\n";
        broadcastToSpectators(bct);
    }

    void NetworkManager::onResourceRemoved(const Tile* tile, ResourceType type) {
        if (!tile) return;
        (void)type;
        // TODO: Get tile coordinates from World
        int x = 0, y = 0;  // Placeholder
        std::string bct = "bct " + std::to_string(x) + " " + std::to_string(y) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::FOOD)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::LINEMATE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::DERAUMERE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::SIBUR)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::MENDIANE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::PHIRAS)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::THYSTAME)) + "\n";
        broadcastToSpectators(bct);
    }

    void NetworkManager::registerPlayer(Player* player) {
        if (player) {
            player->attach(this);
            players_.push_back(player);
        }
    }

    void NetworkManager::unregisterPlayer(Player* player) {
        if (player) {
            player->detach(this);
            players_.erase(
                std::remove(players_.begin(), players_.end(), player),
                players_.end()
            );
        }
    }

    void NetworkManager::handleTeamUpdate(const std::string& teamName, const Team* team) {
        if (!team) {
            // Team was removed
            std::stringstream ss;
            ss << "tna " << teamName << "\n";
            broadcastToSpectators(ss.str());
            return;
        }

        // Only send team updates to spectators that haven't received initial state
        for (const auto& [fd, client] : clients_) {
            if (client->getType() == ClientConnection::Type::Spectator && 
                spectatorsWithTeamInfo_.find(fd) == spectatorsWithTeamInfo_.end()) {
                std::stringstream ss;
                ss << "tna " << teamName << "\n";
                client->sendData(ss.str());
            }
        }
    }

    void NetworkManager::handleMapUpdate(int x, int y, const Tile* tile) {
        if (!tile) {
            return;
        }

        // Broadcast tile update to spectators
        std::string bct = "bct " + std::to_string(x) + " " + std::to_string(y) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::FOOD)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::LINEMATE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::DERAUMERE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::SIBUR)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::MENDIANE)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::PHIRAS)) + " " +
                         std::to_string(tile->getResourceCount(ResourceType::THYSTAME)) + "\n";
        broadcastToSpectators(bct);
    }

    void NetworkManager::registerTile(Tile* tile) {
        if (tile) {
            tile->attach(this);
            tiles_.push_back(tile);
        }
    }

    void NetworkManager::unregisterTile(Tile* tile) {
        if (tile) {
            tile->detach(this);
            tiles_.erase(
                std::remove(tiles_.begin(), tiles_.end(), tile),
                tiles_.end()
            );
        }
    }

    void NetworkManager::onPlayerAdded(const Player* player) {
        if (!player) return;
        std::stringstream ss;
        ss << "pnw " << player->getId() << " "
           << player->getX() << " "
           << player->getY() << " "
           << static_cast<int>(player->getDirection()) << " "
           << player->getLevel() << " "
           << player->getTeam().getName() << "\n";
        broadcastToSpectators(ss.str());
    }

    void NetworkManager::onPlayerRemoved(const Player* player) {
        if (!player) return;
        std::stringstream ss;
        ss << "pdi " << player->getId() << "\n";
        broadcastToSpectators(ss.str());
    }

    void NetworkManager::onTeamWon(const Team* team) {
        if (!team) return;
        std::stringstream ss;
        ss << "seg " << team->getName() << "\n";
        broadcastToSpectators(ss.str());
    }

    void NetworkManager::handlePlayerUpdate(int playerId, const Player* player) {
        if (!player) return;

        // Get the client connection for this player
        auto it = clients_.find(playerId);
        if (it == clients_.end()) return;

        ClientConnection* client = it->second.get();
        if (!client) return;

        // Send player state update
        std::stringstream ss;
        ss << "pin " << player->getId() << " "
           << player->getX() << " "
           << player->getY() << " "
           << static_cast<int>(player->getDirection()) << " "
           << player->getLevel() << " "
           << player->getTeam().getName() << "\n";
        client->sendData(ss.str());

        // Send inventory update
        const auto& inventory = player->getInventory();
        std::stringstream inv;
        inv << "pinv " << player->getId() << " "
            << inventory.getCount(ResourceType::FOOD) << " "
            << inventory.getCount(ResourceType::LINEMATE) << " "
            << inventory.getCount(ResourceType::DERAUMERE) << " "
            << inventory.getCount(ResourceType::SIBUR) << " "
            << inventory.getCount(ResourceType::MENDIANE) << " "
            << inventory.getCount(ResourceType::PHIRAS) << " "
            << inventory.getCount(ResourceType::THYSTAME) << "\n";
        client->sendData(inv.str());
    }
} 
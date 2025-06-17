#include "net/NetworkManager.hpp"
#include "commands/Command.hpp"
#include <stdexcept>
#include <unistd.h>
#include <cstring>
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
    NetworkManager::NetworkManager(World& world, int playerPort, int spectatorPort)
        : world_(world)
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
        } catch (...) {
            std::cerr << "NetworkManager constructor failed" << std::endl;
            throw;
        }
    }

    void NetworkManager::start() {
        // Register player socket
        eventLoop_.addFd(playerSocket_.getFd(), EPOLLIN | EPOLLET, [this](uint32_t events) {
            if (events & EPOLLIN)
                acceptNewConnection(playerSocket_.getFd(), ClientConnection::Type::Player);
        });

        // Register spectator socket
        eventLoop_.addFd(spectatorSocket_.getFd(), EPOLLIN | EPOLLET, [this](uint32_t events) {
            if (events & EPOLLIN)
                acceptNewConnection(spectatorSocket_.getFd(), ClientConnection::Type::Spectator);
        });

        // Register STDIN
        eventLoop_.addFd(STDIN_FILENO, EPOLLIN, [this](uint32_t events) {
            if (events & EPOLLIN)
                handleStdinCommand();
        });
        running_ = true;
    }

    void NetworkManager::stop() {
        running_ = false;
    }

    void NetworkManager::update() {
        if (!running_) return;

        eventLoop_.poll();
    }

    void NetworkManager::acceptNewConnection(int serverFd, ClientConnection::Type type) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        while (true) {
            int clientFd = ::accept(serverFd, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
            if (clientFd == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                throw std::runtime_error("accept failed: " + std::string(std::strerror(errno)));
            }

            auto conn = std::make_unique<ClientConnection>(clientFd);
            conn->setType(type);

            if (type == ClientConnection::Type::Spectator)
                sendInitialStateToSpectator(conn.get());

            clientManager_.addClient(clientFd, std::move(conn));

            eventLoop_.addFd(clientFd, EPOLLIN | EPOLLOUT | EPOLLET, [this, clientFd](uint32_t events) {
                handleClientEvent(clientFd, events);
            });
        }
    }

    void NetworkManager::handleClientEvent(int clientFd, uint32_t events) {
        ClientConnection* client = clientManager_.getClient(clientFd);
        if (!client) return;

        if (events & EPOLLIN) {
            if (!client->readData()) {
                clientManager_.removeClient(clientFd);
                return;
            }

            while (client->hasCompleteCommand()) {
                auto cmdLine = client->getNextCommand();
                std::string cmdName = CommandRouter::getCommandName(cmdLine);

                // Check if client can execute this command
                if (!client->canExecuteCommand(cmdName)) {
                    if (client->getType() == ClientConnection::Type::Player && 
                        client->getState() == ClientConnection::State::UNREGISTERED) {
                        client->sendData("ko\n");  // Invalid command for unregistered player
                    }
                    continue;
                }

                // Route to appropriate command handler based on client type
                std::unique_ptr<Command> command;
                if (client->getType() == ClientConnection::Type::Spectator) {
                    command = commandRouter_->routeCommand(cmdLine, client);
                } else if (client->getType() == ClientConnection::Type::Player) {
                    command = commandRouter_->routeCommand(cmdLine, client);
                }

                if (command) {
                    command->execute();
                } else {
                    client->sendData("ko\n");  // Unknown command
                }
            }
        }

        if (events & (EPOLLERR | EPOLLHUP)) {
            clientManager_.removeClient(clientFd);
        }

        // TODO: handle EPOLLOUT if you have outgoing write queues
    }

    void NetworkManager::sendInitialStateToSpectator(ClientConnection* spectator) {
        if (!spectator || spectator->getType() != ClientConnection::Type::Spectator) {
            return;
        }

        const Map& map = world_.getMap();
        const std::vector<std::unique_ptr<Team>>& teams = world_.getTeams();

        // Map size
        std::stringstream ss;
        ss << "msz " << map.getWidth() << " " << map.getHeight() << std::endl;
//        spectator->sendData(ss.str());

        // Team names
        for (const auto& team : teams) {
            ss << "tna " << team->getName() << std::endl;
        }

        // Send current state of all tiles
        // for (int y = 0; y < map.getHeight(); ++y) {
        //     for (int x = 0; x < map.getWidth(); ++x) {
        //         const Tile* tile = map.getTile(x, y);
        //         if (tile) {
        //             handleMapUpdate(x, y, tile);
        //         }
        //     }
        // }

        // Send current state of all players
        // for (const auto& [id, player] : world_.getPlayers()) {
        //     if (player) {
        //         onPlayerAdded(player.get());
        //     }
        // }
       spectator->sendData(ss.str());
    }

    size_t NetworkManager::connectedClientsSize() const {
        return clientManager_.getClients().size();
    }

    void NetworkManager::registerCommandHandler(const std::string& command, CommandHandler handler) {
        commandRouter_->registerHandler(command, handler);
    }

    void NetworkManager::handleStdinCommand() {
        char buf[1024];
        std::string stdinBuffer;

        ssize_t bytesRead = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        
        if (bytesRead > 0) {
            buf[bytesRead] = '\0';
            stdinBuffer += buf;
            
            // Process complete lines
            size_t pos;
            while ((pos = stdinBuffer.find('\n')) != std::string::npos) {
                std::string cmd = stdinBuffer.substr(0, pos);
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
                
                stdinBuffer.erase(0, pos + 1);
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
        for (const auto& [fd, client] : clientManager_.getClients()) {
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
} 

// void NetworkManager::handleMapUpdate(int x, int y, const Tile* tile) {
//     if (!tile) {
//         return;
//     }

//     // Broadcast tile update to spectators
//     std::string bct = "bct " + std::to_string(x) + " " + std::to_string(y) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::FOOD)) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::LINEMATE)) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::DERAUMERE)) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::SIBUR)) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::MENDIANE)) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::PHIRAS)) + " " +
//                      std::to_string(tile->getResourceCount(ResourceType::THYSTAME)) + "\n";
//     broadcastToSpectators(bct);
// }

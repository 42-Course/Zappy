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
#include "commands/CommandDispatcher.hpp"

namespace Zappy {
    NetworkManager::NetworkManager(World& world, int playerPort, int spectatorPort)
        : world_(world)
        , playerSocket_(AF_INET, SOCK_STREAM, 0)
        , spectatorSocket_(AF_INET, SOCK_STREAM, 0)
        , playerPort_(playerPort)
        , spectatorPort_(spectatorPort)
        , broadcaster_(clientManager_)
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
                std::string cmdName = CommandDispatcher::extractCommandName(cmdLine);

                // Check if client can execute this command
                if (!client->canExecuteCommand(cmdName)) {
                    if (client->getType() == ClientConnection::Type::Player && 
                        client->getState() == ClientConnection::State::UNREGISTERED) {
                        client->sendData("ko\n");  // Invalid command for unregistered player
                    }
                    continue;
                }

                // Route to appropriate command handler based on client type
                std::unique_ptr<ICommand> command;
                switch (client->getType()) {
                    case ClientConnection::Type::Spectator:
                        command = spectatorDispatcher_.dispatch(cmdLine, client);
                        break;
                    case ClientConnection::Type::Player:
                        command = playerDispatcher_.dispatch(cmdLine, client);
                        break;
                    default:
                        client->sendData("ko\n");
                        return;
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

        // Team names
        for (const auto& team : teams) {
            ss << "tna " << team->getName() << std::endl;
        }

        // Players
        for (const auto& [id, player] : world_.getPlayers()) {
            if (player) {
                ss << player->toPnwString();
            }
        }

        // Tiles
        for (int y = 0; y < map.getHeight(); ++y) {
            for (int x = 0; x < map.getWidth(); ++x) {
                const Tile* tile = map.getTile(x, y);
                if (!tile) continue;

                ss << tile->toBctString(x, y);
            }
        }

       spectator->sendData(ss.str());
    }

    size_t NetworkManager::connectedClientsSize() const {
        return clientManager_.getClients().size();
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
                    auto command = serverDispatcher_.dispatch(cmd, nullptr);
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

    void NetworkManager::registerPlayerCommand(const std::string& cmd, CommandHandler h) {
        playerDispatcher_.registerHandler(cmd, std::move(h));
    }

    void NetworkManager::registerSpectatorCommand(const std::string& cmd, CommandHandler h) {
        spectatorDispatcher_.registerHandler(cmd, std::move(h));
    }

    void NetworkManager::registerServerCommand(const std::string& cmd, CommandHandler h) {
        serverDispatcher_.registerHandler(cmd, std::move(h));
    }

    std::vector<std::string> NetworkManager::getServerCommandNames() const {
        return serverDispatcher_.getCommandNames();
    }

    std::unique_ptr<ICommand> NetworkManager::createServerCommand(const std::string& command,
                                                               const std::vector<std::string>& tokens,
                                                               ClientConnection* client) const {
        return serverDispatcher_.createCommand(command, tokens, client);
    }
}
#include "net/NetworkManager.hpp"
#include "commands/Command.hpp"
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <iostream>

namespace Zappy {
    // Socket implementation
    Socket::Socket(int domain, int type, int protocol) {
        fd_ = socket(domain, type, protocol);
        if (fd_ == -1) {
            throw std::runtime_error("Failed to create socket");
        }
    }

    Socket::~Socket() {
        if (fd_ != -1) {
            close(fd_);
        }
    }

    void Socket::setNonBlocking() {
        setNonBlocking(fd_);
    }

    void Socket::setNonBlocking(int fd) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags == -1) {
            throw std::runtime_error("Failed to get socket flags");
        }
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
            throw std::runtime_error("Failed to set socket non-blocking");
        }
    }

    // NetworkManager implementation
    NetworkManager::NetworkManager(int playerPort, int spectatorPort)
        : epollFd_(-1)
        , playerServerFd_(-1)
        , spectatorServerFd_(-1)
        , commandRouter_(std::make_unique<CommandRouter>())
        , serverCommandRouter_(std::make_unique<CommandRouter>())
        , running_(false) {
        
        try {
            initializeEpoll();
            
            // Setup player server socket
            Socket playerSocket(AF_INET, SOCK_STREAM, 0);
            playerSocket.setNonBlocking();
            
            struct sockaddr_in playerAddr;
            playerAddr.sin_family = AF_INET;
            playerAddr.sin_port = htons(playerPort);
            playerAddr.sin_addr.s_addr = INADDR_ANY;
            
            if (bind(playerSocket.getFd(), (struct sockaddr*)&playerAddr, sizeof(playerAddr)) == -1) {
                throw std::runtime_error("Failed to bind player socket");
            }
            
            if (listen(playerSocket.getFd(), SOMAXCONN) == -1) {
                throw std::runtime_error("Failed to listen on player socket");
            }
            
            playerServerFd_ = playerSocket.getFd();
            
            // Setup spectator server socket similarly
            Socket spectatorSocket(AF_INET, SOCK_STREAM, 0);
            spectatorSocket.setNonBlocking();
            
            struct sockaddr_in spectatorAddr;
            spectatorAddr.sin_family = AF_INET;
            spectatorAddr.sin_port = htons(spectatorPort);
            spectatorAddr.sin_addr.s_addr = INADDR_ANY;
            
            if (bind(spectatorSocket.getFd(), (struct sockaddr*)&spectatorAddr, sizeof(spectatorAddr)) == -1) {
                throw std::runtime_error("Failed to bind spectator socket");
            }
            
            if (listen(spectatorSocket.getFd(), SOMAXCONN) == -1) {
                throw std::runtime_error("Failed to listen on spectator socket");
            }
            
            spectatorServerFd_ = spectatorSocket.getFd();
            
            // Add server sockets to epoll
            epoll_event ev;
            ev.events = EPOLLIN;
            
            ev.data.fd = playerServerFd_;
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, playerServerFd_, &ev) == -1) {
                throw std::runtime_error("Failed to add player server to epoll");
            }
            
            ev.data.fd = spectatorServerFd_;
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, spectatorServerFd_, &ev) == -1) {
                throw std::runtime_error("Failed to add spectator server to epoll");
            }

            // Add stdin to epoll
            Socket::setNonBlocking(STDIN_FILENO);
            ev.data.fd = STDIN_FILENO;
            if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
                throw std::runtime_error("Failed to add stdin to epoll");
            }
            
        } catch (...) {
            cleanup();
            throw;
        }
    }

    NetworkManager::~NetworkManager() {
        cleanup();
    }

    void NetworkManager::cleanup() {
        if (epollFd_ != -1) close(epollFd_);
        if (playerServerFd_ != -1) close(playerServerFd_);
        if (spectatorServerFd_ != -1) close(spectatorServerFd_);
        
        epollFd_ = -1;
        playerServerFd_ = -1;
        spectatorServerFd_ = -1;
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
            } else if (events_[n].data.fd == playerServerFd_ || events_[n].data.fd == spectatorServerFd_) {
                acceptNewConnections();
            } else {
                handleClientData();
            }
        }
    }

    void NetworkManager::acceptNewConnections() {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        // Accept connections on both server sockets
        for (int serverFd : {playerServerFd_, spectatorServerFd_}) {
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
                if (serverFd == playerServerFd_) {
                    client->setType(ClientConnection::Type::Player);
                } else {
                    client->setType(ClientConnection::Type::Spectator);
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

    void NetworkManager::handleClientData() {
        for (const auto& event : events_) {
            if (event.data.fd == playerServerFd_ || event.data.fd == spectatorServerFd_) {
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
                    auto command = commandRouter_->routeCommand(commandLine, client.get());
                    if (command) {
                        command->execute();
                    }
                }
            }
            
            // Handle error or hangup
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
} 
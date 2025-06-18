#include "core/Engine.hpp"
#include "commands/server/HelpCommand.hpp"
#include "commands/server/StatusCommand.hpp"
#include "commands/server/ExitCommand.hpp"
#include "commands/server/WatchCommand.hpp"
#include "commands/server/ClearCommand.hpp"
#include "commands/spectator/MSZCommand.hpp"
#include "commands/spectator/BCTCommand.hpp"
#include "commands/spectator/MCTCommand.hpp"
#include "commands/spectator/TNACommand.hpp"
#include "commands/spectator/PPOCommand.hpp"
#include "commands/spectator/PLVCommand.hpp"
#include "commands/spectator/PINCommand.hpp"
#include "commands/spectator/SGTCommand.hpp"
#include "commands/spectator/SSTCommand.hpp"
#include "commands/player/ForwardCommand.hpp"
#include <csignal>
#include <string.h>
#include <iostream>

namespace Zappy {
    // Initialize static member
    Engine* Engine::instance_ = nullptr;

    Engine::Engine(const Config& config)
        : world_(std::make_unique<World>(config.getMapWidth(), config.getMapHeight(), config.isInfiniteMap()))
        , gameLoop_(std::make_unique<GameLoop>(config.getTickRate()))
        , network_(std::make_unique<NetworkManager>(*world_, *gameLoop_, config.getPlayerPort(), config.getSpectatorPort()))
        , watchService_(std::make_unique<WatchService>(*this))
        , running_(false) {
        
        // Set singleton instance
        instance_ = this;

        // Initialize teams
        for (const auto& team : config.getTeams()) {
            world_->addTeam(team, config.getMaxPlayersPerTeam());
        }

        setupGameLoop();
        setupSignalHandlers();
        registerCommands();
        welcome();
    }

    void Engine::setupGameLoop() {
        gameLoop_->onUpdate([this]() {
            network_->update();  // Process network events
            watchService_->tick(); // Update watch service
        });

        gameLoop_->onFixedUpdate([this]() {
            world_->update();    // Update game state
        });
    }

    void Engine::registerCommands() {
        // Register command handlers
        network_->registerServerCommand("help", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<HelpCommand>(*this, tokens, client);
        });

        network_->registerServerCommand("status", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<StatusCommand>(*this, tokens, client);
        });

        network_->registerServerCommand("exit", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<ExitCommand>(*this, tokens, client);
        });

        network_->registerServerCommand("watch", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<WatchCommand>(*this, tokens, *watchService_, client);
        });

        network_->registerServerCommand("clear", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<ClearCommand>(*this, tokens, client);
        });

        // Register spectator command handlers
        network_->registerSpectatorCommand("msz", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<MSZCommand>(*world_, tokens, client);
        });

        network_->registerSpectatorCommand("bct", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<BCTCommand>(*world_, tokens, client);
        });

        network_->registerSpectatorCommand("mct", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<MCTCommand>(*world_, tokens, client);
        });

        network_->registerSpectatorCommand("tna", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<TNACommand>(*world_, tokens, client);
        });

        network_->registerSpectatorCommand("ppo", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<PPOCommand>(*world_, tokens, client);
        });

        network_->registerSpectatorCommand("plv", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<PLVCommand>(*world_, tokens, client);
        });

        network_->registerSpectatorCommand("pin", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<PINCommand>(*world_, tokens, client);
        });
        
        network_->registerSpectatorCommand("sgt", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<SGTCommand>(*gameLoop_, tokens, client);
        });

        network_->registerSpectatorCommand("sst", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<SSTCommand>(*gameLoop_, tokens, client);
        });

        // Register spectator command handlers
        network_->registerPlayerCommand("forward", [this](const std::vector<std::string>& tokens, ClientConnection* client) {
            return std::make_unique<ForwardCommand>(*world_, tokens, client);
        });
    }

    Engine::~Engine() {
        stop();
        if (instance_ == this) {
            instance_ = nullptr;
        }
    }

    void Engine::welcome() const {
        std::cout << "\033[1;32m=> Booting Zappy in development mode\033[0m\n";
        std::cout << "\033[1;32m=> ZappyServer version: 42.0\033[0m\n";
        std::cout << "\033[1;32m=> Run `./Zappy --help` for more startup options\033[0m\n";
        std::cout << "Zappy listening in single thread mode\n";
        std::cout << "* C++ version:\t" << __cplusplus << "\n";
        std::cout << "* Server PID:\t" << getpid() << "\n";
        std::cout << "* Players:\thttp://host:" << network_->getPlayerPort() << "\n";
        std::cout << "* Spectators:\thttp://host:" << network_->getSpectatorPort() << "\n\n";
        std::cout << "$> " << std::flush;
    }

    void Engine::setupSignalHandlers() {
        struct sigaction sa;
        sa.sa_handler = handleSignal;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        // Handle SIGINT (Ctrl+C)
        sigaction(SIGINT, &sa, nullptr);
        
        // Handle SIGTERM
        sigaction(SIGTERM, &sa, nullptr);
        
        // Handle SIGQUIT
        sigaction(SIGQUIT, &sa, nullptr);
    }

    void Engine::handleSignal(int signal) {
        std::cout << "\nReceived signal " << signal << " (" << strsignal(signal) << ")" << std::endl;
        
        if (instance_) {
            if (instance_->watchService_->isEnabled()) {
                // If watch mode is enabled, just disable it
                instance_->watchService_->disable();
                std::cout << "$> " << std::flush;
            } else {
                // Otherwise stop the server
                std::cout << "Stopping server gracefully..." << std::endl;
                instance_->stop();
            }
        }
    }

    void Engine::start() {
        if (running_) return;
        running_ = true;

        // Start subsystems
        network_->start();
        gameLoop_->start();  // This will block until stop() is called
    }

    void Engine::stop() {
        if (!running_) return;
        running_ = false;

        // Stop subsystems in reverse order
        gameLoop_->stop();
        network_->stop();
        
        std::cout << "Server stopped." << std::endl;
    }
} 
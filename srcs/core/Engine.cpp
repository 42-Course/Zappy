#include "core/Engine.hpp"
#include "commands/server/HelpCommand.hpp"
#include "commands/server/StatusCommand.hpp"
#include "commands/server/ExitCommand.hpp"
#include "commands/server/WatchCommand.hpp"
#include "commands/server/ClearCommand.hpp"
#include <csignal>
#include <string.h>
#include <iostream>

namespace Zappy {
    // Initialize static member
    Engine* Engine::instance_ = nullptr;

    Engine::Engine(const Config& config)
        : world_(std::make_unique<World>(config.getMapWidth(), config.getMapHeight()))
        , network_(std::make_unique<NetworkManager>(config.getPlayerPort(), config.getSpectatorPort()))
        , gameLoop_(std::make_unique<GameLoop>(config.getTickRate()))
        , watchService_(std::make_unique<WatchService>(*this))
        , running_(false) {
        
        // Set singleton instance
        instance_ = this;
        
        // Initialize teams
        for (const auto& team : config.getTeams()) {
            world_->addTeam(team, config.getMaxPlayersPerTeam());
        }

        // Set up game loop callbacks
        gameLoop_->onUpdate([this]() {
            network_->update();  // Process network events
            watchService_->tick(); // Update watch service
        });

        gameLoop_->onFixedUpdate([this]() {
            world_->update();    // Update game state
        });

        // Register commands and their information
        registerCommands();

        // Show welcome message and initial prompt
        std::cout << "\033[1;32m=== Welcome to Zappy Server ===\033[0m\n";
        std::cout << "Type 'help' for available commands\n";
        std::cout << "$> " << std::flush;
    }

    void Engine::registerCommands() {
        // Register command information
        commandInfo_["help"] = {"Display available commands and their usage", "help [command] - Show help for all commands or specific command"};
        commandInfo_["status"] = {"Display server status and statistics", "status - Show current server state"};
        commandInfo_["exit"] = {"Stop the server and exit", "exit - Gracefully shutdown the server"};
        commandInfo_["watch"] = {"Toggle watch mode to monitor server status", "watch [on|off] - Toggle watch mode or explicitly enable/disable it"};
        commandInfo_["clear"] = {"Clear the terminal screen", "clear - Clear terminal output"};

        // Register command handlers
        network_->registerServerCommandHandler("help", [this](const std::string& commandLine, ClientConnection*) {
            return std::make_unique<HelpCommand>(*this, commandLine, commandInfo_);
        });

        network_->registerServerCommandHandler("status", [this](const std::string&, ClientConnection*) {
            return std::make_unique<StatusCommand>(*this);
        });

        network_->registerServerCommandHandler("exit", [this](const std::string&, ClientConnection*) {
            return std::make_unique<ExitCommand>(*this);
        });

        network_->registerServerCommandHandler("watch", [this](const std::string& commandLine, ClientConnection*) {
            return std::make_unique<WatchCommand>(*this, commandLine, *watchService_);
        });

        network_->registerServerCommandHandler("clear", [this](const std::string&, ClientConnection*) {
            return std::make_unique<ClearCommand>(*this);
        });
    }

    Engine::~Engine() {
        stop();
        if (instance_ == this) {
            instance_ = nullptr;
        }
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
#include "core/Engine.hpp"
#include "commands/server/HelpCommand.hpp"
#include "commands/server/StatusCommand.hpp"
#include "commands/server/ExitCommand.hpp"

namespace Zappy {
    Engine::Engine(const Config& config)
        : world_(std::make_unique<World>(config.getMapWidth(), config.getMapHeight()))
        , network_(std::make_unique<NetworkManager>(config.getPlayerPort(), config.getSpectatorPort()))
        , gameLoop_(std::make_unique<GameLoop>(config.getTickRate()))
        , running_(false) {
        
        // Initialize teams
        for (const auto& team : config.getTeams()) {
            world_->addTeam(team, config.getMaxPlayersPerTeam());
        }

        // Set up game loop callbacks
        gameLoop_->onUpdate([this]() {
            network_->update();  // Process network events
        });

        gameLoop_->onFixedUpdate([this]() {
            world_->update();    // Update game state
        });

        // Register server commands
        network_->registerServerCommandHandler("help", [this](const std::string&, ClientConnection*) {
            return std::make_unique<HelpCommand>(this);
        });

        network_->registerServerCommandHandler("status", [this](const std::string&, ClientConnection*) {
            return std::make_unique<StatusCommand>(this);
        });

        network_->registerServerCommandHandler("exit", [this](const std::string&, ClientConnection*) {
            return std::make_unique<ExitCommand>(this);
        });

        // Show initial prompt
        std::cout << "$> " << std::flush;
    }

    Engine::~Engine() {
        stop();
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
    }
} 
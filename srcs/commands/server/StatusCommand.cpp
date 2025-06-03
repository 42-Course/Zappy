#include "commands/server/StatusCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>

namespace Zappy {
    StatusCommand::StatusCommand(Engine* engine)
        : ServerCommand("status", engine) {
    }

    void StatusCommand::execute() {
        std::cout << "\nServer Status:\n";
        std::cout << "-------------\n";
        
        const auto& world = engine_->getWorld();
        const auto& network = engine_->getNetwork();
        const auto& gameLoop = engine_->getGameLoop();
        
        // Server info
        double uptime = static_cast<double>(gameLoop.getCurrentTick()) / gameLoop.getTickRate();
        std::cout << "Uptime: " << uptime << " seconds\n";
        std::cout << "Current tick: " << gameLoop.getCurrentTick() << "\n";
        std::cout << "Tick rate: " << gameLoop.getTickRate() << " ticks/second\n\n";
        
        // World info
        const auto& players = world.getPlayers();
        const auto& teams = world.getTeams();
        
        std::cout << "World Statistics:\n";
        std::cout << "- Map size: " << world.getMap().getWidth() << "x" << world.getMap().getHeight() << "\n";
        std::cout << "- Total players: " << players.size() << "\n";
        std::cout << "- Total teams: " << teams.size() << "\n\n";
        
        // Team details
        std::cout << "Teams:\n";
        for (const auto* team : teams) {
            std::cout << "- " << std::left << std::setw(20) << team->getName() 
                      << ": " << team->getCurrentPlayers() << " players\n";
        }
        std::cout << std::endl;
        
        // Network info
        int totalClients = 0;
        int playerCount = 0;
        int spectatorCount = 0;

        // Count client types
        for (int fd = 3; fd < 1024; ++fd) { // Reasonable fd range
            const auto* client = network.getClient(fd);
            if (!client) continue;
            
            totalClients++;
            if (client->getType() == ClientConnection::Type::Player) {
                playerCount++;
            } else if (client->getType() == ClientConnection::Type::Spectator) {
                spectatorCount++;
            }
        }
        
        std::cout << "Network Statistics:\n";
        std::cout << "- Connected clients: " << totalClients << "\n";
        std::cout << "- Players connected: " << playerCount << "\n";
        std::cout << "- Spectators connected: " << spectatorCount << "\n";
        std::cout << std::endl;
    }
} 
#include "commands/server/StatusCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Zappy {
    StatusCommand::StatusCommand(Engine& engine, ClientConnection* client)
        : ServerCommand("status", engine, client) {
    }

    CommandStatus StatusCommand::execute() {
        try {
            std::stringstream status;
            const auto& world = engine_.getWorld();
            const auto& network = engine_.getNetwork();
            const auto& gameLoop = engine_.getGameLoop();
            
            // Server info
            status << "\nServer Status:\n";
            status << "-------------\n";
            double uptime = static_cast<double>(gameLoop.getCurrentTick()) / gameLoop.getTickRate();
            status << "Uptime: " << uptime << " seconds\n";
            status << "Current tick: " << gameLoop.getCurrentTick() << "\n";
            status << "Tick rate: " << gameLoop.getTickRate() << " ticks/second\n\n";
            
            // World info
            const auto& players = world.getPlayers();
            const auto& teams = world.getTeams();
            
            status << "World Statistics:\n";
            status << "- Map size: " << world.getMap().getWidth() << "x" << world.getMap().getHeight() << "\n";
            status << "- Total players: " << players.size() << "\n";
            status << "- Total teams: " << teams.size() << "\n\n";
            
            // Team details
            status << "Teams:\n";
            for (const auto* team : teams) {
                status << "- " << std::left << std::setw(20) << team->getName() 
                       << ": " << team->getCurrentPlayers() << " players\n";
            }
            status << "\n";
            
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
            
            status << "Network Statistics:\n";
            status << "- Connected clients: " << totalClients << "\n";
            status << "- Players connected: " << playerCount << "\n";
            status << "- Spectators connected: " << spectatorCount << "\n";
            status << std::endl;
            
            // If we have a client, send the status through the network
            if (getClient()) {
                getClient()->sendData(status.str());
            } else {
                // Otherwise print to console
                std::cout << status.str();
            }
            
            logCommand("Detailed status information sent");
            return CommandStatus::COMPLETED;
        } catch (const std::exception& e) {
            setErrorMessage(std::string("Failed to execute status command: ") + e.what());
            return CommandStatus::FAILED;
        }
    }

    bool StatusCommand::parseArgs(const std::vector<std::string>& args) {
        if (!Command::parseArgs(args)) return false;
        return validateArgCount(0);  // Status command takes no arguments
    }

    std::string StatusCommand::getDescription() const {
        return "Display detailed server status and statistics";
    }

    std::string StatusCommand::getUsage() const {
        return "status";
    }
} 
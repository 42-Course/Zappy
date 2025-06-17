#include "commands/server/StatusCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Zappy {

StatusCommand::StatusCommand(Engine& engine, 
                           const std::vector<std::string>& tokens,
                           ClientConnection* client)
    : Command("status", client)
    , engine_(engine) {
    // Skip validation if we have no tokens (direct command creation)
    if (!tokens.empty()) {
        // Skip the first token (command name) when parsing args
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        if (!parseArgs(args)) {
            setErrorMessage("Status command takes no arguments");
            setStatus(CommandStatus::INVALID);
        }
    }
}

CommandStatus StatusCommand::execute() {
    if (getStatus() == CommandStatus::INVALID) {
        std::cout << "Error: " << getErrorMessage() << "\n";
        std::cout << "Usage: " << getUsage() << "\n";
        return CommandStatus::FAILED;
    }

    std::cout << "\nServer Status:\n";
    std::cout << "-------------\n";
    std::cout << "Running: " << (engine_.isRunning() ? "Yes" : "No") << "\n";
    
    // Network status
    auto& network = engine_.getNetwork();
    std::cout << "\nNetwork:\n";
    std::cout << "  Player Port: " << network.getPlayerPort() << "\n";
    std::cout << "  Spectator Port: " << network.getSpectatorPort() << "\n";
    std::cout << "  Connected Clients: " << network.connectedClientsSize() << "\n";
    
    // World status
    auto& world = engine_.getWorld();
    auto& map = world.getMap();
    std::cout << "\nWorld:\n";
    std::cout << "  Size: " << map.getWidth() << "x" << map.getHeight() << "\n";
    std::cout << "  Teams: " << world.getTeams().size() << "\n";
    
    // Game loop status
    auto& gameLoop = engine_.getGameLoop();
    std::cout << "\nGame Loop:\n";
    std::cout << "  Tick Rate: " << gameLoop.getTickRate() << " Hz\n";
    std::cout << "  Running: " << (gameLoop.isRunning() ? "Yes" : "No") << "\n";
    
    return CommandStatus::COMPLETED;
}

bool StatusCommand::parseArgs(const std::vector<std::string>& args) {
    // Status takes no arguments
    return args.empty();
}

std::string StatusCommand::getDescription() const {
    return "Display detailed server status and statistics";
}

std::string StatusCommand::getUsage() const {
    return "status";
}
} 
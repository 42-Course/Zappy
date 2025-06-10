#include "commands/spectator/MSZCommand.hpp"
#include "core/World.hpp"
#include <sstream>

namespace Zappy {

MSZCommand::MSZCommand(ClientConnection* client, World* world)
    : Command("msz", CommandType::SPECTATOR, 0, client), world_(world) {}

bool MSZCommand::parseArgs(const std::vector<std::string>& args) {
    // First call base class implementation to store args
    if (!Command::parseArgs(args)) {
        return false;
    }
    
    // MSZ command takes no arguments
    return validateArgCount(0);
}

CommandStatus MSZCommand::execute() {
    if (!world_) {
        setErrorMessage("World not initialized");
        return CommandStatus::FAILED;
    }
    
    try {
        // Format: msz X Y
        const Map& map = world_->getMap();
        std::string response = getName() + " " + 
            std::to_string(map.getWidth()) + " " + 
            std::to_string(map.getHeight()) + "\n";
        
        // Send response to client
        getClient()->sendData(response);
        
        logCommand("Sent map size: " + std::to_string(map.getWidth()) + 
                  "x" + std::to_string(map.getHeight()));
        
        return CommandStatus::COMPLETED;
    } catch (const std::exception& e) {
        setErrorMessage(std::string("Failed to execute MSZ command: ") + e.what());
        return CommandStatus::FAILED;
    }
}

} // namespace Zappy 
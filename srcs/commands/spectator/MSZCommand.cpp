#include "commands/spectator/MSZCommand.hpp"
#include "core/World.hpp"
#include <sstream>

namespace Zappy {

MSZCommand::MSZCommand(World& world, ClientConnection* client)
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
    try {
        getClient()->sendData(world_.getMap().toMszString());
        
        logCommand("Sent map size");
        
        return CommandStatus::COMPLETED;
    } catch (const std::exception& e) {
        setErrorMessage(std::string("Failed to execute MSZ command: ") + e.what());
        return CommandStatus::FAILED;
    }
}

} // namespace Zappy 
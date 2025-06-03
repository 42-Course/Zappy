#include "commands/ServerCommand.hpp"
#include "net/CommandRouter.hpp"

namespace Zappy {
    ServerCommand::ServerCommand(const std::string& name, Engine* engine)
        : Command(name)
        , engine_(engine) {
        // Parse command arguments if any
        args_ = CommandRouter::getCommandArgs(name);
    }
} 
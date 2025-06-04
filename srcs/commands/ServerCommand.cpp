#include "commands/ServerCommand.hpp"
#include "net/CommandRouter.hpp"

namespace Zappy {
    ServerCommand::ServerCommand(Engine& engine, const std::string& name)
        : Command(name)
        , engine_(engine) {
        // Parse command arguments if any
        args_ = CommandRouter::getCommandArgs(name);
    }
} 
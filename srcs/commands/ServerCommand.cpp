#include "commands/ServerCommand.hpp"
#include "net/CommandRouter.hpp"
#include "core/Engine.hpp"

namespace Zappy {
    ServerCommand::ServerCommand(const std::string& name, Engine& engine, ClientConnection* client)
        : Command(name, CommandType::STDIN, 0, client), engine_(engine) {
        // Parse command arguments if any
        args_ = CommandRouter::getCommandArgs(name);
    }
} 
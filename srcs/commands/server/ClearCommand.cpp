#include "commands/server/ClearCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace Zappy {

ClearCommand::ClearCommand(Engine& engine, 
                         const std::vector<std::string>& tokens,
                         ClientConnection* client)
    : Command("clear", client)
    , engine_(engine) {
    // Skip validation if we have no tokens (direct command creation)
    if (!tokens.empty()) {
        // Skip the first token (command name) when parsing args
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        if (!parseArgs(args)) {
            setErrorMessage("Clear command takes no arguments");
            setStatus(CommandStatus::INVALID);
        }
    }
}

CommandStatus ClearCommand::execute() {
    if (getStatus() == CommandStatus::INVALID) {
        std::cout << "Error: " << getErrorMessage() << "\n";
        std::cout << "Usage: " << getUsage() << "\n";
        return CommandStatus::FAILED;
    }

    // Clear screen using ANSI escape sequence
    std::cout << "\033[2J\033[H";
    return CommandStatus::COMPLETED;
}

bool ClearCommand::parseArgs(const std::vector<std::string>& args) {
    // Clear takes no arguments
    return args.empty();
}

std::string ClearCommand::getDescription() const {
    return "Clear the terminal screen";
}

std::string ClearCommand::getUsage() const {
    return "clear";
}

} // namespace Zappy 
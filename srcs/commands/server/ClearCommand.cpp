#include "commands/server/ClearCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace Zappy {

ClearCommand::ClearCommand(Engine& engine, ClientConnection* client)
    : ServerCommand("clear", engine, client) {}

CommandStatus ClearCommand::execute() {
    try {
        // ANSI escape sequence to clear screen and move cursor to home position
        std::cout << "\033[2J\033[H" << std::flush;
        logCommand("Screen cleared");
        return CommandStatus::COMPLETED;
    } catch (const std::exception& e) {
        setErrorMessage(std::string("Failed to execute clear command: ") + e.what());
        return CommandStatus::FAILED;
    }
}

bool ClearCommand::parseArgs(const std::vector<std::string>& args) {
    if (!Command::parseArgs(args)) return false;
    return validateArgCount(0);  // Clear command takes no arguments
}

std::string ClearCommand::getDescription() const {
    return "Clear the terminal screen";
}

std::string ClearCommand::getUsage() const {
    return "clear";
}

} // namespace Zappy 
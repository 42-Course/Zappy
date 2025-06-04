#include "commands/server/ClearCommand.hpp"
#include <iostream>

namespace Zappy {

ClearCommand::ClearCommand(Engine& engine)
    : ServerCommand(engine, "clear")
{}

void ClearCommand::execute() {
    // Clear screen using ANSI escape code
    std::cout << "\033[2J\033[H";
}

std::string ClearCommand::getDescription() const {
    return "Clear the terminal screen";
}

std::string ClearCommand::getUsage() const {
    return "clear - Clear the terminal screen";
}

} // namespace Zappy 
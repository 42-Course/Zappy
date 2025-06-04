#include "commands/server/ExitCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace Zappy {
    ExitCommand::ExitCommand(Engine& engine)
        : ServerCommand(engine, "exit") {
    }

    void ExitCommand::execute() {
        std::cout << "\nStopping server...\n";
        engine_.stop();
        std::cout << "Server stopped. Goodbye!\n" << std::endl;
    }
} 
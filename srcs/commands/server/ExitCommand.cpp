#include "commands/server/ExitCommand.hpp"
#include "core/Engine.hpp"
#include <iostream>

namespace Zappy {
    ExitCommand::ExitCommand(Engine& engine, ClientConnection* client)
        : ServerCommand("exit", engine, client) {
    }

    CommandStatus ExitCommand::execute() {
        try {
            std::string response = "Server is shutting down...\n";
            
            // Send response through appropriate channel
            if (getClient()) {
                getClient()->sendData(response);
            } else {
                std::cout << response;
            }
            
            logCommand("Server shutdown initiated");
            engine_.stop();
            return CommandStatus::COMPLETED;
        } catch (const std::exception& e) {
            setErrorMessage(std::string("Failed to execute exit command: ") + e.what());
            return CommandStatus::FAILED;
        }
    }

    bool ExitCommand::parseArgs(const std::vector<std::string>& args) {
        if (!Command::parseArgs(args)) return false;
        return validateArgCount(0);  // Exit command takes no arguments
    }

    std::string ExitCommand::getDescription() const {
        return "Stop the server and exit";
    }

    std::string ExitCommand::getUsage() const {
        return "exit - Gracefully shutdown the server";
    }
} 
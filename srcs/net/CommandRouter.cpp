#include "net/CommandRouter.hpp"
#include "commands/Command.hpp"
#include <sstream>
#include <algorithm>

namespace Zappy {
    CommandRouter::CommandRouter() {}
    
    CommandRouter::~CommandRouter() {
        handlers_.clear();
    }

    void CommandRouter::registerHandler(const std::string& command, CommandHandler handler) {
        handlers_[command] = handler;
    }

    void CommandRouter::unregisterHandler(const std::string& command) {
        handlers_.erase(command);
    }

    std::unique_ptr<Command> CommandRouter::routeCommand(const std::string& commandLine, ClientConnection* client) {
        // Split the command line into tokens
        std::vector<std::string> tokens = Command::splitArgs(commandLine);
        
        if (tokens.empty()) {
            return nullptr;
        }
        
        // Get the command name and convert to lowercase
        std::string commandName = tokens[0];
        std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);
        
        // Find the handler
        auto it = handlers_.find(commandName);
        if (it != handlers_.end()) {
            // Pass the entire tokens vector to the handler, including command name
            return it->second(tokens, client);
        }
        
        return nullptr;
    }

    std::string CommandRouter::getCommandName(const std::string& commandLine) {
        std::istringstream iss(commandLine);
        std::string commandName;
        iss >> commandName;
        
        // Convert to lowercase for case-insensitive comparison
        std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);
        
        return commandName;
    }

    std::vector<std::string> CommandRouter::getCommandArgs(const std::string& commandLine) {
        std::istringstream iss(commandLine);
        std::vector<std::string> args;
        std::string token;
        
        // Skip the command name
        iss >> token;
        
        // Get remaining arguments
        while (iss >> token) {
            args.push_back(token);
        }
        
        return args;
    }
} 
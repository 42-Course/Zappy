#include "net/CommandRouter.hpp"
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
        std::string commandName = getCommandName(commandLine);
        
        auto it = handlers_.find(commandName);
        if (it != handlers_.end()) {
            return it->second(commandLine, client);
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
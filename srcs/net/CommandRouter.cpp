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
        // Extract command name and arguments
        std::string commandName = getCommandName(commandLine);
        auto args = getCommandArgs(commandLine);
        
        // Find handler for command
        auto handlerIt = handlers_.find(commandName);
        if (handlerIt != handlers_.end()) {
            return handlerIt->second(args, client);
        }
        
        return nullptr;
    }

    std::string CommandRouter::getCommandName(const std::string& commandLine) {
        std::istringstream iss(commandLine);
        std::string commandName;
        iss >> commandName;
        return commandName;
    }

    std::vector<std::string> CommandRouter::getCommandArgs(const std::string& commandLine) {
        std::istringstream iss(commandLine);
        std::string token;
        std::vector<std::string> args;
        
        // Skip the command name
        iss >> token;
        
        // Get remaining tokens as arguments
        while (iss >> token) {
            args.push_back(token);
        }
        
        return args;
    }

    std::vector<std::string> CommandRouter::getCommandNames() const {
        std::vector<std::string> names;
        names.reserve(handlers_.size());
        for (const auto& [name, _] : handlers_) {
            names.push_back(name);
        }
        return names;
    }

    std::unique_ptr<Command> CommandRouter::createCommand(const std::string& command,
                                                        const std::vector<std::string>& tokens,
                                                        ClientConnection* client) const {
        auto handlerIt = handlers_.find(command);
        if (handlerIt != handlers_.end()) {
            return handlerIt->second(tokens, client);
        }
        return nullptr;
    }
} 
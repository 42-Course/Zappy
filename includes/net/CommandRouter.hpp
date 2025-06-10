#pragma once

#include "commands/Command.hpp"
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include "net/ClientConnection.hpp"

namespace Zappy {

    // Define command handler type as a function that takes tokens and client
    using CommandHandler = std::function<std::unique_ptr<Command>(const std::vector<std::string>&, ClientConnection*)>;

    class CommandRouter {
    public:
        CommandRouter();
        ~CommandRouter();

        // Register/unregister command handlers
        void registerHandler(const std::string& command, CommandHandler handler);
        void unregisterHandler(const std::string& command);

        // Route and execute commands
        std::unique_ptr<Command> routeCommand(const std::string& commandLine, ClientConnection* client);

        // Command parsing
        static std::string getCommandName(const std::string& commandLine);
        static std::vector<std::string> getCommandArgs(const std::string& commandLine);

    private:
        std::map<std::string, CommandHandler> handlers_;
    };
} 
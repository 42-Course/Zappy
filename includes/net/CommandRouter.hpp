#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include "commands/Command.hpp"
#include "net/ClientConnection.hpp"

namespace Zappy {

    using CommandHandler = std::function<std::unique_ptr<Command>(const std::string&, ClientConnection*)>;

    class CommandRouter {
    public:
        CommandRouter();
        ~CommandRouter();

        // Command registration
        void registerHandler(const std::string& command, CommandHandler handler);
        void unregisterHandler(const std::string& command);

        // Command routing
        std::unique_ptr<Command> routeCommand(const std::string& commandLine, ClientConnection* client);

        // Command parsing
        static std::string getCommandName(const std::string& commandLine);
        static std::vector<std::string> getCommandArgs(const std::string& commandLine);

    private:
        std::unordered_map<std::string, CommandHandler> handlers_;
    };
} 
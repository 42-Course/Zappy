// includes/commands/CommandDispatcher.hpp
#pragma once

// #include "commands/CommandHandler.hpp"
#include "commands/ICommand.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace Zappy {
    class ClientConnection;

    using CommandHandler = std::function<std::unique_ptr<ICommand>(const std::vector<std::string>&, ClientConnection*)>;

    class CommandDispatcher {
    public:
        void registerHandler(const std::string& name, CommandHandler handler);
        std::unique_ptr<ICommand> dispatch(const std::string& line, ClientConnection* client) const;

        std::vector<std::string> getCommandNames() const;

        std::unique_ptr<ICommand> createCommand(const std::string& name,
                                        const std::vector<std::string>& tokens = {},
                                        ClientConnection* client = nullptr) const;

        static std::string extractCommandName(const std::string& line);
        
    private:
        std::unordered_map<std::string, CommandHandler> handlers_;
    };
}

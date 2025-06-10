#pragma once

#include "commands/Command.hpp"
#include "net/CommandRouter.hpp"
#include "net/ClientConnection.hpp"

namespace Zappy {
    class Engine;  // Forward declaration

    class ServerCommand : public Command {
    public:
        ServerCommand(const std::string& name, Engine& engine, ClientConnection* client = nullptr);
        virtual ~ServerCommand() = default;

        // Additional server command functionality
        virtual std::string getDescription() const = 0;
        virtual std::string getUsage() const = 0;

    protected:
        Engine& engine_;
    };
} 
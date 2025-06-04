#pragma once

#include <string>
#include <vector>
#include "Command.hpp"
#include "net/CommandRouter.hpp"

namespace Zappy {
    class Engine;  // Forward declaration

    class ServerCommand : public Command {
    public:
        ServerCommand(Engine& engine, const std::string& commandLine);
        virtual ~ServerCommand() = default;

        // Pure virtual methods that each server command must implement
        virtual void execute() override = 0;
        virtual std::string getDescription() const = 0;
        virtual std::string getUsage() const = 0;

    protected:
        Engine& engine_;
    };
} 
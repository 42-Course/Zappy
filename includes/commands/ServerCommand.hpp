#pragma once

#include <string>
#include <vector>
#include "Command.hpp"

namespace Zappy {
    class Engine;  // Forward declaration

    class ServerCommand : public Command {
    public:
        ServerCommand(const std::string& name, Engine* engine);
        virtual ~ServerCommand() = default;

        // Pure virtual method that each server command must implement
        virtual void execute() = 0;
        virtual std::string getDescription() const = 0;

    protected:
        Engine* engine_;
        std::vector<std::string> args_;
    };
} 
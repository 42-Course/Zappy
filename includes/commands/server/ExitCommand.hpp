#pragma once

#include "commands/ServerCommand.hpp"

namespace Zappy {
    class ExitCommand : public ServerCommand {
    public:
        explicit ExitCommand(Engine& engine);
        void execute() override;
        std::string getDescription() const override {
            return "Stop the server and exit";
        }
        std::string getUsage() const override {
            return "exit - Gracefully shutdown the server";
        }
    };
} 
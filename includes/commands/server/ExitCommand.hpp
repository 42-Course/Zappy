#pragma once

#include "commands/ServerCommand.hpp"

namespace Zappy {
    class ExitCommand : public ServerCommand {
    public:
        ExitCommand(Engine* engine);
        void execute() override;
        std::string getDescription() const override {
            return "Stop the server and exit";
        }
    };
} 
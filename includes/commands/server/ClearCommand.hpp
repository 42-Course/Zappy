#pragma once

#include "commands/ServerCommand.hpp"

namespace Zappy {

class ClearCommand : public ServerCommand {
public:
    explicit ClearCommand(Engine& engine);
    ~ClearCommand() = default;

    void execute() override;
    std::string getDescription() const override;
    std::string getUsage() const override;
};

} // namespace Zappy 
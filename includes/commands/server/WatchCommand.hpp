#pragma once

#include "commands/ServerCommand.hpp"
#include "services/WatchService.hpp"

namespace Zappy {

class WatchCommand : public ServerCommand {
public:
    WatchCommand(Engine& engine, const std::string& commandLine, WatchService& watchService);
    ~WatchCommand() = default;

    void execute() override;
    std::string getDescription() const override;
    std::string getUsage() const override;

private:
    WatchService& _watchService;
};

} // namespace Zappy 
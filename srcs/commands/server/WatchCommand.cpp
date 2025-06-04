#include "commands/server/WatchCommand.hpp"
#include <iostream>

namespace Zappy {

WatchCommand::WatchCommand(Engine& engine, const std::string& commandLine, WatchService& watchService)
    : ServerCommand(engine, commandLine)
    , _watchService(watchService)
{}

void WatchCommand::execute() {
    if (!args_.empty()) {
        if (args_[0] == "off") {
            _watchService.disable();
            return;
        } else if (args_[0] == "on") {
            _watchService.enable();
            return;
        }
    }
    
    // Toggle watch mode if no argument is provided
    if (_watchService.isEnabled()) {
        _watchService.disable();
    } else {
        _watchService.enable();
    }
}

std::string WatchCommand::getDescription() const {
    return "Toggle watch mode to monitor server status in real-time";
}

std::string WatchCommand::getUsage() const {
    return "watch [on|off] - Toggle watch mode or explicitly enable/disable it";
}

} // namespace Zappy 
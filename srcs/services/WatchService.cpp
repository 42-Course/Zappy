#include "services/WatchService.hpp"
#include "core/Engine.hpp"

#include <iostream>

namespace Zappy {

WatchService::WatchService(Engine& engine)
    : _engine(engine)
    , _enabled(false)
{}

void WatchService::enable() {
    _enabled = true;
    std::cout << "\033[32mWatch mode enabled\033[0m" << std::endl;
}

void WatchService::disable() {
    _enabled = false;
    std::cout << "\033[31mWatch mode disabled\033[0m" << std::endl;
}

void WatchService::clearScreen() const {
    // Clear screen using ANSI escape code
    std::cout << "\033[2J\033[H";
}

void WatchService::tick() {
    if (_enabled) {
        clearScreen();
        printServerInfo();
    }
}

bool WatchService::isEnabled() const {
    return _enabled;
}

void WatchService::printServerInfo() const {
    std::cout << "\033[1;34m=== Server Status ===\033[0m\n";
    // TODO: Add more detailed server info from engine
    std::cout << "Time: " << _engine.getGameLoop().getDeltaTime() << "ms\n";
    std::cout << "Tick: " << _engine.getGameLoop().getCurrentTick() << "\n";
    // Add more status information here
}

} // namespace Zappy 
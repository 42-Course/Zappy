#include "services/WatchService.hpp"
#include "core/Engine.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

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
    // Calculate server life time in seconds based on ticks and tick rate
    double totalSeconds = static_cast<double>(_engine.getGameLoop().getCurrentTick()) / _engine.getGameLoop().getTickRate();
    
    // Calculate hours, minutes, seconds
    int hours = static_cast<int>(totalSeconds) / 3600;
    int minutes = (static_cast<int>(totalSeconds) % 3600) / 60;
    int seconds = static_cast<int>(totalSeconds) % 60;

    // Get current time
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTimeT = std::chrono::system_clock::to_time_t(currentTime);
    
    std::cout << "\033[1;34m=== Zappy Server Status ===\033[0m\n\n";
    
    // Server Information
    std::cout << "\033[1;36mServer Information:\033[0m\n";
    std::cout << "* Server Life:\t" << std::setfill('0') << std::right << std::setw(2) << hours << ":"
                                  << std::setfill('0') << std::right << std::setw(2) << minutes << ":"
                                  << std::setfill('0') << std::setw(2) << seconds << "\n";
    std::cout << "* Current Time:\t" << std::ctime(&currentTimeT);
    std::cout << "* Delta Time:\t" << _engine.getGameLoop().getDeltaTime() << "ms\n";
    std::cout << "* Current Tick:\t" << _engine.getGameLoop().getCurrentTick() << "\n";
    std::cout << "* Tick Rate:\t" << _engine.getGameLoop().getTickRate() << " ticks/second\n\n";
    
    // World Information
    const auto& world = _engine.getWorld();
    std::cout << "\033[1;36mWorld Information:\033[0m\n";
    std::cout << "* Map Size:\t" << world.getMap().getWidth() << "x" << world.getMap().getHeight() << "\n";
    std::cout << "* Total Teams:\t" << world.getTeams().size() << "\n\n";
    
    // Team Information
    std::cout << "\033[1;36mTeam Status:\033[0m\n";
    for (const auto& team : world.getTeams()) {
        std::cout << "* " << std::left << std::setfill(' ') << std::setw(20) << team->getName() 
                  << ": " << team->getCurrentPlayers() << " players\n";
    }
    std::cout << "\n";
    
    // Network Information
    const auto& network = _engine.getNetwork();
    int totalClients = 0;
    int playerCount = 0;
    int spectatorCount = 0;
    
    // Count client types
    for (const auto& client : network.getClientManager().getClients()) {
        totalClients++;
        if (client.second->getType() == ClientConnection::Type::Spectator) {
            spectatorCount++;
        } else if (client.second->getType() == ClientConnection::Type::Player) {
            playerCount++;
        }
    }
    
    std::cout << "\033[1;36mNetwork Status:\033[0m\n";
    std::cout << "* Total Clients:\t" << totalClients << "\n";
    std::cout << "* Players:\t" << playerCount << "\n";
    std::cout << "* Spectators:\t" << spectatorCount << "\n";
    
    std::cout << "\n\033[90mType 'watch' or press 'Ctl + C' to disable monitoring\033[0m\n";
}

} // namespace Zappy 
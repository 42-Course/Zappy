// Engine.hpp - Core game engine coordinator
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include "World.hpp"
#include "net/NetworkManager.hpp"
#include "GameLoop.hpp"
#include "Config.hpp"

namespace Zappy {
    class Engine {
    public:
        Engine(const Config& config);
        ~Engine();
        
        void start();
        void stop();
        
        // Accessors for subsystems
        World& getWorld() { return *world_; }
        NetworkManager& getNetwork() { return *network_; }
        GameLoop& getGameLoop() { return *gameLoop_; }
        
        bool isRunning() const { return running_; }
        
    private:
        std::unique_ptr<World> world_;
        std::unique_ptr<NetworkManager> network_;
        std::unique_ptr<GameLoop> gameLoop_;
        bool running_;
    };
} 
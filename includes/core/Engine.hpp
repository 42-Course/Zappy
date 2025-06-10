// Engine.hpp - Core game engine coordinator
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "World.hpp"
#include "net/NetworkManager.hpp"
#include "GameLoop.hpp"
#include "Config.hpp"
#include "services/WatchService.hpp"

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
        WatchService& getWatchService() { return *watchService_; }
        
        bool isRunning() const { return running_; }
        
        // Signal handling
        static void setupSignalHandlers();
        static void handleSignal(int signal);
        static Engine* getInstance() { return instance_; }
        
    private:
        void registerCommands();
        
        std::unique_ptr<World> world_;
        std::unique_ptr<NetworkManager> network_;
        std::unique_ptr<GameLoop> gameLoop_;
        std::unique_ptr<WatchService> watchService_;
        bool running_;
        
        // Singleton instance for signal handling
        static Engine* instance_;
    };
} 
#pragma once

#include <functional>
#include <chrono>
#include <string>

namespace Zappy {
    class GameLoop {
    public:
        GameLoop(int tickRate = 100);  // Default 100 ticks per second
        ~GameLoop();

        void start();
        void stop();
        void setTickRate(int ticksPerSecond);
        int getTickRate() const { return tickRate_; }
        
        // Register update callbacks
        void onUpdate(std::function<void()> callback);
        void onFixedUpdate(std::function<void()> callback);
        
        // Time management
        double getDeltaTime() const { return deltaTime_; }
        unsigned int getCurrentTick() const { return currentTick_; }
        bool isRunning() const { return running_; }
        
        // Serialization helpers for network responses
        std::string toSgtString() const; // Time
    private:
        void run();
        void calculateDeltaTime();
        
        bool running_;
        int tickRate_;
        unsigned int currentTick_;
        double deltaTime_;
        double accumulator_;
        std::chrono::steady_clock::time_point lastUpdate_;
        
        std::function<void()> updateCallback_;
        std::function<void()> fixedUpdateCallback_;
    };
} 
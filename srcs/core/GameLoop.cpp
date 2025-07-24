#include "core/GameLoop.hpp"
#include <stdexcept>
#include <thread>

namespace Zappy {
GameLoop::GameLoop(int tickRate)
    : running_(false), tickRate_(tickRate), currentTick_(0), deltaTime_(0.0), accumulator_(0.0),
      updateCallback_(nullptr), fixedUpdateCallback_(nullptr) {
  if (tickRate <= 0) {
    throw std::invalid_argument("Tick rate must be positive");
  }
}

GameLoop::~GameLoop() {
  stop();
}

void GameLoop::start() {
  if (running_)
    return;
  running_    = true;
  lastUpdate_ = std::chrono::steady_clock::now();
  run();
}

void GameLoop::stop() {
  running_ = false;
}

void GameLoop::setTickRate(int ticksPerSecond) {
  if (ticksPerSecond <= 0) {
    throw std::invalid_argument("Tick rate must be positive");
  }
  tickRate_ = ticksPerSecond;
}

void GameLoop::onUpdate(std::function<void()> callback) {
  updateCallback_ = std::move(callback);
}

void GameLoop::onFixedUpdate(std::function<void()> callback) {
  fixedUpdateCallback_ = std::move(callback);
}

void GameLoop::calculateDeltaTime() {
  auto now    = std::chrono::steady_clock::now();
  deltaTime_  = std::chrono::duration<double>(now - lastUpdate_).count();
  lastUpdate_ = now;
}

std::string GameLoop::toSgtString() const {
  return "sgt " + std::to_string(tickRate_) + "\n";
}

void GameLoop::run() {
  while (running_) {
    calculateDeltaTime();
    accumulator_ += deltaTime_;

    double fixedTimeStep = 1.0 / tickRate_;

    // Update as many times as necessary to catch up
    while (accumulator_ >= fixedTimeStep) {
      if (fixedUpdateCallback_) {
        fixedUpdateCallback_();
      }
      currentTick_++;
      accumulator_ -= fixedTimeStep;
      updateCallback_(); // world update callback
    }

    // // Variable update
    // if (updateCallback_) {
    //     updateCallback_();
    // }

    // Sleep to prevent CPU overuse
    if (deltaTime_ < fixedTimeStep) {
      std::this_thread::sleep_for(
        std::chrono::microseconds(static_cast<long>((fixedTimeStep - deltaTime_) * 1000000)));
    }
  }
}
} // namespace Zappy
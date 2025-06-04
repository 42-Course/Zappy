#pragma once

namespace Zappy {
class Engine;

class WatchService {
public:
    WatchService(Engine& engine);
    ~WatchService() = default;

    void enable();
    void disable();
    void tick();
    bool isEnabled() const;

private:
    void printServerInfo() const;
    void clearScreen() const;
    
    Engine& _engine;
    bool _enabled;
};

} // namespace Zappy 
#pragma once

#include <string>
#include <queue>
#include <memory>
#include <vector>
#include "core/Player.hpp"
#include "core/Spectator.hpp"

namespace Zappy {
    class ClientConnection {
    public:
        enum class Type {
            Unknown,
            Player,
            Spectator
        };

        enum class State {
            UNREGISTERED,  // Initial state
            TEAM_SELECTED, // Player has selected a team
            ACTIVE        // Fully registered and active
        };

        ClientConnection(int fd);
        ~ClientConnection();

        // Socket operations
        bool readData();
        bool sendData(const std::string& data);
        void close();
        
        // Client type management
        void setType(Type type);
        Type getType() const { return type_; }
        bool isAuthenticated() const { return type_ != Type::Unknown; }
        
        // Client state management
        void setState(State state) { state_ = state; }
        State getState() const { return state_; }
        bool canExecuteCommand(const std::string& command) const;
        
        // Buffer management
        bool hasCompleteCommand() const;
        std::string getNextCommand();
        
        // Accessors
        int getFd() const { return fd_; }
        Player* getPlayer() { return player_; }
        // Spectator* getSpectator() { return spectator_.get(); }
        void setPlayer(Player* player) { player_ = player; }
        // void setSpectator(std::unique_ptr<Spectator> spectator) { spectator_ = std::move(spectator); }

    private:
        static constexpr size_t BUFFER_SIZE = 4096;
        
        int fd_;
        Type type_;
        State state_;
        std::vector<char> readBuffer_;
        std::string commandBuffer_;
        std::queue<std::string> pendingCommands_;
        
        Player* player_; // owned by the World object
        // std::unique_ptr<Spectator> spectator_;
        
        void processBuffer();
        bool isCommandComplete(const std::string& cmd) const;
    };
}
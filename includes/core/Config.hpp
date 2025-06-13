#pragma once

#include <string>
#include <vector>

namespace Zappy {
    class Config {
    public:
        Config(int width = 10, int height = 10, int tickRate = 100,
              int playerPort = 4242, int spectatorPort = 8080,
              int maxPlayersPerTeam = 6)
            : width_(width)
            , height_(height)
            , tickRate_(tickRate)
            , playerPort_(playerPort)
            , spectatorPort_(spectatorPort)
            , maxPlayersPerTeam_(maxPlayersPerTeam) {
            // Add a default team for testing
            teams_.push_back("Team1");
        }

        // Getters
        int getMapWidth() const { return width_; }
        int getMapHeight() const { return height_; }
        int getTickRate() const { return tickRate_; }
        int getPlayerPort() const { return playerPort_; }
        int getSpectatorPort() const { return spectatorPort_; }
        int getMaxPlayersPerTeam() const { return maxPlayersPerTeam_; }
        bool isInfiniteMap() const { return infinite_map_; }
        const std::vector<std::string>& getTeams() const { return teams_; }

        // Setters
        void addTeam(const std::string& team) { teams_.push_back(team); }

    private:
        int width_;
        int height_;
        int tickRate_;
        int playerPort_;
        int spectatorPort_;
        int maxPlayersPerTeam_;
        bool infinite_map_;
        std::vector<std::string> teams_;
    };
} 
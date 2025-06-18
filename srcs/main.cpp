//***************************//
//*Template by pulgamecanica*//
//***************************//
    
extern "C" {
    #include <signal.h>
    #include <string.h>
    #include <fcntl.h>
    #include <stdlib.h>
    #include <sys/time.h>
}

#include "Zappy.inc"
#include "core/Engine.hpp"
#include "core/Config.hpp"
#include "services/Logger.hpp"

static void print_help() {
    std::cout << "Usage: ./Zappy -x width -y height -c #clients -n team [team2] ... [OPTIONS]\n" <<
        "\nAbout: " << "Zappy Game, it's a game about the " << BLUE << "Trantor" << ENDC <<
        " World\n       Trantorians live, collect food and stones and perform a ritual to win\n" <<
        "       This is the `server` where the world is managed, it's GOD!" << std::endl <<
        "\n[" << RED << "Mandatory OPTIONS" << ENDC << "]:" << std::endl <<
        "  -c <#clients>       number of players allowed per team" << std::endl <<
        "  -n team <team2> ... teams, this is, the teams that will exist on trantor" << std::endl <<
        "  -x <width>          refers to the initial horizontal size of trantor (width > 10)" << std::endl <<
        "  -y <height>         refers to the initial vertical size of trantor (height > 10)" << std::endl <<
        "\n[Extra OPTIONS]:, if a parameter is not between `[]` then it's mandatory\n" <<
        "  -t <ms>      set the time unit divider, it's 100 by default (100 frames per second)" << std::endl <<
        "               the higher this number, the faster the world will be (ms > 0)" << std::endl <<
        "  -f <file>    load configuration from file" << std::endl <<
        "  -l <acronym> default Zappy language <must be present on the conf.file>" << std::endl <<
        "  -P <port>    set the players port to something else than 4242" << std::endl <<
        "  -S <port>    set the spectators port to something else than 2121" << std::endl <<
        "  -h           display this help message" << std::endl;
}

// ssize_t gettimeofday_ms() {
//     struct timeval tv;

//     if(gettimeofday(&tv, NULL) == -1) {
//         throw std::runtime_error(std::string("gettimeofday()") + std::string(strerror(errno)));
//     }
//     return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
// }

int main(int argc, char *argv[])
{
    // Seed rand
    srand(time(NULL));
    int opt;
    int players_port(4242);
    int spectators_port(2121);
    int default_time(100);
    int w(20), h(20), num_players(3); // Just for testing purposes
    std::vector<std::string> teams;

    try {
        while ((opt = getopt(argc, argv, "x:y:c:n:t:f:l:P:S:h")) != -1) {
            switch (opt) {
            case 'x':
                w = std::stoi(optarg);
                if (w < 10)
                    throw std::runtime_error("width must be >= 10");
                break;
            case 'y':
                h = std::stoi(optarg);
                if (h < 10)
                    throw std::runtime_error("height must be >= 10");
                break;
            case 'c':
                num_players = std::stoi(optarg);
                if (num_players < 1)
                    throw std::runtime_error("number of players must be >= 1");
                break;
            case 'n':
                teams.push_back(optarg);
                break;
            case 't':
                default_time = std::stoi(optarg);
                if (default_time < 1)
                    throw std::runtime_error("time unit must be >= 1");
                break;
            case 'P':
                players_port = std::stoi(optarg);
                if (players_port < 1024 || players_port > 65535)
                    throw std::runtime_error("player port must be between 1024 and 65535");
                break;
            case 'S':
                spectators_port = std::stoi(optarg);
                if (spectators_port < 1024 || spectators_port > 65535)
                    throw std::runtime_error("spectator port must be between 1024 and 65535");
                break;
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            default:
                print_help();
                exit(EXIT_FAILURE);
            }
        }
    } catch (std::exception &e) {
        std::cerr << RED << "Error: " << e.what() << ENDC " ("
            << BLUE << "option[" << ENDC << (char)opt << BLUE  << "]"
            << " => Invalid Arg [" << RED << optarg << BLUE "]"
            << ENDC << ")" << std::endl;
        return (1);
    }
    if (w == -1 || h == -1) {
        std::cerr << RED << "Error:" << ENDC <<" [-x, -y] Width & Height must be provided" <<
            std::endl << argv[0] <<" -h for more information" << std::endl;
        return (1);
    }
    if (num_players == -1) {
        std::cerr << RED << "Error:" << ENDC <<" [-c] # Clients must be provided " <<
            std::endl << argv[0] <<" -h for more information" << std::endl;
        return (1);
    }
    if (teams.empty()) {
        {
            // Default teams
            teams.push_back("Team Rocket");
        }
    }

    try {
        // Initialize logger
        auto& logger = Zappy::Logger::getInstance();
        logger.info("Zappy Server starting up...");
        
        // Create configuration
        Zappy::Config config(w, h, default_time, players_port, spectators_port, num_players);
        logger.info("Configuration created: " + std::to_string(w) + "x" + std::to_string(h) + 
                   " map, " + std::to_string(num_players) + " players per team");
        
        // Add teams from command line
        for (const auto& team : teams) {
            config.addTeam(team);
            logger.info("Added team: " + team);
        }

        // Create and start engine
        logger.info("Initializing game engine...");
        Zappy::Engine engine(config);
        
        // Set up signal handlers
        logger.info("Setting up signal handlers...");
        Zappy::Engine::setupSignalHandlers();
        
        // Start the engine
        logger.info("Starting game engine...");
        engine.start();
        
        // The engine's GameLoop will handle the main loop until a signal is received
        // or the exit command is issued
        
        logger.info("Zappy Server shutting down normally");
        std::cout << YELLOW << "Thank you for using the " << GREEN "Zappy Server" << ENDC << " :)" << std::endl;
        return (0);
    } catch (std::exception &e) {
        Zappy::Logger::getInstance().error("Fatal error: " + std::string(e.what()));
        std::cerr << RED << e.what() << ENDC << std::endl;
        return (1);
    }
}

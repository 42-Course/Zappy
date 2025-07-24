//***************************//
//*Template by pulgamecanica*//
//***************************//

extern "C" {
#include <fcntl.h>
#include <sys/time.h>
}

#include <csignal>
#include <cstdlib>
#include <string>

#include "Zappy.inc"
#include "core/Config.hpp"
#include "core/Engine.hpp"
#include "services/Logger.hpp"

constexpr int DEFAULT_WIDTH          = 20;
constexpr int DEFAULT_HEIGHT         = 20;
constexpr int DEFAULT_CLIENTS        = 3;
constexpr int DEFAULT_TIME           = 100;
constexpr int DEFAULT_PLAYER_PORT    = 4242;
constexpr int DEFAULT_SPECTATOR_PORT = 2121;
constexpr int MIN_MAP_SIZE           = 10;
constexpr int MIN_PORT               = 1024;
constexpr int MAX_PORT               = 65535;

struct ServerOptions {
  int width           = DEFAULT_WIDTH;
  int height          = DEFAULT_HEIGHT;
  int num_players     = DEFAULT_CLIENTS;
  int players_port    = DEFAULT_PLAYER_PORT;
  int spectators_port = DEFAULT_SPECTATOR_PORT;
  int default_time    = DEFAULT_TIME;
  std::vector<std::string> teams;
};

static void print_help() {
  std::cout
    << "Usage: ./Zappy -x width -y height -c #clients -n team [team2] ... [OPTIONS]\n"
    << "\nAbout: "
    << "Zappy Game, it's a game about the " << BLUE << "Trantor" << ENDC
    << " World\n       Trantorians live, collect food and stones and perform a ritual to win\n"
    << "       This is the `server` where the world is managed, it's GOD!" << std::endl
    << "\n[" << RED << "Mandatory OPTIONS" << ENDC << "]:" << std::endl
    << "  -c <#clients>       number of players allowed per team" << std::endl
    << "  -n team <team2> ... teams, this is, the teams that will exist on trantor" << std::endl
    << "  -x <width>          refers to the initial horizontal size of trantor (width > 10)"
    << std::endl
    << "  -y <height>         refers to the initial vertical size of trantor (height > 10)"
    << std::endl
    << "\n[Extra OPTIONS]:, if a parameter is not between `[]` then it's mandatory\n"
    << "  -t <ms>      set the time unit divider, it's 100 by default (100 frames per second)"
    << std::endl
    << "               the higher this number, the faster the world will be (ms > 0)" << std::endl
    << "  -f <file>    load configuration from file" << std::endl
    << "  -l <acronym> default Zappy language <must be present on the conf.file>" << std::endl
    << "  -P <port>    set the players port to something else than 4242" << std::endl
    << "  -S <port>    set the spectators port to something else than 2121" << std::endl
    << "  -h           display this help message" << std::endl;
}

static ServerOptions parse_args(int argc, char **argv) {
  ServerOptions so;
  int opt;

  while ((opt = getopt(argc, argv, "x:y:c:n:t:f:l:P:S:h")) != -1) {
    switch (opt) {
    case 'x':
      so.width = std::stoi(optarg);
      if (so.width < MIN_MAP_SIZE) {
        throw std::runtime_error("width must be >= " + std::to_string(MIN_MAP_SIZE));
      }
      break;
    case 'y':
      so.height = std::stoi(optarg);
      if (so.height < MIN_MAP_SIZE)
        throw std::runtime_error("height must be >= " + std::to_string(MIN_MAP_SIZE));
      break;
    case 'c':
      so.num_players = std::stoi(optarg);
      if (so.num_players < 1)
        throw std::runtime_error("number of players must be >= 1");
      break;
    case 'n':
      so.teams.emplace_back(optarg);
      break;
    case 't':
      so.default_time = std::stoi(optarg);
      if (so.default_time < 1)
        throw std::runtime_error("time unit must be >= 1");
      break;
    case 'P':
      so.players_port = std::stoi(optarg);
      if (so.players_port < MIN_PORT || so.players_port > MAX_PORT)
        throw std::runtime_error("player port must be between " + std::to_string(MIN_PORT) +
                                 " and " + std::to_string(MAX_PORT));
      break;
    case 'S':
      so.spectators_port = std::stoi(optarg);
      if (so.spectators_port < MIN_PORT || so.spectators_port > MAX_PORT)
        throw std::runtime_error("spectator port must be between " + std::to_string(MIN_PORT) +
                                 " and " + std::to_string(MAX_PORT));
      break;
    case 'h':
      print_help();
      exit(EXIT_SUCCESS);
    default:
      print_help();
      exit(EXIT_FAILURE);
    }
  }
  return so;
}

int main(int argc, char *argv[]) {
  // Seed rand
  srand(time(nullptr));

  try {
    ServerOptions options = parse_args(argc, argv);

    if (options.width == -1 || options.height == -1) {
      std::cerr << RED << "Error:" << ENDC << " [-x, -y] Width & Height must be provided"
                << std::endl
                << argv[0] << " -h for more information" << std::endl;
      return (1);
    }

    if (options.num_players == -1) {
      std::cerr << RED << "Error:" << ENDC << " [-c] # Clients must be provided " << std::endl
                << argv[0] << " -h for more information" << std::endl;
      return (1);
    }

    if (options.teams.empty()) {
      {
        // Default teams
        options.teams.emplace_back("Team Rocket");
      }
    }

    // Initialize logger
    auto &logger = Zappy::Logger::getInstance();
    logger.info("Zappy Server starting up...");

    // Create configuration
    Zappy::Config config(options.width, options.height, options.default_time, options.players_port,
                         options.spectators_port, options.num_players);

    logger.info("Configuration created: " + std::to_string(options.width) + "x" +
                std::to_string(options.height) + " map, " + std::to_string(options.num_players) +
                " players per team");

    // Add teams from command line
    for (const auto &team : options.teams) {
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
    std::cout << YELLOW << "Thank you for using the " << GREEN "Zappy Server" << ENDC << " :)"
              << std::endl;
    return (0);
  } catch (std::exception &e) {
    Zappy::Logger::getInstance().error("Fatal error: " + std::string(e.what()));
    std::cerr << RED << e.what() << ENDC << std::endl;
    return (1);
  }
}

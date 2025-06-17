#include "commands/spectator/SGTCommand.hpp"
#include "core/GameLoop.hpp"
#include <sstream>

namespace Zappy {

SGTCommand::SGTCommand(GameLoop& gameLoop, const std::vector<std::string>& tokens, ClientConnection* client)
    : Command("sgt", CommandType::SPECTATOR, 0, client), gameLoop_(gameLoop) {
      if (!parseArgs(tokens))
        setStatus(CommandStatus::INVALID);
}

bool SGTCommand::parseArgs(const std::vector<std::string>& args) {
    return Command::parseArgs(args) && validateArgCount(0);
}

CommandStatus SGTCommand::execute() {
    std::stringstream ss;
    ss << "sgt " << gameLoop_.getTickRate() << "\n";
    getClient()->sendData(ss.str());
    logCommand("Sent current tick rate");
    return CommandStatus::COMPLETED;
}

}

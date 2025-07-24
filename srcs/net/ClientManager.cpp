#include "net/ClientManager.hpp"

namespace Zappy {

void ClientManager::addClient(int fd, std::unique_ptr<ClientConnection> client) {
  clients_[fd] = std::move(client);
}

ClientConnection *ClientManager::getClient(int fd) {
  auto it = clients_.find(fd);
  return (it != clients_.end()) ? it->second.get() : nullptr;
}

void ClientManager::removeClient(int fd) {
  clients_.erase(fd);
}

} // namespace Zappy

#pragma once

#include "net/ClientConnection.hpp"
#include <functional>

namespace Zappy {
  class ClientManager {
  public:
      void addClient(int fd, std::unique_ptr<ClientConnection> client);
      ClientConnection* getClient(int fd);
      void removeClient(int fd);
      const auto& getClients() const { return clients_; }

  private:
      std::map<int, std::unique_ptr<ClientConnection>> clients_;
  };
} 

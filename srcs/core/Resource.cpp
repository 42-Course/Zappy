#include "core/Resource.hpp"
#include <stdexcept>

namespace Zappy {
Resource::Resource(ResourceType type) : type_(type) {}

std::string Resource::getName() const {
  return typeToString(type_);
}

std::string Resource::typeToString(ResourceType type) {
  switch (type) {
  case ResourceType::FOOD:
    return "food";
  case ResourceType::LINEMATE:
    return "linemate";
  case ResourceType::DERAUMERE:
    return "deraumere";
  case ResourceType::SIBUR:
    return "sibur";
  case ResourceType::MENDIANE:
    return "mendiane";
  case ResourceType::PHIRAS:
    return "phiras";
  case ResourceType::THYSTAME:
    return "thystame";
  default:
    throw std::invalid_argument("Unknown resource type");
  }
}

ResourceType Resource::stringToType(const std::string &name) {
  if (name == "food")
    return ResourceType::FOOD;
  if (name == "linemate")
    return ResourceType::LINEMATE;
  if (name == "deraumere")
    return ResourceType::DERAUMERE;
  if (name == "sibur")
    return ResourceType::SIBUR;
  if (name == "mendiane")
    return ResourceType::MENDIANE;
  if (name == "phiras")
    return ResourceType::PHIRAS;
  if (name == "thystame")
    return ResourceType::THYSTAME;
  throw std::invalid_argument("Unknown resource name: " + name);
}
} // namespace Zappy
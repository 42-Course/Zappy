#include "core/Player.hpp"
#include "core/Inventory.hpp"
#include "core/Resource.hpp"
#include "core/Team.hpp"
#include <algorithm>

namespace Zappy {
Player::Player(int id, Team &team, int x, int y, Direction direction)
    : id_(id), team_(team), x_(x), y_(y), direction_(direction), level_(1), inventory_(),
      alive_(true), ticksSinceLastFood_(0), currentCommandTicks_(0) {
  inventory_.add(ResourceType::FOOD, 10);
  // for (int i = 0; i < INITIAL_FOOD; ++i)
  //     inventory_.addResource(Resource())
}

Player::~Player() {
  // Notify observers that player is being destroyed
  // notify([this](IObserver* obs) {
  //     obs->onPlayerDied(this);
  // });
}

void Player::notifyMoved() const {
  notify([this](IObserver *obs) { obs->onPlayerMoved(this); });
}

void Player::notifyTurned() const {
  notify([this](IObserver *obs) { obs->onPlayerTurned(this); });
}

void Player::notifyLevelUp() const {
  notify([this](IObserver *obs) { obs->onPlayerLevelUp(this); });
}

void Player::notifyInventoryChanged() const {
  notify([this](IObserver *obs) { obs->onPlayerInventoryChanged(this); });
}

void Player::notifyDied() const {
  notify([this](IObserver *obs) { obs->onPlayerDied(this); });
}

void Player::notifyDropedResource(ResourceType type) const {
  notify([this, type](IObserver *obs) { obs->onPlayerDropedResource(this, type); });
}

void Player::notifyTookResource(ResourceType type) const {
  notify([this, type](IObserver *obs) { obs->onPlayerTookResource(this, type); });
}

std::pair<int, int> Player::getPosition() const {
  return std::pair<int, int>(x_, y_);
}

void Player::setPosition(int x, int y) {
  if (!alive_)
    return;
  x_ = x;
  y_ = y;
  notifyMoved();
}

void Player::kick(Direction direction) {
  if (!alive_)
    return;
  std::pair<int, int> vec = {0, 0};

  switch (direction) {
  case Direction::NORTH:
    vec.second = -1;
    break;
  case Direction::EAST:
    vec.first = 1;
    break;
  case Direction::SOUTH:
    vec.second = 1;
    break;
  case Direction::WEST:
    vec.first = -1;
    break;
  }
  setPosition(x_ + vec.first, y_ + vec.second);
  // notifyKick();
}

void Player::setDirection(Direction direction) {
  if (!alive_)
    return;
  direction_ = direction;
  notifyTurned();
}

void Player::levelUp() {
  if (!alive_)
    return;
  level_++;
  notifyLevelUp();
}

void Player::addResource(ResourceType type, int amount) {
  if (!alive_ || amount <= 0)
    return;

  // Create a single resource and add it multiple times
  for (int i = 0; i < amount; i++) {
    if (!inventory_.add(type)) {
      break; // Stop if we can't add more
    }
  }
  notifyTookResource(type);
  notifyInventoryChanged();
}

bool Player::removeResource(ResourceType type, int amount) {
  if (!alive_ || amount <= 0)
    return false;

  if (inventory_.getCount(type) < amount)
    return false;
  // Create a single resource and remove it multiple times
  for (int i = 0; i < amount; i++) {
    if (!inventory_.remove(type)) {
      break; // Stop if we can't remove more
    }
  }
  notifyDropedResource(type);
  notifyInventoryChanged();
  return true;
}

void Player::enqueueCommand(std::unique_ptr<ICommand> command) {
  if (canQueueCommand()) {
    commandQueue_.push(std::move(command));
  }
}

void Player::die() {
  if (!alive_)
    return;
  alive_ = false;
  notifyDied();
}

void Player::broadcast(const std::string &message) {
  if (!alive_ || message.empty())
    return;

  // TODO: Implement broadcast through World/NetworkManager
  // - Send message to all players in range
  // - Handle message routing
}

bool Player::startIncantation() {
  if (!alive_)
    return false;

  // TODO: Implement elevation requirements check
  // - Check if player has required resources
  // - Check if other players are present and at correct level
  // - Check if tile has required resources
  return false;
}

void Player::fork() {
  if (!alive_)
    return;

  // TODO: Implement player reproduction
  // - Create new player in same team
  // - Place on same tile
  // - Initialize with basic resources
}

void Player::update() {
  if (!alive_)
    return;

  // Food consumption
  ++ticksSinceLastFood_;
  if (ticksSinceLastFood_ >= 126) {
    if (inventory_.getCount(ResourceType::FOOD) > 0) {
      inventory_.remove(ResourceType::FOOD, 1);
      ticksSinceLastFood_ = 0;
      notifyInventoryChanged();
    } else {
      die(); // No food left
      return;
    }
  }

  // Command execution
  if (!commandQueue_.empty()) {
    auto &cmd = commandQueue_.front();
    ++currentCommandTicks_;
    // possibly check if command is possible (ko? or should ko only be when finished)
    if (currentCommandTicks_ >= cmd->getTimeCost()) {
      if (cmd->execute() != CommandStatus::FAILED) {
        commandQueue_.pop();
        currentCommandTicks_ = 0;
      } else {
        // optionally log or handle "ko\n"
        commandQueue_.pop();
        currentCommandTicks_ = 0;
      }
    }
  }
  // TODO: Implement time-based updates
  // - Check for level up conditions
  // - Handle ongoing actions
}

std::string Player::toPnwString() const {
  return "pnw " + std::to_string(id_) + " " + std::to_string(x_) + " " + std::to_string(y_) + " " +
         std::to_string(static_cast<int>(direction_)) + " " + std::to_string(level_) + " " +
         team_.getName() + "\n";
}

std::string Player::toPpoString() const {
  return "ppo " + std::to_string(id_) + " " + std::to_string(x_) + " " + std::to_string(y_) + " " +
         std::to_string(static_cast<int>(direction_)) + "\n";
}

std::string Player::toPlvString() const {
  return "plv " + std::to_string(id_) + " " + std::to_string(level_) + "\n";
}

std::string Player::toPinString() const {
  std::string result =
    "pin " + std::to_string(id_) + " " + std::to_string(x_) + " " + std::to_string(y_);
  for (int i = 0; i < static_cast<int>(ResourceType::THYSTAME); ++i) {
    result += " " + std::to_string(inventory_.getCount(static_cast<ResourceType>(i)));
  }
  result += "\n";
  return result;
}

std::string Player::toPexString() const {
  return "pex " + std::to_string(id_) + "\n";
}

std::string Player::toPbcString(const std::string &message) const {
  return "pbc " + std::to_string(id_) + " " + message + "\n";
}

std::string Player::toString() const {
  return "P(" + std::to_string(id_) + ") - (" + team_.getName() + ")";
}

} // namespace Zappy
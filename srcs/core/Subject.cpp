#include "core/Subject.hpp"

namespace Zappy {
void Subject::attach(IObserver *observer) {
  observers_.push_back(observer);
}

void Subject::detach(IObserver *observer) {
  observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
}

void Subject::notify(const std::function<void(IObserver *)> &callback) const {
  for (auto *observer : observers_) {
    callback(observer);
  }
}
} // namespace Zappy
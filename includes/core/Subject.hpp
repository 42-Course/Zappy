#pragma once

#include "ISubject.hpp"
#include "IObserver.hpp"
#include <algorithm>
#include <functional>
#include <vector>

namespace Zappy {
    class Subject : public ISubject {
    public:
        void attach(IObserver* observer) override;
        void detach(IObserver* observer) override;

    protected:
        void notify(const std::function<void(IObserver*)>& callback) const;

    private:
        std::vector<IObserver*> observers_;
    };
}

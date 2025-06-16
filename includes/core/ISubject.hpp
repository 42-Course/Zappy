#pragma once

namespace Zappy {
    class IObserver;

    class ISubject {
    public:
        virtual ~ISubject() = default;

        virtual void attach(IObserver* observer) = 0;
        virtual void detach(IObserver* observer) = 0;
    };
}

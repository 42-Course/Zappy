#pragma once

#include <string>

namespace Zappy {
    enum class ResourceType {
        FOOD,
        LINEMATE,
        DERAUMERE,
        SIBUR,
        MENDIANE,
        PHIRAS,
        THYSTAME
    };

    class Resource {
    public:
        Resource(ResourceType type);
        ~Resource() = default;

        ResourceType getType() const { return type_; }
        std::string getName() const;

        static std::string typeToString(ResourceType type);
        static ResourceType stringToType(const std::string& name);

    private:
        ResourceType type_;
    };
} 
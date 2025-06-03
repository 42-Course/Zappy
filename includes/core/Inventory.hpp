#pragma once

#include <map>
#include <memory>

namespace Zappy {
    // class Resource;

    class Inventory {
    public:
        Inventory();
        ~Inventory();

        // Resource operations
        // void addResource(Resource* resource);
        // bool removeResource(Resource* resource);
        int getResourceCount(const std::string& type) const;
        bool hasResources(const std::map<std::string, int>& requirements) const;

        // Getters
        const std::map<std::string, int>& getContents() const { return contents_; }

    private:
        std::map<std::string, int> contents_;  // Maps resource type to count
    };
} 
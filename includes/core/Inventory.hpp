#pragma once

#include <map>
#include <string>
#include <algorithm>

namespace Zappy {
    // class Resource;

    class Inventory {
    public:
        Inventory();
        ~Inventory();

        // Resource operations
        // void addResource(Resource* resource);
        // bool removeResource(Resource* resource);
        void add(const std::string& resource, int amount);
        void remove(const std::string& resource, int amount);
        int get(const std::string& resource) const;
        void set(const std::string& resource, int amount);
        int getResourceCount(const std::string& type) const;
        bool hasResources(const std::map<std::string, int>& requirements) const;

        // Getters
        const std::map<std::string, int>& getContents() const;

    private:
        std::map<std::string, int> contents_;  // Maps resource type to count
    };
} 
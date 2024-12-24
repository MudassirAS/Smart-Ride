#ifndef HASHMAP_H
#define HASHMAP_H

#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

template<typename J>

// Custom HashMap class to store JSON objects
class HashMap {
private:
    vector<json> map; // A vector to store JSON objects

public:
    // Add a JSON object to the HashMap
    void put(const json& value) {
        map.push_back(value);
    }

    // Retrieve a JSON object by index
    json get(size_t index) const {
        if (index < map.size()) {
            return map[index];
        }
        throw runtime_error("Index out of bounds.");
    }

    // Check if the HashMap contains a specific JSON object
    bool contains(const json& value) const {
        return find(map.begin(), map.end(), value) != map.end();
    }

    // Remove a JSON object by index
    void remove(size_t index) {
        if (index < map.size()) {
            map.erase(map.begin() + index);
        } else {
            throw runtime_error("Index out of bounds.");
        }
    }

    // Retrieve the size of the HashMap
    size_t size() const {
        return map.size();
    }

    // Check if the HashMap is empty
    bool isEmpty() const {
        return map.empty();
    }

    // Clear all elements in the HashMap
    void clear() {
        map.clear();
    }

    // Retrieve all entries as a vector
    vector<json> entries() const {
        return map;
    }
    
    void update(size_t index, const json& newValue) {
    if (index < map.size()) {
        map[index] = newValue;
    } else {
        throw runtime_error("Index out of bounds.");
    }
}

   void replaceAll(const json& newEntries) {
    if (!newEntries.is_array()) {
        throw runtime_error("Input must be a JSON array.");
    }
    map = newEntries.get<vector<json>>();
}


    // Iterator support for range-based loops
    typename vector<json>::iterator begin() {
        return map.begin();
    }

    typename vector<json>::iterator end() {
        return map.end();
    }

    typename vector<json>::const_iterator begin() const {
        return map.begin();
    }

    typename vector<json>::const_iterator end() const {
        return map.end();
    }
};

#endif

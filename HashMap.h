#ifndef HASHMAP_H
#define HASHMAP_H

// Custom HashMap class (mimics an unordered_map with additional methods if needed)
template<typename K, typename V>
class HashMap {
private:
    unordered_map<K, V> map;

public:
    void put(const K& key, const V& value) {
        map[key] = value;
    }

    V get(const K& key) const {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        throw runtime_error("Key not found.");
    }

    bool containsKey(const K& key) const {
        return map.find(key) != map.end();
    }

    void remove(const K& key) {
        map.erase(key);
    }

    typename unordered_map<K, V>::iterator begin() {
        return map.begin();
    }

    typename unordered_map<K, V>::iterator end() {
        return map.end();
    }
};


#endif
#ifndef JSON_MANAGER_H
#define JSON_MANAGER_H


#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "HashMap.h"

using namespace std;
using json = nlohmann::json;

const string USERS_FILE = "users.json";
const string DRIVERS_FILE = "drivers.json";
const string RIDE_REQUESTS_FILE = "rideRequests.json";
const string RIDE_HISTORY = "rideHistory.json";
const string RIDE_REQUEST_QUEUE = "rideRequestQueue.json";

HashMap<json> usersMap;
HashMap<json> driversMap;
HashMap<json> rideRequestMap;
HashMap<json> rideHistoryMap;
HashMap<json> rideRequestQueueMap;

// Load JSON data from a file
json loadFromFile(const string& fileName) {
    ifstream file(fileName);

    // If the file does not exist, create one
    if (!file.is_open()) {
        ofstream createFile(fileName); // Create the file
        if (!createFile.is_open()) {
            throw runtime_error("Failed to create the file: " + fileName);
        }
        createFile << json::array().dump(); // Write an empty JSON array to the file
        createFile.close();
        return json::array(); // Return an empty array
    }

    json data;
    file >> data;
    return data;
}


// Save JSON data to a file
void saveToFile(const string& fileName, const HashMap<json>& map) {
    ofstream outFile(fileName);
    if (outFile.is_open()) {
        json dataArray = json::array();
        for (const auto& entry : map.entries()) {
            dataArray.push_back(entry);
        }
        outFile << dataArray.dump(4); // Pretty indentation
        outFile.close();
    } else {
        cout << "Error opening file for writing." << endl;
    }
}

// Load JSON files into HashMaps
void initialization() {
    auto loadToHashMap = [](const string& fileName, HashMap<json>& hashMap) {
        json jsonArray = loadFromFile(fileName);
        for (const auto& obj : jsonArray) {
            hashMap.put(obj); // Add directly to the HashMap
        }
    };

    loadToHashMap(USERS_FILE, usersMap);
    loadToHashMap(DRIVERS_FILE, driversMap);
    loadToHashMap(RIDE_REQUESTS_FILE, rideRequestMap);
    loadToHashMap(RIDE_HISTORY, rideHistoryMap);
    loadToHashMap(RIDE_REQUEST_QUEUE, rideRequestQueueMap);
}

#endif

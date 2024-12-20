#ifndef json_Files
#define json_Files

#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

const string USERS_FILE = "users.json";
const string DRIVERS_FILE = "drivers.json";
const string RIDE_REQUESTS_FILE = "rideRequests.json";
const string RIDE_HISTORY = "rideHistory.json";

json usersArray; 
json driversArray;
json rideRequestArray;
json rideHistory;

int userIDCounter = 1;
int driverIDCounter = 1;


// Load JSON data from a file
json loadFromFile(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        return json::array(); // Return an empty array if file does not exist
    }

    json data;
    file >> data;
    return data;
}

// Save JSON data to a file
void saveToFile(const string& fileName, const json& jsonData) {
    ofstream file(fileName);
    if (file.is_open()) {
        file << jsonData.dump(4); // Pretty print JSON with indentation
    } else {
        cerr << "Error saving to " << fileName << endl;
    }
}

// Load JSON Files
void initialization() {
    usersArray = loadFromFile(USERS_FILE);
    driversArray = loadFromFile(DRIVERS_FILE);
    rideRequestArray = loadFromFile(RIDE_REQUESTS_FILE);
    rideHistory= loadFromFile(RIDE_HISTORY);

    // Determine the next available IDs
    for (const auto& user : usersArray) {
        string userID = user["UserID"];
        userIDCounter = max(userIDCounter, stoi(userID.substr(1)) + 1);
    }

    for (const auto& driver : driversArray) {
        string driverID = driver["DriverID"];
        driverIDCounter = max(driverIDCounter, stoi(driverID.substr(1)) + 1);
    }
}

#endif
# ifndef MYJSON_H
# define MYJSON_H

#include <iostream>
#include <fstream>
#include "json.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include "RideRequest.h"

using namespace std;

using json = nlohmann::json;

class MainMenu {
private:
    static const string USERS_FILE;
    static const string DRIVERS_FILE;
    static const string DRIVER_LOCATIONS_FILE;

    static json usersArray; // Stores all user entries
    static json driversArray; // Stores all driver entries
    static json driverLocationsArray; // Stores driver locations

    static int userIDCounter;
    static int driverIDCounter;

public:
    // Load JSON data from a file
    static json loadFromFile(const string& fileName) {
        ifstream file(fileName);
        if (!file.is_open()) {
            return json::array(); // Return an empty array if file does not exist
        }

        json data;
        file >> data;
        return data;
    }

    // Save JSON data to a file
    static void saveToFile(const string& fileName, const json& jsonData) {
        ofstream file(fileName);
        if (file.is_open()) {
            file << jsonData.dump(4); // Pretty print JSON with indentation
        } else {
            cerr << "Error saving to " << fileName << endl;
        }
    }

    // Register a new user
    static void registerUser(const string& name, const string& password, const string& phoneNo, const string& email) {
        string userID = "U" + to_string(userIDCounter++);
        json user = {
            {"UserID", userID},
            {"Name", name},
            {"Password", password},
            {"PhoneNo", phoneNo},
            {"Email", email}
        };

        usersArray.push_back(user);
        saveToFile(USERS_FILE, usersArray);
        cout << "User registered successfully with UserID: " << userID << endl;
    }

    // Register a new driver
    static void registerDriver(const string& name, const string& password, const string& phoneNo, const string& vehicleName, const string& vehicleRegNumber, float fuelAvg) {
        string driverID = "D" + to_string(driverIDCounter++);
        json driver = {
            {"DriverID", driverID},
            {"Name", name},
            {"Password", password},
            {"PhoneNo", phoneNo},
            {"VehicleName", vehicleName},
            {"VehicleRegNumber", vehicleRegNumber},
            {"FuelAverage", fuelAvg}
        };

        driversArray.push_back(driver);
        saveToFile(DRIVERS_FILE, driversArray);
        cout << "Driver registered successfully with DriverID: " << driverID << endl;
    }

    // User login
    static bool userLogin(const string& userID, const string& password) {
        int choice; 
        string source = "" , destination = "";
 
        for (const auto& user : usersArray) {
            if (user["UserID"] == userID && user["Password"] == password) {
                cout << "User login successful.\n" << endl;
                cout << "\nDo you need a ride ? (1 / 0) : ";
                cin >> choice;
                if (choice == 1){     

                cout << "\nEnter your 'starting (current)' location : ";
                cin >> source;
                cout << "\nEnter your 'end (destination)' location : ";
                cin >> destination;
                
                RideRequest::requestRide(source, destination);

                }
                return true;
            }
        }
        cout << "Invalid UserID or password." << endl;
        return false;
    }

    // Driver login
    static bool driverLogin(const string& driverID, const string& password) {
        for (auto& driver : driversArray) {
            if (driver["DriverID"] == driverID && driver["Password"] == password) {
                cout << "Driver login successful." << endl;

                // Prompt driver to set location
                string location;
                cout << "Enter your current location: ";
                getline(cin, location);

                setDriverLocation(driverID, location);
                return true;
            }
        }
        cout << "Invalid DriverID or password." << endl;
        return false;
    }

    // Set driver location
    static void setDriverLocation(const string& driverID, const string& location) {
        driverLocationsArray = loadFromFile(DRIVER_LOCATIONS_FILE);

        // Check if the driver already has a location entry
        bool driverExists = false;
        for (auto& entry : driverLocationsArray) {
            if (entry["DriverID"] == driverID) {
                entry["Location"] = location; // Update location
                driverExists = true;
                break;
            }
        }

        // If the driver doesn't have an entry, create a new one
        if (!driverExists) {
            json newLocationEntry = {
                {"DriverID", driverID},
                {"Location", location}
            };
            driverLocationsArray.push_back(newLocationEntry);
        }

        saveToFile(DRIVER_LOCATIONS_FILE, driverLocationsArray);
        cout << "Location updated successfully for DriverID: " << driverID << endl;
    }

    // Begin method
    static void Begin() {
        usersArray = loadFromFile(USERS_FILE);
        driversArray = loadFromFile(DRIVERS_FILE);

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
};

// Static member variables initialization
const string MainMenu::USERS_FILE = "users.json";
const string MainMenu::DRIVERS_FILE = "drivers.json";
const string MainMenu::DRIVER_LOCATIONS_FILE = "driverLocations.json";

json MainMenu::usersArray;
json MainMenu::driversArray;
json MainMenu::driverLocationsArray;

int MainMenu::userIDCounter = 1;
int MainMenu::driverIDCounter = 1;

# endif
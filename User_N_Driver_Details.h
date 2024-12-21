# ifndef User_N_DRIVER_DETAILS
# define User_N_DRIVER_DETAILS

#include <iostream>
#include <iomanip>
#include <fstream>
#include "json.hpp"
#include <string>
#include <vector>
#include <cstdlib>
#include "RideRequest.h"
#include "Json_Manager.h"
#include "getterFunctions.h"

using namespace std;
using json = nlohmann::json;

// Register a new user
void registerUser(const string& name, const string& password, const string& phoneNo, const string& email) {
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
void registerDriver(const string& name, const string& password, const string& phoneNo, const string& vehicleName, const string& vehicleRegNumber, float fuelAvg) {
    string driverID = "D" + to_string(driverIDCounter++);
    json driver = {
        {"DriverID", driverID},
        {"Name", name},
        {"Password", password},
        {"PhoneNo", phoneNo},
        {"VehicleName", vehicleName},
        {"VehicleRegNumber", vehicleRegNumber},
        {"FuelAverage", fuelAvg},
        {"Location", ""}
    };

    driversArray.push_back(driver);
    saveToFile(DRIVERS_FILE, driversArray);
    cout << "Driver registered successfully with DriverID: " << driverID << endl;
}

// User login
bool userLogin(const string& username, const string& password) {
    for (const auto& user : usersArray) {
        if (user["Name"] == username && user["Password"] == password) {
            cout << "User login successful.\n" << endl;
            cout << "\nDo you need a ride ? (1 / 0) : ";
            int choice; 
            cin >> choice;
            if (choice == 1)
                requestRide(username);

            return true;
        }
    }
    return false;
}

// Set driver location
void setDriverLocation(const string& username) {
    
    Graph map;
    map.initializeGraph(map);
    
    cout << "Select your current location from the list below:\n" << endl;
    map.displayLocations();
    int size = map.sizeofMap(), locationIndex;

    while (true) {
        cout << "\nEnter the number corresponding to your location: ";
        cin >> locationIndex;

        // Validate input
        if (cin.fail() || locationIndex < 1 || locationIndex > size) {
            cout << "Invalid input. Please enter a valid number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
         else 
            break;  
     }
     
    string location = map.getLocationFromIndex(locationIndex);
    
    for(auto& entry : driversArray) {
    
    if(entry["Name"] == username){
      entry["Location"] = location;
      break;
    }
}

    saveToFile(DRIVERS_FILE, driversArray);
    cout << "Location updated successfully for " << username << endl;
}

// Driver login
bool driverLogin(const string& username, const string& password) {
    for (auto& driver : driversArray) {
        if (driver["Name"] == username && driver["Password"] == password) {
            cout << "\nDriver login successful." << endl;

            if(driver["Location"] == "")
              setDriverLocation(username);
            else
              cout << "\nYour current location is already set to: " << driver["Location"] << endl;
            
            if (!rideRequestQueue.empty())
                assignRideRequestToDriver(username);
            else
                availableRideRequests(username);
            
         return true;
        }
    }
    
    return false;
}

# endif
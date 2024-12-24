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
    json user = {
        {"UserName", name},
        {"Password", password},
        {"PhoneNo", phoneNo},
        {"Email", email}
    };

    usersMap.put(user);
    saveToFile(USERS_FILE, usersMap);
    cout << "User registered successfully with UserName: " << name << endl;
}

// Register a new driver
void registerDriver(const string& name, const string& password, const string& phoneNo, const string& vehicleName, const string& vehicleRegNumber, float fuelAvg) {
      
        json driver = {
        {"DriverName", name},
        {"Password", password},
        {"PhoneNo", phoneNo},
        {"VehicleName", vehicleName},
        {"VehicleRegNumber", vehicleRegNumber},
        {"FuelAverage", fuelAvg},
        {"Location", ""}
    };

    driversMap.put(driver);
    saveToFile(DRIVERS_FILE, driversMap);
    cout << "Driver registered successfully with DriverID: " << name << endl;
}

// User login
bool userLogin(const string& username, const string& password) {
    for (const auto& user : usersMap) {
        if (user["UserName"] == username && user["Password"] == password) {
            system("cls");
            cout << "\n-----User login successful-----\n" << endl;
            cout << "\nDo you need a ride?\n\n1. Yes\n2. No\n";
            int choice; 
            cin >> choice;
            while(choice != 1 && choice != 2) {
                cout << "\nInvalid choice! Please try again: ";
                cin >> choice;
            }
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
    
    for(auto& entry : driversMap)
        if(entry["DriverName"] == username){
            entry["Location"] = location;
            break;
        }

    saveToFile(DRIVERS_FILE, driversMap);
    cout << "Location updated successfully for " << username << endl;
}

// Driver login
bool driverLogin(const string& username, const string& password) {
    for (auto& driver : driversMap) {
        if (driver["DriverName"] == username && driver["Password"] == password) {
            system("cls");
            cout << "\n-----Driver login successful-----" << endl;

            if(driver["Location"] == "")
              setDriverLocation(username);
            else
              cout << "\nYour current location is already set to: " << driver["Location"] << endl;
            
            if (rideRequestQueueMap.size() != 0)
                assignRideRequestToDriver(username);
            else
                availableRideRequests(username);
            
         return true;
        }
    }
    
    return false;
}

# endif

# ifndef MYJSON_H
# define MYJSON_H

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
    cout << "Invalid username or password." << endl;
    return false;
}

void acceptRideRequest(int size){

    cout << "\nEnter the number corresponding to the ride request you want to accept, or '0' to log out: ";

    int requestChoice;
    cin >> requestChoice;

    if (cin.fail() || requestChoice < 0 || requestChoice > size) {
        cout << "Invalid input. Please enter a valid number." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    } 
    else if (requestChoice > 0) {
        // Find and remove the accepted ride request
        json updatedRequests, acceptedRide;
        int currentIndex = 0;

        for (const auto& request : rideRequestArray) {
            currentIndex++;
            if (currentIndex == requestChoice) {
                cout << "\nYou have accepted the ride of "<< request["User"] <<" from " 
                        << request["Source"] << " to " << request["Destination"] 
                        << " with a fare of " << request["Fare"] << " PKR." << endl;
                
                auto [currentDay, currentDate] = getCurrentDayAndDate();
                string driverVehicle = getDriverVehicle(request["Driver Name"]);
               
                acceptedRide = {
                {"User", request["User"]},
                {"Source", request["Source"] },
                {"Destination", request["Destination"] },
                {"Driver Name", request["Driver Name"]},
                {"Driver Vehicle", driverVehicle},
                {"Fare", request["Fare"]},
                {"Day", currentDay},
                {"Date", currentDate},
                {"Ride Status", ""}
            };        
              
               rideHistory.push_back(acceptedRide);
               saveToFile(RIDE_HISTORY, rideHistory);
               continue;
            }

            updatedRequests.push_back(request);
        }      
        // Save the updated rideRequests.json
        ofstream outFile("rideRequests.json");
        outFile << setw(4) << updatedRequests;
        outFile.close();        
    }
}

void availableRideRequests(const string& name){

        cout << "\nAvailable Ride Requests for You:" << endl;

        bool hasRequests = false;
        int count=1 , size = 0;
        for (const auto& request : rideRequestArray) {
            if (request["Driver Name"] == name) {
                hasRequests = true;
                cout <<count<<"- " << "User: " << request["User"]
                        << ", Source: " << request["Source"] 
                        << ", Destination: " << request["Destination"]
                        << ", Fare: " << request["Fare"] << " PKR" << endl;
                        count++;
            }
            ++size;
        }

        if (!hasRequests) {
            cout << "\nNo ride requests available for you at the moment." << endl;
        }

        else if (hasRequests)
          acceptRideRequest(size);
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
            
            availableRideRequests(username); 
         
         return true;
        }
    }
    
    cout << "Invalid username or password." << endl;
    return false;
}

# endif
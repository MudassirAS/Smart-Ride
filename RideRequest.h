#ifndef RIDEREQUEST_H
#define RIDEREQUEST_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "json.hpp"
#include "Graph.h"
#include <tuple>
#include "Json_Manager.h"
#include "getterFunctions.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef max

using namespace std;
using json = nlohmann::json;

// Function that tracks the route and prints live location of driver at any point. Also updates driver's position when they reach a new location
string trackRoute(vector<string> path, int totalDistance, Graph map, string driverName) {
    int distanceCovered = 0;
    int i;

    for(i = 0; i < path.size(); i++) {
        cout << path[i];
        int tempDistance = map.getEdgeWeight(path[i], path[i+1]);     // Distance from current location to the next location 

        for(int d = 0; d < tempDistance; d += 30) {     // Prints a new dot for every 30 metres covered (showing time spent from reaching one location to another)
            Sleep(800);
            cout << " .";
        }
        distanceCovered += tempDistance;
    }

    for(auto& driver : driversArray)
        if(driver["Name"] == driverName){
            driver["Location"] = path[i-1];
            saveToFile(DRIVERS_FILE, driversArray);    // Save driver's new location

            return driver["Location"];
        }
}


// Function to add a ride request when no drivers are available
void addRideRequest(const string& name, const string& source, const string& destination) {
    
    // Create a new ride request as a JSON object
    json rideRequest = {
        {"User", name},
        {"Source", source},
        {"Destination", destination},
        {"fare", 0.0}
    };

    // Add the new ride request to the queue (array)
    rideRequestQueue.push_back(rideRequest);

    // Write the updated queue back to the file
    saveToFile(RIDE_REQUEST_QUEUE,rideRequestQueue);
    cout << "\nRide request added to the queue successfully!" << endl;
}

void assignRideRequestToDriver(const string& driverName) {
    Graph map;
    map.initializeGraph(map);

    // Check if the queue is empty
    if (rideRequestQueue.empty()) {
        cout << "No ride requests available in the queue." << endl;
        return;
    }
    
    // Display all ride requests in the queue with an index (1, 2, 3, ...)
    cout << "\nAvailable Ride Requests for Driver: " << driverName << endl;
    for (size_t i = 0; i < rideRequestQueue.size(); ++i) {
        cout << "\nRide " << i + 1 << ": ";
        cout << "User: " << rideRequestQueue[i]["User"] << endl;
        cout << "Source: " << rideRequestQueue[i]["Source"] << endl;
        cout << "Destination: " << rideRequestQueue[i]["Destination"] << endl;
        // Calculate the fare
        string source = rideRequestQueue[i]["Source"];
        string destination = rideRequestQueue[i]["Destination"];
        double fare = getFare(source, destination, driverName);
        cout << "Fare: " << fare << " PKR" << endl;

        cout << "-----------------------------" << endl;
    }

    // Ask the driver to select which ride they want to accept
    int selectedRide = -1;
    while (true) {
        cout << "\nSelect a ride (1-" << rideRequestQueue.size() << "): ";
        cin >> selectedRide;

        if (cin.fail() || selectedRide < 1 || selectedRide > rideRequestQueue.size()) {
            cout << "Invalid choice. Please try again." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    }

    // Get the selected ride request from the queue
    json acceptedRide, selectedRideRequest = rideRequestQueue[selectedRide - 1];

    // Display the selected ride request to the driver
    cout << "\nRide has been selected. Details are given below:" << endl;
    cout << "\nDriver: " << driverName << endl;
    cout << "User: " << selectedRideRequest["User"] << endl;
    cout << "Source: " << selectedRideRequest["Source"] << endl;
    cout << "Destination: " << selectedRideRequest["Destination"] << endl;

    // Calculate the fare
    string source = selectedRideRequest["Source"];
    string destination = selectedRideRequest["Destination"];
    string driverVehicle = getDriverVehicle(driverName);
    string userName = selectedRideRequest["User"];
    double fare = getFare(source, destination, driverName);
    auto [currentDay, currentDate] = getCurrentDayAndDate();
    cout << ", Fare: " << fare << " PKR" << endl;
    cout << "-----------------------------" << endl;
    // Add the fare to the selected ride request
    selectedRideRequest["Fare"] = fare;

    // Remove the selected ride request from the queue
    rideRequestQueue.erase(rideRequestQueue.begin() + selectedRide - 1);

    // Save the updated queue back to the file
    saveToFile(RIDE_REQUEST_QUEUE,rideRequestQueue);
    cout << "\nYour Ride is going to start." << endl;

    acceptedRide = {
                {"User", selectedRideRequest["User"]},
                {"Source", selectedRideRequest["Source"] },
                {"Destination", selectedRideRequest["Destination"] },
                {"Driver Name", driverName},
                {"Driver Vehicle", driverVehicle},
                {"Fare", fare},
                {"Day", currentDay},
                {"Date", currentDate},
                {"Ride Status", ""}
            };     

            rideHistory.push_back(acceptedRide);
            saveToFile(RIDE_HISTORY, rideHistory);
             
            vector<string> sourceToDestPath = map.findShortestPath(source, destination);
            int distance = map.findDistance(sourceToDestPath);

            map.printPath(sourceToDestPath, source, destination);

            cout << "\nPress any key to start ride: ";
            getch();
            system("cls");

            string location = trackRoute(sourceToDestPath, distance, map, driverName);
            cout << "\n\nYou have completed your ride with: '" << userName << "'";  
            cout << "\n\nYour location has been updated to: '" << location << "'";
            cout << "\n\n---Please collect " << fare << "/- from the customer!";  

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
        string src, dest, userName, driverName, driverVehicle;

        for (const auto& request : rideRequestArray) {
            currentIndex++;
            if (currentIndex == requestChoice) {
                cout << "\nYou have accepted the ride of "<< request["User"] <<" from " 
                        << request["Source"] << " to " << request["Destination"] 
                        << " with a fare of " << request["Fare"] << " PKR." << endl;
                     
                auto [currentDay, currentDate] = getCurrentDayAndDate();
                driverVehicle = getDriverVehicle(request["Driver Name"]);
                driverName = request["Driver Name"];
                src = request["Source"], dest = request["Destination"];
                userName = request["User"];
                
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
        saveToFile(RIDE_REQUESTS_FILE, updatedRequests);
        
        Graph map; map.initializeGraph(map); 
        vector<string> sourceToDestPath = map.findShortestPath(src, dest);
        int distance = map.findDistance(sourceToDestPath);
        
        map.printPath(sourceToDestPath, src, dest);
        
        string location = trackRoute(sourceToDestPath, distance, map, driverName);
        cout << "\n\nYou have completed your ride with: '" << userName << "'";  
        cout << "\n\nYour location has been updated to: '" << location << "'";      
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

// Request a ride and find the top 3 nearest drivers with fare calculation
void requestRide(const string& username) {
    Graph map;
    map.initializeGraph(map);

    system("cls");
    map.displayLocations();

    int sourceX, destinationX;  // Source and destination index
    cout << "\n\nEnter your starting (current) location number: ";
    cin >> sourceX;
    cout << "\nEnter your end (destination) location number: ";
    cin >> destinationX;

    system("cls");

    string source = map.getLocationFromIndex(sourceX);
    string destination = map.getLocationFromIndex(destinationX);

    vector<string> sourceToDestPath = map.findShortestPath(source, destination);

    map.printPath(sourceToDestPath, source, destination);  
    cout << endl;

    int distance = map.findDistance(sourceToDestPath);
    cout << "\nTotal distance ----- " << distance << " meters" << endl;

    vector<string> availableDrivers;
    for(auto& driver : driversArray) {
        if(driver["Location"] != "")
            availableDrivers.push_back(driver["Name"]);
    }

    if (availableDrivers.empty() || driversArray.empty()) {
        cout << "No drivers available at the moment." << endl;
        addRideRequest(username, source, destination);
        return;
    }

    vector<tuple<string, string, int, string, double>> driverDistancesAndFares;

    // Calculate distance and fare for each driver
    for (const auto& DriverName : availableDrivers) {
        string username = getDriverName(DriverName);
        string driverLocation = getDriverLocation(DriverName);
        string vehicleName = getVehicleName(DriverName);
        double fuelMileage = getFuelAverage(DriverName);

        // Find corresponding fuelAvg from the drivers file
        auto driverData = find_if(driversArray.begin(), driversArray.end(), [&](const json& d) {
            return d["Name"] == username;
        });

        if (driverData != driversArray.end()) {
            // Calculate the distance and fare
            vector<string> driverToSourcePath = map.findShortestPath(driverLocation, source);
            int driverDistance = map.findDistance(driverToSourcePath);
            
            double fare = getFare(source, destination, username);

            // Store the data in the tuple
            driverDistancesAndFares.push_back({username, vehicleName, driverDistance, driverLocation, fare});
        }
    }

    // Sort the drivers by distance
    sort(driverDistancesAndFares.begin(), driverDistancesAndFares.end(), 
        [](const tuple<string, string, int, string, double>& a, const tuple<string, string, int, string, double>& b) {
            return get<2>(a) < get<2>(b); // Sort by the third element (distance)
        }

    );

    // Display the nearest drivers and their fares
    cout << "\nAvailable Drivers Nearest to Your Location:" << endl << endl;
    int maxDriversToShow = min(3, (int)driverDistancesAndFares.size());

    for (int i = 0; i < maxDriversToShow; ++i) {
        cout << "Driver name: " << get<0>(driverDistancesAndFares[i]) 
                << ", Vehicle: " << get<1>(driverDistancesAndFares[i]) 
                << ", Distance: " << get<2>(driverDistancesAndFares[i]) << " meters" 
                << ", Location: " << get<3>(driverDistancesAndFares[i]) 
                << ", Fare: " << get<4>(driverDistancesAndFares[i]) << " PKR" << endl;
    }
    
    // User selects a driver
        int selectedDriverIndex = -1;
        while (true) {
            cout << "\nSelect a driver (1-" << maxDriversToShow << "): ";
            cin >> selectedDriverIndex;
            if (cin.fail() || selectedDriverIndex < 1 || selectedDriverIndex > maxDriversToShow) {
                cout << "Invalid choice. Please try again." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else 
                break;
            
        }

        // Confirm selected driver
        string selectedDriverName = get<0>(driverDistancesAndFares[selectedDriverIndex - 1]);
        string selectedDriverVehicle = get<1>(driverDistancesAndFares[selectedDriverIndex - 1]);
        cout << "\nYou have selected Driver: " << selectedDriverName
                << " with a fare of " << fixed << setprecision(2) << get<4>(driverDistancesAndFares[selectedDriverIndex - 1]) << " PKR." << endl;
        
        json rideRequest = {
            {"User", username},
            {"Source", source},
            {"Destination", destination},
            {"Driver Name", selectedDriverName},
            {"Driver Vehicle", selectedDriverVehicle},
            {"Fare", get<4>(driverDistancesAndFares[selectedDriverIndex - 1])}
        };

        rideRequestArray.push_back(rideRequest);
        saveToFile(RIDE_REQUESTS_FILE, rideRequestArray);
        cout << "\nYour ride request has been sent to Driver: " << selectedDriverName << endl;

        // Tracking driver to source path
        string selectedDriverLocation = get<3>(driverDistancesAndFares[selectedDriverIndex - 1]);
        vector<string> driverToSourcePath = map.findShortestPath(selectedDriverLocation, source);
        int driverDistance = map.findDistance(driverToSourcePath);

        cout << "\nDriver " << selectedDriverName << " is on their way to your location. Please br ready!\n\nDriver is currently at: ";
        trackRoute(driverToSourcePath, driverDistance, map, selectedDriverName);
        cout << "\nThe driver has reached! Kindly locate the driver!";




        // Loop (wait) till driver starts the ride.



        // After driver has started the ride
        cout << "\nThe ride has started. Buckle up!\n";
        trackRoute(sourceToDestPath, distance, map, selectedDriverName);
    }



#endif

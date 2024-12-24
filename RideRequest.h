#ifndef RIDEREQUEST_H
#define RIDEREQUEST_H

#include <iostream>
#include <iomanip>
#include <conio.h>
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

    for(auto& driver : driversMap.entries())
        if(driver["DriverName"] == driverName){
            driver["Location"] = path[i-1];
            saveToFile(DRIVERS_FILE, driversMap);    // Save driver's new location

            return driver["Location"];
        }
    return "";
}

// Function to add a ride request when no drivers are available
void addRideRequest(const string& name, const string& source, const string& destination) {
    
    // Create a new ride request as a JSON object
    json rideRequest = {
        {"UserName", name},
        {"Source", source},
        {"Destination", destination},
        {"Fare", 0.0}
    };

    // Add the new ride request to the queue (array)
    rideRequestQueueMap.put(rideRequest);

    // Write the updated queue back to the file
    saveToFile(RIDE_REQUEST_QUEUE,rideRequestQueueMap);
    cout << "\nRide request added to the queue successfully!" << endl;
}

void assignRideRequestToDriver(const string& driverName) {
    Graph map;
    map.initializeGraph(map);

    // Check if the queue is empty
    if (rideRequestQueueMap.size() == 0) {
        cout << "No ride requests available in the queue." << endl;
        return;
    }
    
    // Display all ride requests in the queue with an index (1, 2, 3, ...)
    cout << "\nAvailable Ride Requests for Driver: " << driverName << endl;
    int i = 1;
    for (auto& rideRequest : rideRequestQueueMap.entries()) {
        cout << "\nRide " << i++ << ": ";
        cout << "User: " << rideRequest["UserName"] << endl;
        cout << "Source: " << rideRequest["Source"] << endl;
        cout << "Destination: " << rideRequest["Destination"] << endl;
        // Calculate the fare
        string source = rideRequest["Source"];
        string destination = rideRequest["Destination"];
        double fare = getFare(source, destination, driverName);
        cout << "Fare: " << fare << " PKR" << endl;

        cout << "-----------------------------" << endl;
    }

    // Ask the driver to select which ride they want to accept
    int selectedRide = -1;
    while (true) {
        cout << "\nSelect a ride (1-" << rideRequestQueueMap.size() << "): ";
        cin >> selectedRide;

        if (cin.fail() || selectedRide < 1 || selectedRide > rideRequestQueueMap.size()) {
            cout << "Invalid choice. Please try again." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            break;
        }
    }

    // Get the selected ride request from the queue
    json acceptedRide, selectedRideRequest = rideRequestQueueMap.entries()[selectedRide - 1];

    // Calculate the fare
    string source = selectedRideRequest["Source"];
    string destination = selectedRideRequest["Destination"];
    string driverVehicle = getDriverVehicle(driverName);
    string userName = selectedRideRequest["UserName"];
    double fare = getFare(source, destination, driverName);
    auto [currentDay, currentDate] = getCurrentDayAndDate();

    // Display the selected ride request to the driver
    cout << "\nRide has been selected. Details are given below:" << endl;
    cout << "\nDriver: " << driverName << endl;
    cout << "User: " << userName << endl;
    cout << "Source: " << source << endl;
    cout << "Destination: " << destination << endl;
    cout << "Fare: " << fare << " PKR" << endl;
    cout << "-----------------------------" << endl;
    // Add the fare to the selected ride request
    selectedRideRequest["Fare"] = fare;
    rideRequestQueueMap.update(selectedRide - 1, selectedRideRequest);
    cout << "Test0";
    saveToFile(RIDE_REQUEST_QUEUE,rideRequestQueueMap);
    // Remove the selected ride request from the queue
    rideRequestQueueMap.remove(selectedRide - 1);
    // Save the updated queue back to the file
    saveToFile(RIDE_REQUEST_QUEUE,rideRequestQueueMap);
    cout << "\nYour Ride is going to start." << endl;
   
    acceptedRide = {
                {"UserName", selectedRideRequest["UserName"]},
                {"Source", selectedRideRequest["Source"] },
                {"Destination", selectedRideRequest["Destination"] },
                {"DriverName", driverName},
                {"Driver Vehicle", driverVehicle},
                {"Fare", fare},
                {"Day", currentDay},
                {"Date", currentDate},
                {"Ride Status", ""}
            };     

            rideHistoryMap.put(acceptedRide);
            saveToFile(RIDE_HISTORY, rideHistoryMap);
             
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

       for (const auto& request : rideRequestMap.entries()) {
            if (currentIndex == requestChoice - 1) {
             cout << "\nYou have accepted the ride of " << request["UserName"] << " from " 
             << request["Source"] << " to " << request["Destination"] 
             << " with a fare of " << request["Fare"] << " PKR." << endl;

            auto [currentDay, currentDate] = getCurrentDayAndDate();
            driverVehicle = getDriverVehicle(request["DriverName"]);
            driverName = request["DriverName"];
            src = request["Source"];
            dest = request["Destination"];
            userName = request["UserName"];
                    
            acceptedRide = {
            {"UserName", request["UserName"]},
            {"Source", request["Source"]},
            {"Destination", request["Destination"]},
            {"DriverName", request["DriverName"]},
            {"Driver Vehicle", driverVehicle},
            {"Fare", request["Fare"]},
            {"Day", currentDay},
            {"Date", currentDate},
            {"Ride Status", "Accepted"}
        };

        // Save the accepted ride to ride history
        rideHistoryMap.put(acceptedRide);
        saveToFile(RIDE_HISTORY, rideHistoryMap);

        // Skip adding this request to updatedRequests
    } else {
        // Add unaccepted rides to the updated list
        updatedRequests.push_back(request);
         }
         ++currentIndex; // Increment index correctly
     }

        // Update the rideRequestMap after processing
        rideRequestMap.replaceAll(updatedRequests); 
        // Save the updated rideRequests.json
        saveToFile(RIDE_REQUESTS_FILE, rideRequestMap);
           
    }
}

void availableRideRequests(const string& name){

        cout << "\nAvailable Ride Requests for You:" << endl;

        bool hasRequests = false;
        int count=1 , size = rideRequestMap.size();
        for (const auto& request : rideRequestMap.entries()) {
            if (request["DriverName"] == name) {
                hasRequests = true;
                cout <<count<<"- " << "User: " << request["UserName"]
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
    for(auto& driver : driversMap.entries()) {
        if(driver["Location"] != "")
            availableDrivers.push_back(driver["DriverName"]);
    }

    if (availableDrivers.empty() || driversMap.size() == 0) {
        cout << "No drivers available at the moment." << endl;
        addRideRequest(username, source, destination);
        return;
    }

    vector<tuple<string, string, int, string, double>> driverDistancesAndFares;

    // Calculate distance and fare for each driver
    for (const auto& DriverName : availableDrivers) {
        string username = getDriverName(DriverName);
        string driverLocation = getDriverLocation(DriverName);
        string vehicleName = getDriverVehicle(DriverName);
        double fuelMileage = getFuelAverage(DriverName);

        // Find corresponding fuelAvg from the drivers file
        auto driverData = find_if(driversMap.begin(), driversMap.end(), [&](const json& d) {
            return d["DriverName"] == username;
        });

        if (driverData != driversMap.end()) {
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
        double fare =  get<4>(driverDistancesAndFares[selectedDriverIndex - 1]);
        cout << "\nYou have selected Driver: " << selectedDriverName
                << " with a fare of " << fixed << setprecision(2) << get<4>(driverDistancesAndFares[selectedDriverIndex - 1]) << " PKR." << endl;
        
        json rideRequest = {
            {"UserName", username},
            {"Source", source},
            {"Destination", destination},
            {"DriverName", selectedDriverName},
            {"Driver Vehicle", selectedDriverVehicle},
            {"Fare", get<4>(driverDistancesAndFares[selectedDriverIndex - 1])},
            {"Ride Status", "Pending"}
        };

        rideRequestMap.put(rideRequest);
        saveToFile(RIDE_REQUESTS_FILE, rideRequestMap);
        cout << "\nYour ride request has been sent to Driver: " << selectedDriverName << endl;

        // Tracking driver to source path
        string selectedDriverLocation = get<3>(driverDistancesAndFares[selectedDriverIndex - 1]);
        vector<string> driverToSourcePath = map.findShortestPath(selectedDriverLocation, source);
        int driverDistance = map.findDistance(driverToSourcePath);
        
        cout << "\nWaiting for the driver to accept the request!\n";

        bool isAccepted = false;
        while(!isAccepted) {
            Sleep(4000);

            json array = loadFromFile(RIDE_HISTORY);
                for (size_t i = 0; i < array.size(); ++i) {
                 if (array[i]["DriverName"] == selectedDriverName && 
                     array[i]["Fare"] == fare &&
                     array[i]["Ride Status"] == "Accepted") {
                     rideHistoryMap.update(i, array[i]);
                     saveToFile(RIDE_HISTORY, rideHistoryMap);
                     isAccepted = true;
                     break;
                 }
             }
        }

        cout << "\nDriver " << selectedDriverName << " is on their way to your location. Please br ready!\n\nDriver is currently at: ";
        trackRoute(driverToSourcePath, driverDistance, map, selectedDriverName);
        cout << "\nThe driver has reached! Kindly locate the driver!";
        
       

        // Loop (wait) till driver starts the ride.



        // After driver has started the ride
        cout << "\nThe ride has started. Buckle up!\n";
        trackRoute(sourceToDestPath, distance, map, selectedDriverName);
    }



#endif

#ifndef RIDEREQUEST_H
#define RIDEREQUEST_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "json.hpp"
#include "Graph.h"
#include <tuple>
#include "Json_Manager.h"
#include "getterFunctions.h"

using namespace std;
using json = nlohmann::json;

const int FUEL_PRICE_PER_LITRE = 260; // PKR

#undef max

const int FUEL_PRICE_PER_LITRE = 260; // PKR



// Function that tracks the route and prints live location of driver at any point. Also updates driver's position when they reach a new location
void trackRoute(vector<string> path, int totalDistance, Graph map, string driverName) {
    int distanceCovered = 0;
    json driver;

    for(auto& entry : driversArray)     // Searching for the driver's information just once
        if(entry["Name"] == driverName)
            driver = entry;

    for(int i = 0; i < path.size(); i++) {
        cout << path[i];
        driver["Location"] = path[i];
        int tempDistance = map.getEdgeWeight(path[i], path[i+1]);     // Distance from current location to the next location 

        for(int d = 0; d < tempDistance; d += 30) {     // Prints a new dot for every 30 metres covered (showing time spent from reaching one location to another)
            Sleep(500);
            cout << " .";
        }

        distanceCovered += tempDistance;
    }
}

// Request a ride and find the top 3 nearest drivers with fare calculation
void requestRide(const string& username) {
    Graph map;
    map.initializeGraph(map);
    map.displayLocations();

    int sourceX, destinationX;  // Source and destination index
    cout << "\nEnter your starting (current) location number: ";
    cin >> sourceX;
    cout << "\nEnter your end (destination) location number: ";
    cin >> destinationX;

    string source = map.getLocationFromIndex(sourceX);
    string destination = map.getLocationFromIndex(destinationX);


    vector<string> shortestPath = map.findShortestPath(source, destination);
    int size = shortestPath.size(), count = 0;

    if (!shortestPath.empty()) {
        cout << "\nShortest path from " << source << " to " << destination << ": ";
        for (const auto& node : shortestPath)
            if(count++ + 1 == size)
                cout << node << " ";
            else
                cout << node << " -> ";
        
        cout << endl;
  
        int distance = map.findDistance(shortestPath);
        cout << "\nTotal distance: " << distance << " meters" << endl;

        vector<string> availableDrivers;
        for(auto& driver : driversArray) {
            if(driver["Location"] != "")
                availableDrivers.push_back(driver["DriverID"]);
        }

        if (availableDrivers.empty() || driversArray.empty()) {
            cout << "No drivers available at the moment." << endl;
            return;
        }

        vector<tuple<string, string, int, string, double>> driverDistancesAndFares;

        // Calculate distance and fare for each driver
        for (const auto& DriverID : availableDrivers) {
            string username = getDriverName(DriverID);
            string driverLocation = getDriverLocation(DriverID);
            string vehicleName = getVehicleName(DriverID);
            float fuelMileage = getFuelAverage(DriverID);

            // Find corresponding fuelAvg from the drivers file
            auto driverData = find_if(driversArray.begin(), driversArray.end(), [&](const json& d) {
                return d["Name"] == username;
            });

            if (driverData != driversArray.end()) {
                // Calculate the distance and fare
                vector<string> driverToSourcePath = map.findShortestPath(driverLocation, source);
                int driverDistance = map.findDistance(driverToSourcePath);

                vector<string> sourceToDestPath = map.findShortestPath(source, destination);
            
                double sourceToDestDistance = map.findDistance(sourceToDestPath) / 1000.0;

                // Calculate fare using the given formula
                double fare = ((sourceToDestDistance / fuelMileage) * FUEL_PRICE_PER_LITRE) * 10;
                fare = roundToTwoDecimals(fare);

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
    }
}


#endif

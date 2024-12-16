#ifndef RIDEREQUEST_H
#define RIDEREQUEST_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "json.hpp"
#include "Graph.h" // Assuming the Graph class is defined in a separate header file
#include <tuple>

using namespace std;
using json = nlohmann::json;

const int FUEL_PRICE_PER_LITRE = 260; // PKR

const string DRIVER_LOCATIONS_FILE = "driverLocations.json"; 
const string DRIVERS_FILE = "drivers.json";  // File containing driver fuelAvg data

// Load driver locations from file
static json loadDriverLocations() {
    ifstream file(DRIVER_LOCATIONS_FILE);
    if (!file.is_open()) {
        return json::array(); // Return an empty array if file does not exist
    }

    json data;
    file >> data;
    return data;
}

// Load driver data (fuelAvg) from drivers.json
static json loadDriversData() {
    ifstream file(DRIVERS_FILE);
    if (!file.is_open()) {
        cerr << "Error loading drivers' data." << endl;
        return json::array(); // Return empty array if file does not exist
    }

    json data;
    file >> data;
    return data;
}

class RideRequest {
private:
    static json driverLocationsArray;
    static json driversArray;

public:

    // Request a ride and find the top 3 nearest drivers with fare calculation
    static void requestRide(const string& source, const string& destination) {
        Graph map;
        map.initializeGraph(map);

        vector<string> shortestPath = map.findShortestPath(source, destination);
        int size = shortestPath.size(), count = 0;

        if (!shortestPath.empty()) {
            cout << "\nShortest path from " << source << " to " << destination << ": ";
            for (const auto& node : shortestPath) {
                if(count++ + 1 == size){
                    cout << node << " ";
                } else {
                    cout << node << " -> ";
                }
            }
            cout << endl;

            int distance = map.findDistance(shortestPath);
            cout << "\nTotal distance: " << distance << " meters" << endl;

            // Load driver locations and fuel average data
            driverLocationsArray = loadDriverLocations();
            driversArray = loadDriversData();

            if (driverLocationsArray.empty() || driversArray.empty()) {
                cout << "No drivers available at the moment." << endl;
                return;
            }

            vector<tuple<string, int, string, double>> driverDistancesAndFares;

            // Calculate distance and fare for each driver
            for (const auto& entry : driverLocationsArray) {
                string driverID = entry["DriverID"];
                string driverLocation = entry["Location"];

                // Find corresponding fuelAvg from the drivers file
                auto driverData = find_if(driversArray.begin(), driversArray.end(), [&](const json& d) {
                    return d["DriverID"] == driverID;
                });

                if (driverData != driversArray.end()) {
                    // Assuming the fuelAvg is a float
                    float fuelMileage = (*driverData)["fuelAverage"];

                    // Calculate the distance and fare
                    vector<string> driverToSourcePath = map.findShortestPath(driverLocation, source);
                    int driverDistance = map.findDistance(driverToSourcePath);

                    vector<string> sourceToDestPath = map.findShortestPath(source, destination);
                    int sourceToDestDistance = map.findDistance(sourceToDestPath)/1000;

                    // Calculate fare using the given formula
                    double fare = ((double) (sourceToDestDistance / fuelMileage) * FUEL_PRICE_PER_LITRE) * 10;

                    // Store the data in the tuple
                    driverDistancesAndFares.push_back({driverID, driverDistance, driverLocation, fare});
                } else {
                    cout << "Driver ID " << driverID << " not found in drivers data." << endl;
                }
            }

            // Sort the drivers by distance
            sort(driverDistancesAndFares.begin(), driverDistancesAndFares.end(), 
                [](const tuple<string, int, string, double>& a, const tuple<string, int, string, double>& b) {
                    return get<1>(a) < get<1>(b); // Sort by distance (2nd element)
                }
            );

            // Display the nearest drivers and their fares
            cout << "\nAvailable Drivers Nearest to Your Location:" << endl << endl;
            int maxDriversToShow = min(3, (int)driverDistancesAndFares.size());

            for (int i = 0; i < maxDriversToShow; ++i) {
                cout << "DriverID: " << get<0>(driverDistancesAndFares[i]) 
                     << ", Distance: " << get<1>(driverDistancesAndFares[i]) << " meters" 
                     << ", Location: " << get<2>(driverDistancesAndFares[i]) 
                     << ", Fare: " << get<3>(driverDistancesAndFares[i]) << " PKR" << endl;
            }
        }
    }
};

// Static member variables initialization
json RideRequest::driverLocationsArray;
json RideRequest::driversArray;

#endif

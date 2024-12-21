#ifndef getterFunctions
#define getterFunctions

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "json.hpp"
#include "ctime"
#include "Json_Manager.h"

using namespace std;
using json = nlohmann::json;
const int FUEL_PRICE_PER_LITRE = 260; // PKR

string getDriverName(string name) {
    for (auto& entry : driversArray)
        if(entry["Name"] == name)
            return entry["Name"];
    
    return "";
}

string getDriverVehicle(string name) {
    for (auto& entry : driversArray)
        if(entry["Name"] == name)
            return entry["VehicleName"];
    
    return "";
}

string getDriverLocation(string name) {
    for (auto& entry : driversArray)
        if(entry["Name"] == name)
            return entry["Location"];
    
    return "";
}

string getVehicleName(string name) {
    for (auto& entry : driversArray)
        if(entry["Name"] == name)
            return entry["VehicleName"];
    
    return "";
}

double getFuelAverage(string name) {
    for (auto& entry : driversArray)
        if (entry["Name"] == name) 
            if (entry.contains("FuelAverage") && entry["FuelAverage"].is_number())
                return entry["FuelAverage"];
            else
                return 0.0;             

    return 0.0;
}

double roundToTwoDecimals(double value) {
    return round(value * 100.0) / 100.0;  
}

double getFare(string source , string destination, string name){
           Graph map;
           map.initializeGraph(map);

                vector<string> sourceToDestPath = map.findShortestPath(source, destination);
            
                double sourceToDestDistance = map.findDistance(sourceToDestPath) / 1000.0;

                // Calculate fare using the given formula
                double fuelMileage = getFuelAverage(name);
                double fare = ((sourceToDestDistance / fuelMileage) * FUEL_PRICE_PER_LITRE) * 10;
                fare = roundToTwoDecimals(fare);
     return fare;           
}

pair<string, string> getCurrentDayAndDate() {
    
    time_t now = time(0);
    tm* localTime = localtime(&now);

    // Extract the day and date
    char dayBuffer[20], dateBuffer[20];
    strftime(dayBuffer, sizeof(dayBuffer), "%A", localTime);     
    strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", localTime);

    return {string(dayBuffer), string(dateBuffer)};
}


#endif
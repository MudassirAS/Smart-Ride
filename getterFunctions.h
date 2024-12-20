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


string getDriverName(string id) {
    for (auto& entry : driversArray)
        if(entry["DriverID"] == id)
            return entry["Name"];
    
    return "";
}

string getDriverVehicle(string name) {
    for (auto& entry : driversArray)
        if(entry["Name"] == name)
            return entry["VehicleName"];
    
    return "";
}

string getDriverLocation(string id) {
    for (auto& entry : driversArray)
        if(entry["DriverID"] == id)
            return entry["Location"];
    
    return "";
}

string getVehicleName(string id) {
    for (auto& entry : driversArray)
        if(entry["DriverID"] == id)
            return entry["VehicleName"];
    
    return "";
}

float getFuelAverage(string id) {
    for (auto& entry : driversArray)
        if (entry["DriverID"] == id) 
            if (entry.contains("fuelAverage") && entry["fuelAverage"].is_number())
                return entry["fuelAverage"];
            else
                return 0.0;              

    return 0.0;
}

double roundToTwoDecimals(double value) {
    return round(value * 100.0) / 100.0;  
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
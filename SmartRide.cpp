#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <limits>
#include "Graph.h" // Include the Graph class header
#include "User_N_Driver_Details.h"
#include "RideRequest.h"

using namespace std;

int main() {
    MainMenu::Begin();
    bool running = true;
    while (running) {
        system("cls");
        cout << "\n\n---------------Main Menu:--------------\n\n";
        cout << "\n1. Register User";
        cout << "\n2. Register Driver";
        cout << "\n3. User Login";
        cout << "\n4. Driver Login";
        cout << "\n5. Exit";
        cout << "\nEnter your choice: ";

        int choice;
        cin >> choice;
        cin.ignore(); // Ignore newline character left in input buffer

        switch (choice) {
            case 1 : { 
                system("cls");
                cout << "\n\n---------------User Registration Page:---------------\n\n";
                string name, password, phoneNo, email;
                cout << "Enter Name: ";
                getline(cin, name);
                cout << "Enter Password: ";
                getline(cin, password);
                cout << "Enter Phone Number: ";
                getline(cin, phoneNo);
                cout << "Enter Email: ";
                getline(cin, email);
                MainMenu::registerUser(name, password, phoneNo, email);
                cout << "\nPress any key to return to main menu: ";
                getch();
                break;
            }
            

            case 2 : {
                system("cls");
                cout << "\n\n---------------Driver Registration Page:---------------\n\n";
                string name, password, phoneNo, vehicleName, vehicleReg, mileageStr;
                cout << "Enter Name: ";
                getline(cin, name);
                cout << "Enter Password: ";
                getline(cin, password);
                cout << "Enter Phone Number: ";
                getline(cin, phoneNo);
                cout << "Enter Vehicle Name: ";
                getline(cin, vehicleName);
                cout << "Enter Vehicle Registration Number: ";
                getline(cin, vehicleReg);
                cout << "Enter Vehicle Milleage: ";
                getline(cin, mileageStr);
                float mileage = stof(mileageStr);
                MainMenu::registerDriver(name, password, phoneNo, vehicleName, vehicleReg, mileage);
                cout << "\nPress any key to return to main menu: ";
                getch();
                break;
            }

            case 3 : {
                system("cls");
                cout << "\n\n---------------User Login Page:--------------\n\n";
                string userID, password;
                cout << "Enter UserID: ";
                getline(cin, userID);
                cout << "\nEnter Password: ";
                getline(cin, password);
                while(!MainMenu::userLogin(userID, password)) {
                    cout << "Wrong password! Please try again: ";
                    getline(cin, password);
                }

                cout << "\nPress any key to return to main menu: ";
                getch();
                break;
            }

            case 4 : {
                system("cls");
                cout << "\n\n---------------Driver Login Page:--------------\n\n";
                string driverID, password;
                cout << "Enter DriverID: ";
                getline(cin, driverID);
                cout << "\nEnter Password: ";
                getline(cin, password);
                while(!MainMenu::driverLogin(driverID, password)) {
                    cout << "Wrong password! Please try again: ";
                    getline(cin, password);
                }

                cout << "\nPress any key to return to main menu: ";
                getch();
                break;
            }

            case 5 : {
                running = false; 
                break;
            }

            default : cout << "Invalid choice. Please try again.";
        }
    }
    return 0;             
}
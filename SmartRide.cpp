#include <iostream>
#include <conio.h>
#include <vector>
#include <string>
#include <limits>
#include "Graph.h"
#include "Json_Manager.h"
#include "User_N_Driver_Details.h"

using namespace std;

int main() {
   
    initialization();
    bool running = true;
    while (running) {
        system("cls");
        cout << "\n\n---------------Main Menu:--------------\n\n";
        cout << "\n1. Register User";
        cout << "\n2. Register Driver";
        cout << "\n3. User Login";
        cout << "\n4. Driver Login";
        cout << "\n5. Exit";
        cout << "\n\nEnter your choice: ";

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
                registerUser(name, password, phoneNo, email);
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
                registerDriver(name, password, phoneNo, vehicleName, vehicleReg, mileage);
                cout << "\nPress any key to return to main menu: ";
                getch();
                break;
            }

            case 3 : {
                system("cls");
                cout << "\n\n---------------User Login Page:--------------\n\n";
                string username, password;
                cout << "Enter username: ";
                getline(cin, username);
                cout << "\nEnter Password: ";
                getline(cin, password);
                while(!userLogin(username, password)) {
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
                string username, password;
                cout << "Enter username: ";
                getline(cin, username);
                cout << "\nEnter Password: ";
                getline(cin, password);
                while(!driverLogin(username, password)) {
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

            default : {
                cout << "Invalid choice. Please try again.";
                cout << "\nPress any key to return to main menu: ";
                getch();
            }
        }
    }
    return 0;             
}
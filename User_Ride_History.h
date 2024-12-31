#ifndef RIDE_HISTORY_NAVIGATOR_H
#define RIDE_HISTORY_NAVIGATOR_H

#include <iostream>
#include <string>
#include "json.hpp"
#include "HashMap.h"

using namespace std;
using json = nlohmann::json;

// Node class to represent each ride
class Node {
public:
    json rideDetails; // Single ride's details
    Node* next;
    Node* prev;

    // Constructor
    Node(const json& ride) : rideDetails(ride), next(nullptr), prev(nullptr) {}
};

// Doubly Linked List class to manage ride history
class DoublyLinkedList {
private:
    Node* head;
    Node* tail;

public:
    // Constructor
    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    // Destructor to free memory
    ~DoublyLinkedList() {
        Node* current = head;
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // Add a ride to the end of the list
    void addRide(const json& ride) {
        Node* newNode = new Node(ride);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }

    // Check if the list is empty
    bool isEmpty() const {
        return head == nullptr;
    }

    // Display rides (forward navigation)
    void displayForward() const {
        Node* current = head;
        while (current) {
            cout << current->rideDetails.dump(4) << endl; // Pretty print JSON
            current = current->next;
        }
    }

    // Display rides (backward navigation)
    void displayBackward() const {
        Node* current = tail;
        while (current) {
            cout << current->rideDetails.dump(4) << endl; // Pretty print JSON
            current = current->prev;
        }
    }

    // Navigate rides interactively
   void navigate() const {
    Node* current = tail; // Start from the most recent ride (tail)

    if (!current) {
        cout << "No ride history available.\n";
        return;
    }

    bool isFirstRide = true;

    while (current) {

        if (isFirstRide) {
            cout << "\n----- Most Recent Ride Details -----\n";
             cout << current->rideDetails.dump(4) << endl;
            isFirstRide = false;
        }

        cout << "\nOptions: (1) Next (2) Previous (0) Exit\n\nEnter choice: ";
        int choice;
        cin >> choice;

        if (choice == 1) {
            if (current->next) {
                current = current->next;
                 cout << current->rideDetails.dump(4) << endl;
            } 
            else {
                cout << "\nNo any further next ride exist to the previous ones.\n";
            }
        } else if (choice == 2) {
            if (current->prev) {
                current = current->prev;
                 cout << current->rideDetails.dump(4) << endl;
            } 
            else {
                cout << "\nNo any further previous ride exist to the next ones.\n";
            }
        } 
        else if (choice == 0) {
            cout << "\nReturning to the main menu...\n";
            return;
        } 
        else {
            cout << "\nInvalid choice. Please enter 1, 2, or 0.\n";
        }
    }
}

};

// Function to create a user's ride history list and navigate through it
void createUserRideHistory(const string& userName, const HashMap<json>& rideHistory) {
    DoublyLinkedList userRides;

    for (const auto& ride : rideHistory.entries()) {
        if (ride["UserName"] == userName) {
            userRides.addRide(ride); // Add matching rides to the linked list
        }
    }

    if (!userRides.isEmpty()) {
        cout << "\n--- Ride History for " << userName << " ---\n";
        userRides.navigate(); // Allow navigation
    }
    else 
        cout << "\nNo rides found for " << userName << ".\n";

}

#endif // RIDE_HISTORY_NAVIGATOR_H

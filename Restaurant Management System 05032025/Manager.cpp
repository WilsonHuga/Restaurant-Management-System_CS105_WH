#include "MenuFunctions.h"
#include <iostream>
using namespace std;

void managerMenu(Restaurant& restaurant) {
    int choice;
    while (true) {
        cout << "\n==== Manager Menu ====\n";
        cout << "1. Add Menu Item\n";
        cout << "2. Remove Menu Item\n";
        cout << "3. Add Special Menu\n";
        cout << "4. View Customer Tables and Bills\n";
        cout << "5. Return to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            restaurant.addMenuItem();
            break;
        case 2:
            restaurant.removeMenuItem();
            break;
        case 3:
            restaurant.addSpecialMenu();
            break;
        case 4:
            restaurant.viewCustomerTablesAndBills();
            break;
        case 5:
            cout << "Returning to main menu.\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}
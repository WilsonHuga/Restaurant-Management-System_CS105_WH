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

void kitchenAttendantMenu(Restaurant& restaurant) {
    int choice;
    int tableNum;

    while (true) {
        cout << "\n==== Kitchen Attendant Menu ====\n";
        cout << "1. View All Orders\n";
        cout << "2. Mark Order as In Preparation\n";
        cout << "3. Mark Order as Completed\n";
        cout << "4. Return to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            restaurant.displayOrders();
            break;
        case 2:
            cout << "Enter table number: ";
            cin >> tableNum;
            restaurant.markInPreparation(tableNum);
            break;
        case 3:
            cout << "Enter table number to mark as completed: ";
            cin >> tableNum;
            for (auto& order : restaurant.orders) {
                if (order.tableNumber == tableNum) {
                    order.markAsCompleted();
                    break;
                }
            }
            break;
        case 4:
            cout << "Returning to main menu.\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

void customerMenu(Restaurant& restaurant) {
    int choice;
    int tableNum;

    // Show available tables
    restaurant.showAvailableTables();

    cout << "Enter your table number: ";
    cin >> tableNum;

    // Validate table number
    if (tableNum < 1 || tableNum > restaurant.tableCount || !restaurant.availableTables[tableNum - 1]) {
        cout << "Invalid or unavailable table number. Please try again.\n";
        return;
    }

    while (true) {
        cout << "\n==== Customer Menu (Table " << tableNum << ") ====\n";
        cout << "1. View Menu\n";
        cout << "2. Place Order\n";
        cout << "3. View Current Order\n";
        cout << "4. Pay Bill\n";
        cout << "5. Return to Main Menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            restaurant.displayMenu();
            break;
        case 2: {
            // Create an order for this table if it doesn't exist yet
            bool orderExists = false;
            for (const auto& order : restaurant.orders) {
                if (order.tableNumber == tableNum) {
                    orderExists = true;
                    break;
                }
            }
            if (!orderExists) {
                restaurant.createOrder(tableNum);
            }

            // Display the menu with numbered items for selection
            cout << "Select items to add to your order (enter 0 to finish):\n";
            restaurant.displayMenu();

            size_t itemChoice; // Changed to size_t
            int quantity;
            while (true) {
                cout << "Enter the item number (0 to finish): ";
                cin >> itemChoice;
                if (itemChoice == 0) break;

                // Calculate total number of items in the menu
                size_t totalItems = restaurant.appetizers.size() + restaurant.mainCourses.size() + restaurant.desserts.size();
                if (itemChoice < 1 || itemChoice > totalItems) {
                    cout << "Invalid item number. Please try again.\n";
                    continue;
                }

                cout << "Enter quantity: ";
                cin >> quantity;
                if (quantity <= 0) {
                    cout << "Quantity must be greater than 0. Please try again.\n";
                    continue;
                }

                // Determine which item was selected
                MenuItem* selectedItem = nullptr;
                if (itemChoice <= restaurant.appetizers.size()) {
                    selectedItem = &restaurant.appetizers[itemChoice - 1];
                }
                else if (itemChoice <= restaurant.appetizers.size() + restaurant.mainCourses.size()) {
                    selectedItem = &restaurant.mainCourses[itemChoice - restaurant.appetizers.size() - 1];
                }
                else {
                    selectedItem = &restaurant.desserts[itemChoice - restaurant.appetizers.size() - restaurant.mainCourses.size() - 1];
                }

                // Add the item to the order
                restaurant.addOrderItem(tableNum, selectedItem, quantity);
            }
            cout << "Order placed successfully.\n";
            break;
        }
        case 3: {
            // Display this table's order
            bool found = false;
            for (const auto& order : restaurant.orders) {
                if (order.tableNumber == tableNum) {
                    order.displayOrder();
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "No order found for Table " << tableNum << ".\n";
            }
            break;
        }
        case 4: {
            // Pay bill
            bool found = false;
            for (auto& order : restaurant.orders) { // Removed const to allow modification
                if (order.tableNumber == tableNum) {
                    order.payOrder();
                    restaurant.availableTables[tableNum - 1] = true; // Free the table
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "No order found for Table " << tableNum << ".\n";
            }
            break;
        }
        case 5:
            cout << "Returning to main menu.\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}
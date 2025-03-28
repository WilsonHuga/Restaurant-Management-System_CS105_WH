#include "MenuFunctions.h"
#include <iostream>

using namespace std;

void kitchenAttendantMenu(Restaurant& restaurant) {
    int choice;
    do {
        cout << "\nKitchen Attendant Menu:\n";
        cout << "1. View orders\n";
        cout << "2. Complete an order\n";
        cout << "3. Return to main menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            restaurant.displayOrders();
            break;
        case 2:
            restaurant.completeOrder();
            break;
        case 3:
            cout << endl;
            return;  // Return to main menu
        default:
            cout << "Invalid choice. Please try again.\n";
            break;
        }
    } while (choice != 3);
}
void customerMenu(Restaurant& restaurant) {
    int choice;
    int tableNumber;
    bool orderCreated = false;

    while (true) {
        cout << "\nCustomer Menu:\n";
        cout << "1. Choose a table\n";
        cout << "2. View menu\n";
        cout << "3. Place an order\n";
        cout << "4. Pay for an order\n";
        cout << "5. Return to main menu\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            restaurant.showAvailableTables();
            cout << "Enter table number: ";
            cin >> tableNumber;
            if (tableNumber < 1 || tableNumber > restaurant.tableCount || !restaurant.availableTables[tableNumber - 1]) {
                cout << "Invalid or occupied table. Please try again.\n";
            }
            else {
                restaurant.createOrder(tableNumber);
                orderCreated = true;
            }
            break;

        case 2:
            restaurant.displayMenu();
            break;

        case 3:
            if (!orderCreated) {
                cout << "Please choose a table first.\n";
            }
            else {
                restaurant.displayMenu();
                int itemChoice, qty;
                cout << "Enter the number of the item you want to order: ";
                cin >> itemChoice;

                cout << "Enter the quantity: ";
                cin >> qty;

                MenuItem* selectedItem = nullptr;
                if (itemChoice >= 1 && itemChoice <= restaurant.appetizers.size()) {
                    selectedItem = &restaurant.appetizers[itemChoice - 1];
                }
                else if (itemChoice > restaurant.appetizers.size() && itemChoice <= restaurant.appetizers.size() + restaurant.mainCourses.size()) {
                    selectedItem = &restaurant.mainCourses[itemChoice - 1 - restaurant.appetizers.size()];
                }
                else if (itemChoice > restaurant.appetizers.size() + restaurant.mainCourses.size() && itemChoice <= restaurant.appetizers.size() + restaurant.mainCourses.size() + restaurant.desserts.size()) {
                    selectedItem = &restaurant.desserts[itemChoice - 1 - restaurant.appetizers.size() - restaurant.mainCourses.size()];
                }

                if (selectedItem) {
                    restaurant.addOrderItem(tableNumber, selectedItem, qty);
                }
                else {
                    cout << "Invalid item choice.\n";
                }
            }
            break;

        case 4:
            if (!orderCreated) {
                cout << "Please choose a table first.\n";
            }
            else {
                cout << "Enter table number to pay for the order: ";
                cin >> tableNumber;

                bool found = false;
                for (auto& order : restaurant.orders) {
                    if (order.tableNumber == tableNumber) {
                        found = true;
                        order.payOrder();
                        orderCreated = false;
                        break;
                    }
                }

                if (!found) {
                    cout << "No order found for table " << tableNumber << ". Please ensure the table has an order.\n";
                }
            }
            break;

        case 5:
            cout << endl;
            return;  // Return to main menu

        default:
            cout << "Invalid choice. Please select again.\n";
            break;
        }
    }
}

//void managerMenu(Restaurant& restaurant) {
//    int choice;
//    do {
//        cout << "\nManager Menu:\n";
//        cout << "1. Add Menu Item\n";
//        cout << "2. Remove Menu Item\n";
//        cout << "3. Add Special Menu\n";
//        cout << "4. View All Customers' Tables and Bills\n";
//        cout << "5. Return to Main Menu\n";
//        cout << "Enter your choice: ";
//        cin >> choice;
//
//        switch (choice) {
//        case 1:
//            // Add menu item
//            restaurant.addMenuItem();
//            break;
//        case 2:
//            // Remove menu item
//            restaurant.removeMenuItem();
//            break;
//        case 3:
//            // Add a special menu item (could be a daily special, etc.)
//            restaurant.addSpecialMenu();
//            break;
//        case 4:
//            // View all customers' orders and bills
//            restaurant.viewCustomerTablesAndBills();
//            break;
//        case 5:
//            cout << endl;
//            return;  // Return to main menu
//        default:
//            cout << "Invalid choice. Please try again.\n";
//            break;
//        }
//    } while (choice != 5);
//}
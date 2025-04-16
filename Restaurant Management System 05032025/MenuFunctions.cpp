#include "MenuFunctions.h"
#include "File_Z.h"
#include "Example_Class_Structure.h"
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
        cout << "5. View All Bills\n";
        cout << "6. Generate Sales Report\n";
        cout << "7. Return to Main Menu\n";
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
            restaurant.viewAllBills();
            break;
        case 6:
            restaurant.generateSalesReport();
            break;
        case 7:
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
    bool found;
    //restaurant.loadSavedOrders();

    while (true) {
        restaurant.loadSavedOrders();

        cout << "\n==== Kitchen Attendant Menu ====\n";
        cout << "1. View All Orders\n";
        cout << "2. View Active Orders\n";
        cout << "3. Mark Order as In Preparation\n";
        cout << "4. Mark Order as Completed\n";
        cout << "5. Return to Main Menu\n";
        cout << endl;
        cout << "6. Monitor Kitchen Notifications\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            restaurant.displayOrders();
            break;
        case 2:
            // Display active orders categorized by status
        {
            cout << "\n==== Active Orders ====\n";
            // First display orders in "In Preparation" status
            cout << "\nIn Preparation:\n";
            bool hasInPrep = false;
            int inPrepCount = 1;
            for (const auto& order : restaurant.orders) {
                if (order.status == "In Preparation") {
                    cout << inPrepCount << ". Table " << order.tableNumber << " - ";
                    order.displayOrder();
                    hasInPrep = true;
                    inPrepCount++;
                }
            }
            if (!hasInPrep) {
                cout << "No orders currently in preparation.\n";
            }

            // Then display orders in "Pending" status
            cout << "\nPending:\n";
            bool hasPending = false;
            int pendingCount = 1;
            for (const auto& order : restaurant.orders) {
                if (order.status == "Pending") {
                    cout << pendingCount << ". Table " << order.tableNumber << " - ";
                    order.displayOrder();
                    hasPending = true;
                    pendingCount++;
                }
            }
            if (!hasPending) {
                cout << "No pending orders.\n";
            }
        }
        break;
        case 3:
            cout << "Enter table number: ";
            cin >> tableNum;

            // Find the latest active order for this table
            found = false;
            for (auto it = restaurant.orders.rbegin(); it != restaurant.orders.rend(); ++it) {
                if (it->tableNumber == tableNum &&
                    it->status != "Completed" &&
                    it->status != "Paid") {
                    found = true;
                    it->markInPreparation();
                    break;
                }
            }
            if (!found) {
                cout << "No active order found for Table " << tableNum << ".\n";
            }
            break;
        case 4:
            cout << "Enter table number to mark as completed: ";
            cin >> tableNum;

            // Find the latest active order for this table
            found = false;
            for (auto it = restaurant.orders.rbegin(); it != restaurant.orders.rend(); ++it) {
                if (it->tableNumber == tableNum &&
                    it->status != "Completed" &&
                    it->status != "Paid") {
                    found = true;
                    it->markAsCompleted();
                    break;
                }
            }
            if (!found) {
                cout << "No active order found for Table " << tableNum << ".\n";
            }
            break;
        case 5:
            cout << "Returning to main menu.\n";
            return;
        case 6: {
            TwoCli::FileManager<TwoCli::Message> messageManager("kitchen_notifications.txt");

            // Lambda to display messages
            auto displayFunc = [](const vector<TwoCli::Message>& messages) {
                for (const auto& msg : messages) {
                    cout << msg.rawText; // Display the entire notification block
                }
                };

            // Lambda to check for changes (by comparing size)
            auto hasChangesFunc = [](const vector<TwoCli::Message>& current, const vector<TwoCli::Message>& last) {
                return current.size() != last.size();
                };

            messageManager.startMonitor(displayFunc, hasChangesFunc, 7);
            restaurant.loadSavedOrders();
            break;
        }
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

void customerMenu(Restaurant& restaurant) {
    int choice;
    int tableNum;

    // Update table availability based on order status
    restaurant.updateTableAvailability();

    bool validTableSelected = false;
    while (!validTableSelected) {
        // Show available tables
        restaurant.showAvailableTables();

        cout << "Enter your table number: ";
        cin >> tableNum;

        // Validate table number
        if (tableNum < 1 || tableNum > restaurant.tableCount) {
            cout << "Invalid table number. Please try again.\n";
            continue;
        }

        // Check if table is available
        if (!restaurant.availableTables[tableNum - 1]) {
            cout << "Table " << tableNum << " is currently active and unavailable.\n";
            cout << "Please choose another table number from the available tables.\n";
            cout << endl;
            continue;
        }

        // If we reach here, the table selection is valid
        validTableSelected = true;
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
            // Check if there's already an active order for this table
            Order activeOrder(0);
            bool hasActiveOrder = restaurant.getActiveOrderForTable(tableNum, activeOrder);

            if (!hasActiveOrder) {
                restaurant.createOrder(tableNum);
            }

            // Display the menu with numbered items for selection
            cout << "Select items to add to your order (enter 0 to finish):\n";
            restaurant.displayMenu();

            size_t itemChoice;
            int quantity;
            bool itemsAdded = false;

            while (true) {
                cout << "Enter the item number (0 to finish): ";
                cin >> itemChoice;
                if (itemChoice == 0) break;

                // Calculate total number of items in the menu INCLUDING special menu items
                size_t totalItems = restaurant.appetizers.size() + restaurant.mainCourses.size() + restaurant.desserts.size() + restaurant.specialMenuItems.size();

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
                else if (itemChoice <= restaurant.appetizers.size() + restaurant.mainCourses.size() +
                    restaurant.desserts.size()) {
                    selectedItem = &restaurant.desserts[itemChoice - restaurant.appetizers.size() -
                        restaurant.mainCourses.size() - 1];
                }
                else {
                    // Special menu items
                    selectedItem = &restaurant.specialMenuItems[itemChoice - restaurant.appetizers.size() -
                        restaurant.mainCourses.size() - restaurant.desserts.size() - 1];
                }

                //tobe chanegd
                /*else {
                // Special menu items
                    size_t specialIndex = itemChoice - restaurant.appetizers.size() - 
                          restaurant.mainCourses.size() - restaurant.desserts.size() - 1;
                    if (specialIndex < restaurant.specialMenuItems.size()) {
                         selectedItem = &restaurant.specialMenuItems[specialIndex];
                    } else {
                        cout << "Invalid special menu item selection.\n";
                        continue;
                        }
                }*/

                // Add the item to the order
                restaurant.addOrderItem(tableNum, selectedItem, quantity);
                itemsAdded = true;
            }

            // Save the order to file
            if (itemsAdded) {
                // Find the active order and save it
                Order updatedActiveOrder(0);
                if (restaurant.getActiveOrderForTable(tableNum, updatedActiveOrder)) {
                    for (auto& order : restaurant.orders) {
                        if (order.tableNumber == tableNum &&
                            order.status != "Completed" &&
                            order.status != "Paid") {
                            order.saveOrderToFile();
                            restaurant.writeKitchenNotification(order, "NEW ORDER");
                            cout << "Order saved successfully.\n";
                            break;
                        }
                    }
                }
            }
            else {
                cout << "No items were added to the order.\n";
            }
            break;
        }
        case 3: {
            // Display this table's active order
            bool found = false;
            for (const auto& order : restaurant.orders) {
                if (order.tableNumber == tableNum &&
                    order.status != "Completed" &&
                    order.status != "Paid") {
                    order.displayOrder();
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "No active order found for Table " << tableNum << ".\n";
            }
            break;
        }
        case 4: {
            // Pay bill
            bool found = false;
            for (auto& order : restaurant.orders) {
                if (order.tableNumber == tableNum &&
                    order.status != "Completed" &&
                    order.status != "Paid") {
                    order.payOrder();
                    restaurant.availableTables[tableNum - 1] = true; // Free the table
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "No active order found for Table " << tableNum << ".\n";
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
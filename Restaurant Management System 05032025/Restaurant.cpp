#include "Restaurant.h"
#include "MenuItem.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>       // For time functions
#include <map>         
#include <iomanip>     // For setw, setfill (formatting)
#include <sstream>     // For stringstream

using namespace std;

bool Restaurant::getActiveOrderForTable(int tableNum, Order& result) {
    return Order::getActiveOrderForTable(tableNum, orders, result);
}



Restaurant::Restaurant(int tableCount) {
    this->tableCount = tableCount;
    availableTables.resize(tableCount, true);
}

void Restaurant::addAppetizer(Appetizer appetizer) {
    appetizers.push_back(appetizer);
}

void Restaurant::addMainCourse(MainCourse mainCourse) {
    mainCourses.push_back(mainCourse);
}

void Restaurant::addDessert(Dessert dessert) {
    desserts.push_back(dessert);
}

void Restaurant::displayMenu() {
    cout << "Menu:\n";
    cout << "Appetizers:\n";
    for (size_t i = 0; i < appetizers.size(); i++) {
        cout << (i + 1) << ". ";
        appetizers[i].displayInfo();
    }
    cout << "Main Courses:\n";
    for (size_t i = 0; i < mainCourses.size(); i++) {
        cout << (i + 1 + appetizers.size()) << ". ";
        mainCourses[i].displayInfo();
    }
    cout << "Desserts:\n";
    for (size_t i = 0; i < desserts.size(); i++) {
        cout << (i + 1 + appetizers.size() + mainCourses.size()) << ". ";
        desserts[i].displayInfo();
    }
}

void Restaurant::displayOrders() {
    cout << "Orders:\n";
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }
    for (auto& order : orders) {
        order.displayOrder();
    }
}

void Restaurant::createOrder(int tableNum) {
    // Check if there's already an active order for this table
    Order activeOrder(0);
    if (getActiveOrderForTable(tableNum, activeOrder)) {
        std::cout << "Table " << tableNum << " already has an active order. Please complete it first.\n";
        return;
    }

    // Create a new order since there's no active one
    Order newOrder(tableNum);
    orders.push_back(newOrder);
    availableTables[tableNum - 1] = false;
    std::cout << "Order created for Table " << tableNum << std::endl;

}

void Restaurant::addOrderItem(int tableNum, MenuItem* menuItem, int qty) {
    // Find the active order for this table
    for (auto& order : orders) {
        if (order.tableNumber == tableNum &&
            order.status != "Completed" &&
            order.status != "Paid") {
            OrderItem orderItem(*menuItem, qty);
            order.addItemToOrder(orderItem);
            std::cout << "Item added to order for Table " << tableNum << std::endl;
            return;
        }
    }
    std::cout << "Table " << tableNum << " does not have an active order.\n";
}

void Restaurant::completeOrder() {
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }

    vector<int> validIndexes;
    cout << "Orders available to complete:\n";
    int index = 1;
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].status == "Pending" || orders[i].status == "In Preparation") {
            cout << index << ". Table " << orders[i].tableNumber << " (" << orders[i].status << ")\n";
            validIndexes.push_back(static_cast<int>(i));
            index++;
        }
    }

    if (validIndexes.empty()) {
        cout << "No pending or in-preparation orders available.\n";
        return;
    }

    int choice;
    cout << "Enter the number of the order to complete: ";
    cin >> choice;

    if (choice < 1 || choice > validIndexes.size()) {
        cout << "Invalid selection. Please try again.\n";
        return;
    }

    orders[validIndexes[choice - 1]].markAsCompleted();
}

void Restaurant::markInPreparation(int tableNum) {
    for (auto& order : orders) {
        if (order.tableNumber == tableNum) {
            order.markInPreparation();
            return;
        }
    }
}

void Restaurant::showAvailableTables() {
    cout << "Available Tables: ";
    for (int i = 0; i < tableCount; i++) {
        if (availableTables[i]) {
            cout << (i + 1) << " ";
        }
    }
    cout << endl;
}

void Restaurant::viewCustomerTablesAndBills() {
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }

    for (const auto& order : orders) {
        cout << "Table " << order.getTableNumber() << " has the following order:\n";
        order.displayOrder();
        cout << "Total Bill: $" << order.calculateTotal() << "\n";
        cout << "\n"; // Fixed: Added newline
    }
}


// Load saved order
void Restaurant::loadSavedOrders() {
    std::ifstream file("order_details.txt");
    if (!file) {
        std::cout << "No saved orders found.\n";
        return;
    }

    orders.clear();
    std::string line;
    Order* currentOrder = nullptr;
    bool readingOrder = false;

    while (getline(file, line)) {
        if (line.find("Order for Table") != std::string::npos) {
            // If we were reading an order, save it before starting a new one
            if (currentOrder != nullptr) {
                orders.push_back(*currentOrder);
                delete currentOrder;
                currentOrder = nullptr;
            }

            // Extract table number and status
            int tableNum = 0;
            std::string statusStr;

            // Parse the table number
            size_t tablePos = line.find("Table") + 6;
            size_t spacePos = line.find(" ", tablePos);
            if (spacePos == std::string::npos) {
                spacePos = line.find("(", tablePos);
            }
            std::string tableNumStr = line.substr(tablePos, spacePos - tablePos);
            tableNum = std::stoi(tableNumStr);

            // Parse the status
            size_t statusStart = line.find("(") + 1;
            size_t statusEnd = line.find(")");
            if (statusStart != std::string::npos && statusEnd != std::string::npos) {
                statusStr = line.substr(statusStart, statusEnd - statusStart);
            }
            else {
                statusStr = "Pending"; // Default status if not specified
            }

            // Create new order
            currentOrder = new Order(tableNum);
            currentOrder->status = statusStr;
            readingOrder = true;
        }
        else if (readingOrder && line.find("Total: $") != std::string::npos) {
            // Extract total
            size_t dollarPos = line.find("$");
            if (dollarPos != std::string::npos) {
                std::string totalStr = line.substr(dollarPos + 1);
                try {
                    currentOrder->total = std::stod(totalStr);
                }
                catch (...) {
                    currentOrder->total = 0.0; // Default if parsing fails
                }
            }
            readingOrder = false;

            // Add the order to our list
            if (currentOrder != nullptr) {
                orders.push_back(*currentOrder);
                delete currentOrder;
                currentOrder = nullptr;
            }
        }
        else if (readingOrder && !line.empty() && currentOrder != nullptr) {
            // Parse order items
            // Format: "Item Name xQ - $P"
            size_t quantityPos = line.find(" x");
            size_t pricePos = line.find(" - $");

            if (quantityPos != std::string::npos && pricePos != std::string::npos) {
                std::string itemName = line.substr(0, quantityPos);

                try {
                    std::string quantityStr = line.substr(quantityPos + 2, pricePos - quantityPos - 2);
                    int quantity = std::stoi(quantityStr);

                    std::string priceStr = line.substr(pricePos + 4);
                    double itemTotal = std::stod(priceStr);
                    double itemPrice = quantity > 0 ? itemTotal / quantity : 0;

                    // Create menu item and add to order
                    MenuItem item(itemName, itemPrice);
                    OrderItem orderItem(item, quantity);
                    currentOrder->items.push_back(orderItem);
                }
                catch (...) {
                    // Skip this item if parsing fails
                    std::cout << "Warning: Failed to parse order item: " << line << std::endl;
                }
            }
        }
    }

    // Add the last order if we haven't yet
    if (currentOrder != nullptr) {
        orders.push_back(*currentOrder);
        delete currentOrder;
    }

    file.close();

    // Update table availability
    updateTableAvailability();

    std::cout << "Loaded " << orders.size() << " orders from file.\n";
}

void Restaurant::updateTableAvailability() {
    // First, mark all tables as available
    for (int i = 0; i < tableCount; i++) {
        availableTables[i] = true;
    }

    // Keep track of the most recent order status for each table
    std::map<int, std::string> latestOrderStatus;

    // Check all orders in memory
    for (const auto& order : orders) {
        // Store the status of this order
        latestOrderStatus[order.tableNumber] = order.status;
    }

    // Now mark tables as unavailable based on the latest status
    for (const auto& entry : latestOrderStatus) {
        int tableNum = entry.first;
        const std::string& status = entry.second;
        if (status != "Completed" && status != "Paid" &&
            tableNum > 0 && tableNum <= tableCount) {
            availableTables[tableNum - 1] = false;
        }
    }
}

// Load menu from menu.txt
void Restaurant::loadMenuFromFile() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "Menu file not found. Starting with empty menu.\n";
        return;
    }

    // Clear existing menu items
    appetizers.clear();
    mainCourses.clear();
    desserts.clear();

    string line;
    string type, name;
    double price;

    while (getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Parse the line (format: TYPE|NAME|PRICE)
        stringstream ss(line);
        string token;

        // Get type
        getline(ss, type, '|');

        // Get name
        getline(ss, name, '|');

        // Get price
        getline(ss, token, '|');
        try {
            price = stod(token);
        }
        catch (...) {
            cout << "Error parsing price for item: " << name << endl;
            continue;
        }

        // Add to appropriate vector based on type
        if (type == "Appetizer") {
            appetizers.push_back(Appetizer(name, price));
        }
        else if (type == "MainCourse") {
            mainCourses.push_back(MainCourse(name, price));
        }
        else if (type == "Dessert") {
            desserts.push_back(Dessert(name, price));
        }
        else {
            cout << "Unknown menu item type: " << type << endl;
        }
    }

    file.close();
    cout << "Menu loaded successfully from file.\n";
}

void Restaurant::saveMenuToFile() {
    ofstream file("menu.txt");
    if (!file) {
        cout << "Error: Cannot open menu.txt for writing.\n";
        return;
    }

    // Write appetizers
    for (const auto& item : appetizers) {
        file << "Appetizer|" << item.getName() << "|" << item.getPrice() << endl;
    }

    // Write main courses
    for (const auto& item : mainCourses) {
        file << "MainCourse|" << item.getName() << "|" << item.getPrice() << endl;
    }

    // Write desserts
    for (const auto& item : desserts) {
        file << "Dessert|" << item.getName() << "|" << item.getPrice() << endl;
    }

    file.close();
    cout << "Menu saved successfully to file.\n";
}




// Manager usage

void Restaurant::addMenuItem() {
    string type, name;
    double price;

    cin.ignore(); // Clear input buffer

    cout << "Enter menu type (appetizer, main, dessert): ";
    getline(cin, type);

    cout << "Enter item name: ";
    getline(cin, name);

    cout << "Enter price: $";
    cin >> price;

    if (type == "appetizer") {
        appetizers.push_back(Appetizer(name, price));
        cout << "Appetizer added successfully.\n";
    }
    else if (type == "main") {
        mainCourses.push_back(MainCourse(name, price));
        cout << "Main Course added successfully.\n";
    }
    else if (type == "dessert") {
        desserts.push_back(Dessert(name, price));
        cout << "Dessert added successfully.\n";
    }
    else {
        cout << "Invalid menu type. Please enter either 'appetizer', 'main', or 'dessert'.\n";
        return;
    }

    // Save menu to file after adding an item
    saveMenuToFile();
}

void Restaurant::removeMenuItem() {
    string type;
    int index;

    cout << "Select menu type to remove from:\n";
    cout << "1. Appetizer\n";
    cout << "2. Main Course\n";
    cout << "3. Dessert\n";
    cout << "Enter choice: ";

    int typeChoice;
    cin >> typeChoice;

    switch (typeChoice) {
    case 1:
        type = "appetizer";
        break;
    case 2:
        type = "main";
        break;
    case 3:
        type = "dessert";
        break;
    default:
        cout << "Invalid choice.\n";
        return;
    }

    // Display items of the selected type
    if (type == "appetizer") {
        cout << "Appetizers:\n";
        for (size_t i = 0; i < appetizers.size(); i++) {
            cout << (i + 1) << ". ";
            appetizers[i].displayInfo();
        }

        if (appetizers.empty()) {
            cout << "No appetizers to remove.\n";
            return;
        }

        cout << "Enter the number of the appetizer to remove: ";
        cin >> index;

        if (index < 1 || index > appetizers.size()) {
            cout << "Invalid selection.\n";
            return;
        }

        appetizers.erase(appetizers.begin() + index - 1);
        cout << "Appetizer removed successfully.\n";
    }
    else if (type == "main") {
        cout << "Main Courses:\n";
        for (size_t i = 0; i < mainCourses.size(); i++) {
            cout << (i + 1) << ". ";
            mainCourses[i].displayInfo();
        }

        if (mainCourses.empty()) {
            cout << "No main courses to remove.\n";
            return;
        }

        cout << "Enter the number of the main course to remove: ";
        cin >> index;

        if (index < 1 || index > mainCourses.size()) {
            cout << "Invalid selection.\n";
            return;
        }

        mainCourses.erase(mainCourses.begin() + index - 1);
        cout << "Main course removed successfully.\n";
    }
    else if (type == "dessert") {
        cout << "Desserts:\n";
        for (size_t i = 0; i < desserts.size(); i++) {
            cout << (i + 1) << ". ";
            desserts[i].displayInfo();
        }

        if (desserts.empty()) {
            cout << "No desserts to remove.\n";
            return;
        }

        cout << "Enter the number of the dessert to remove: ";
        cin >> index;

        if (index < 1 || index > desserts.size()) {
            cout << "Invalid selection.\n";
            return;
        }

        desserts.erase(desserts.begin() + index - 1);
        cout << "Dessert removed successfully.\n";
    }

    // Save menu to file after removing an item
    saveMenuToFile();
}

void Restaurant::addSpecialMenu() {
    string name;
    double price;

    cin.ignore(); // Clear input buffer

    cout << "Enter special menu item name: ";
    getline(cin, name);

    cout << "Enter special item price: $";
    cin >> price;

    mainCourses.push_back(MainCourse(name, price));
    cout << "Special menu item added successfully.\n";

    // Save menu to file after adding a special item
    saveMenuToFile();
}

void Restaurant::viewAllBills() {
    cout << "\n===== All Customer Bills =====\n";
    if (orders.empty()) {
        cout << "No orders/bills available to view.\n";
        return;
    }

    double totalRevenue = 0.0;

    cout << left << std::setw(10) << "Table"
        << setw(15) << "Status"
        << setw(10) << "Amount" << endl;
    cout << string(40, '-') << endl;

    for (const auto& order : orders) {
        cout << left << setw(10) << order.getTableNumber()
            << setw(15) << order.status
            << "$" << fixed << setprecision(2) << order.calculateTotal() << endl;

        // Only count completed or paid orders towards revenue
        if (order.status == "Completed" || order.status == "Paid") {
            totalRevenue += order.calculateTotal();
        }
    }

    cout << string(40, '-') << endl;
    cout << "Total Revenue (Completed/Paid Orders): $" << fixed << setprecision(2) << totalRevenue << endl;
}

void Restaurant ::generateSalesReport() {
    cout << "\n========================================\n";
    cout << "           SALES REPORT\n";
    cout << "========================================\n\n";

    /*double totalRevenue = calculateTotalRevenue();
    cout << "TOTAL REVENUE: $" << fixed << setprecision(2) << totalRevenue << "\n\n";*/



    cout << "\n========================================\n";
    cout << "          END OF REPORT\n";
    cout << "========================================\n";
}
#include "Restaurant.h"
#include <iostream>
#include <algorithm>

using namespace std;

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
    Order newOrder(tableNum);
    orders.push_back(newOrder);
    availableTables[tableNum - 1] = false;
    cout << "Order created for Table " << tableNum << endl;
}

void Restaurant::addOrderItem(int tableNum, MenuItem* menuItem, int qty) {
    for (auto& order : orders) {
        if (order.tableNumber == tableNum) {
            OrderItem orderItem(*menuItem, qty);
            order.addItemToOrder(orderItem);
            cout << "Item added to order for Table " << tableNum << endl;
            return;
        }
    }
    cout << "Table " << tableNum << " does not have an order.\n";
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

void Restaurant::addMenuItem() {
    string type, name;
    double price;
    cout << "Enter menu type (appetizer, main, dessert): ";
    cin >> type;
    cout << "Enter item name: ";
    cin >> name;
    cout << "Enter price: ";
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
    }
}

void Restaurant::removeMenuItem() {
    string type, name;
    cout << "Enter menu type (appetizer, main, dessert): ";
    cin >> type;
    cout << "Enter item name to remove: ";
    cin >> name;

    if (type == "appetizer") {
        auto it = find_if(appetizers.begin(), appetizers.end(), [&name](const Appetizer& item) {
            return item.getName() == name;
            });
        if (it != appetizers.end()) {
            appetizers.erase(it);
            cout << "Appetizer removed successfully.\n";
        }
        else {
            cout << "Appetizer not found.\n";
        }
    }
    else if (type == "main") {
        auto it = find_if(mainCourses.begin(), mainCourses.end(), [&name](const MainCourse& item) {
            return item.getName() == name;
            });
        if (it != mainCourses.end()) {
            mainCourses.erase(it);
            cout << "Main Course removed successfully.\n";
        }
        else {
            cout << "Main Course not found.\n";
        }
    }
    else if (type == "dessert") {
        auto it = find_if(desserts.begin(), desserts.end(), [&name](const Dessert& item) {
            return item.getName() == name;
            });
        if (it != desserts.end()) {
            desserts.erase(it);
            cout << "Dessert removed successfully.\n";
        }
        else {
            cout << "Dessert not found.\n";
        }
    }
    else {
        cout << "Invalid menu type. Please enter either 'appetizer', 'main', or 'dessert'.\n";
    }
}

void Restaurant::addSpecialMenu() {
    string name;
    double price;
    cout << "Enter special menu item name: ";
    cin >> name;
    cout << "Enter special item price: ";
    cin >> price;

    mainCourses.push_back(MainCourse(name, price));
    cout << "Special menu item added successfully.\n";
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
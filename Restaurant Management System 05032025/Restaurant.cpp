#include "Restaurant.h"
#include <iostream>

using namespace std;


Restaurant::Restaurant(int tableCount) {
    //loadOrdersFromFile("order_details.txt");
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
    for (int i = 0; i < appetizers.size(); i++) {
        cout << (i + 1) << ". ";
        appetizers[i].displayInfo();
    }
    cout << "Main Courses:\n";
    for (int i = 0; i < mainCourses.size(); i++) {
        cout << (i + 1 + appetizers.size()) << ". ";
        mainCourses[i].displayInfo();
    }
    cout << "Desserts:\n";
    for (int i = 0; i < desserts.size(); i++) {
        cout << (i + 1 + appetizers.size() + mainCourses.size()) << ". ";
        desserts[i].displayInfo();
    }
}

void Restaurant::displayOrders() {
    cout << "Orders:\n";
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

    // Display orders that are "Pending" or "In Preparation"
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

    // Complete the selected order
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







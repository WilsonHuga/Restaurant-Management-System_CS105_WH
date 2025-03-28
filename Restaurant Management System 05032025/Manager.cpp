#include "Restaurant.h"
#include <iostream>

using namespace std;

void Restaurant::addMenuItem() {
    string type, name;
    double price;
    cout << "Enter menu type (appetizer, main, dessert): ";
    cin >> type;
    cout << "Enter item name: ";
    cin >> name;
    cout << "Enter price: ";
    cin >> price;

    // Validate menu type input
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
    cout << "Enter menu type (appetizer, main, dessert): ";
    cin >> type;
    cout << "Enter item name to remove: ";
    cin >> name;

    // Validate menu type input and attempt removal
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

    // Add the special item to mainCourses (can also create a separate vector for special items)
    mainCourses.push_back(MainCourse(name, price));
    cout << "Special menu item added successfully.\n";
}

void Restaurant::viewCustomerTablesAndBills() {
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }

    for (auto& order : orders) {
        cout << "Table " << order.getTableNumber() << " has the following order:\n";
        order.displayOrder();
        cout << "Total Bill: $" << order.calculateTotal() << "\n";
    }
}

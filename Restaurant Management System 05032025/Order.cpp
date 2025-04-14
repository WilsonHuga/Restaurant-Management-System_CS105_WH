#include "Order.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <map>
#include <iomanip>

using namespace std;

OrderItem::OrderItem(MenuItem menuItem, int qty) {
    item = menuItem;
    quantity = qty;
}

void OrderItem::displayOrderItem() const {
    cout << item.name << " x" << quantity << " - $" << item.price * quantity << endl;
}

Order::Order(int tableNum) {
    tableNumber = tableNum;
    status = "Pending";
    total = 0.0;
}

void Order::addItemToOrder(const OrderItem& orderItem) {
    items.push_back(orderItem);
    total += orderItem.item.price * orderItem.quantity;
}

void Order::displayOrder() const {
    cout << "Order for Table " << tableNumber << " (" << status << "):" << endl;
    for (const auto& item : items) {
        item.displayOrderItem();
    }
    cout << "Total: $" << total << endl;
}

void Order::updateOrderStatusInFile() {
    ifstream inFile("kitchen_notifications.txt");
    if (!inFile) {
        cout << "Error: Unable to open order file for reading.\n";
        return;
    }

    vector<string> fileLines;
    string line;
    while (getline(inFile, line)) {
        fileLines.push_back(line);
    }
    inFile.close();

    bool statusUpdated = false;
    for (int i = fileLines.size() - 1; i >= 0; i--) {
        string tableStr = "Order for Table " + to_string(tableNumber) + " (";
        if (fileLines[i].find(tableStr) != string::npos) {
            size_t statusStart = fileLines[i].find("(") + 1;
            size_t statusEnd = fileLines[i].find(")");
            fileLines[i].replace(statusStart, statusEnd - statusStart, status);
            statusUpdated = true;
            break;
        }
    }

    ofstream outFile("kitchen_notifications.txt");
    if (!outFile) {
        cout << "Error: Unable to open order file for writing.\n";
        return;
    }

    for (const auto& fileLine : fileLines) {
        outFile << fileLine << endl;
    }
    outFile.close();

    if (statusUpdated) {
        cout << "Order status updated in file.\n";
    }
    else {
        saveOrderToFile();
    }
}

void Order::markAsCompleted() {
    if (status == "Completed") {
        cout << "Order for Table " << tableNumber << " is already completed.\n";
        return;
    }
    status = "Completed";
    cout << "Order for Table " << tableNumber << " is now completed." << endl;
    updateOrderStatusInFile();
}

void Order::markInPreparation() {
    if (status == "In Preparation") {
        cout << "Order for Table " << tableNumber << " is already in preparation.\n";
        cout << "Please choose another table." << endl;
        return;
    }
    status = "In Preparation";
    cout << "Order for Table " << tableNumber << " is now in preparation." << endl;
    updateOrderStatusInFile();
}

void Order::payOrder() {
    cout << "The total amount to be paid for the order is: $" << total << endl;
    cout << "Thank you for your payment. Your order is now closed.\n";
    status = "Paid";
    saveOrderToFile();
}

void Order::saveOrderToFile() {
    vector<string> fileContents;
    ifstream inFile("kitchen_notifications.txt");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            fileContents.push_back(line);
        }
        inFile.close();
    }

    stringstream orderContent;
    orderContent << "Order for Table " << tableNumber << " (" << status << "):" << endl;
    for (const auto& item : items) {
        orderContent << item.item.name << " x" << item.quantity << " - $"
            << (item.item.price * item.quantity) << endl;
    }
    orderContent << "Total: $" << total << endl << endl;

    ofstream outFile("kitchen_notifications.txt", ios::app);
    if (!outFile.is_open()) {
        cout << "Error: Unable to open order file for writing.\n";
        return;
    }

    if (fileContents.empty()) {
        outFile << orderContent.str();
    }
    else {
        outFile << orderContent.str();
    }

    outFile.close();
    cout << "Order details saved to kitchen_notifications.txt.\n";
}

double Order::calculateTotal() const {
    return total;
}

int Order::getTableNumber() const {
    return tableNumber;
}

void Order::loadOrdersFromFile(vector<Order>& orders) {
    ifstream inFile("kitchen_notifications.txt");
    if (!inFile) {
        cout << "Error: Unable to open file for reading.\n";
        return;
    }

    orders.clear();
    string line;
    Order currentOrder(0);
    bool readingOrder = false;

    while (getline(inFile, line)) {
        if (line.find("Order for Table") != string::npos) {
            int tableNum;
            string status = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1);
            stringstream ss(line);
            string temp;
            ss >> temp >> temp >> temp >> tableNum;
            currentOrder = Order(tableNum);
            currentOrder.status = status;
            currentOrder.items.clear();
            readingOrder = true;
        }
        else if (readingOrder && line.find("Total: $") != string::npos) {
            double total;
            stringstream ss(line);
            string temp;
            ss >> temp >> total;
            currentOrder.total = total;
            orders.push_back(currentOrder);
            readingOrder = false;
        }
        else if (readingOrder) {
            string name = line.substr(0, line.find(" x"));
            int quantity;
            double price;
            stringstream ss(line.substr(name.length()));
            char dummy;
            ss >> dummy >> quantity >> dummy >> dummy >> price;

            MenuItem menuItem(name, price / quantity);
            currentOrder.items.emplace_back(menuItem, quantity);
        }
    }

    inFile.close();
    cout << "Orders loaded successfully.\n";
}

bool Order::getActiveOrderForTable(int tableNum, const vector<Order>& orders, Order& result) {
    for (auto it = orders.rbegin(); it != orders.rend(); ++it) {
        if (it->tableNumber == tableNum &&
            it->status != "Completed" &&
            it->status != "Paid") {
            result = *it;
            return true;
        }
    }
    return false;
}
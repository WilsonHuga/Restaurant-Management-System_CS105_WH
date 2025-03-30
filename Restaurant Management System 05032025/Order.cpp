#include "Order.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

OrderItem::OrderItem(MenuItem menuItem, int qty) {
    item = menuItem;
    quantity = qty;
}

void OrderItem::displayOrderItem() const { // Added const
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
    saveOrderToFile();
}

void Order::displayOrder() const { // Added const
    cout << "Order for Table " << tableNumber << " (" << status << "):" << endl;
    for (const auto& item : items) {
        item.displayOrderItem();
    }
    cout << "Total: $" << total << endl;
}

void Order::markAsCompleted() {
    if (status == "Completed") {
        cout << "Order for Table " << tableNumber << " is already completed.\n";
        return;
    }
    status = "Completed";
    cout << "Order for Table " << tableNumber << " is now completed." << endl;
    saveOrderToFile();
}

void Order::markInPreparation() {
    status = "In Preparation";
    cout << "Order for Table " << tableNumber << " is now in preparation." << endl;
}

void Order::payOrder() {
    cout << "The total amount to be paid for the order is: $" << total << endl;
    cout << "Thank you for your payment. Your order is now closed.\n";
    status = "Paid";
    saveOrderToFile();
}

void Order::saveOrderToFile() {
    ofstream outFile("order_details.txt", ios::app);
    if (outFile.is_open()) {
        outFile << "Order for Table " << tableNumber << " (" << status << "):\n";
        for (const auto& item : items) {
            outFile << item.item.name << " x" << item.quantity << " - $" << item.item.price * item.quantity << "\n";
        }
        outFile << "Total: $" << total << "\n\n";
        outFile.close();
        cout << "Order details saved to order_details.txt.\n";
    }
    else {
        cout << "Error saving to file.\n";
    }
}

double Order::calculateTotal() const { // Added const
    return total;
}

int Order::getTableNumber() const { // Added const
    return tableNumber;
}

void Order::loadOrdersFromFile(std::vector<Order>& orders) {
    ifstream inFile("order_details.txt");
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
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
    //saveOrderToFile();
}

void Order::displayOrder() const { // Added const
    cout << "Order for Table " << tableNumber << " (" << status << "):" << endl;
    for (const auto& item : items) {
        item.displayOrderItem();
    }
    cout << "Total: $" << total << endl;
}

//020452025
void Order::updateOrderStatusInFile() {
    // Read the entire file
    std::ifstream inFile("kitchen_notifications.txt");// was order tx 
    if (!inFile) {
        std::cout << "Error: Unable to open order file for reading.\n";
        return;
    }

    std::vector<std::string> fileLines;
    std::string line;
    while (getline(inFile, line)) {
        fileLines.push_back(line);
    }
    inFile.close();

    // Find and update the specific order status
    // We need to find the most recent order for this table
    bool statusUpdated = false;
    for (int i = fileLines.size() - 1; i >= 0; i--) {
        std::string tableStr = "Order for Table " + std::to_string(tableNumber) + " (";
        if (fileLines[i].find(tableStr) != std::string::npos) {
            // We found an order for this table, update its status
            size_t statusStart = fileLines[i].find("(") + 1;
            size_t statusEnd = fileLines[i].find(")");
            fileLines[i].replace(statusStart, statusEnd - statusStart, status);
            statusUpdated = true;
            break; // We only update the most recent order for this table
        }
    }

    // Write the updated content back to the file
    std::ofstream outFile("kitchen_notifications.txt");//was order txt 
    if (!outFile) {
        std::cout << "Error: Unable to open order file for writing.\n";
        return;
    }

    for (const auto& fileLine : fileLines) {
        outFile << fileLine << std::endl;
    }
    outFile.close();

    if (statusUpdated) {
        std::cout << "Order status updated in file.\n";
    }
    else {
        // If we didn't find the order, add a new one
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
    //saveOrderToFile();
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
    // First read all existing orders from the file
    std::vector<std::string> fileContents;
	std::ifstream inFile("kitchen_notifications.txt");//was order txt
    if (inFile.is_open()) {
        std::string line;
        while (getline(inFile, line)) {
            fileContents.push_back(line);
        }
        inFile.close();
    }

    // Create the new order content
    std::stringstream orderContent;
    orderContent << "Order for Table " << tableNumber << " (" << status << "):" << std::endl;
    for (const auto& item : items) {
        orderContent << item.item.name << " x" << item.quantity << " - $"
            << (item.item.price * item.quantity) << std::endl;
    }
    orderContent << "Total: $" << total << std::endl << std::endl;

    // Open file for writing (append mode)
	std::ofstream outFile("kitchen_notifications.txt", std::ios::app);// was order txt
    if (!outFile.is_open()) {
        std::cout << "Error: Unable to open order file for writing.\n";
        return;
    }

    // If the file was empty, just write the new order
    if (fileContents.empty()) {
        outFile << orderContent.str();
    }
    else {
        // Otherwise append the new order
        outFile << orderContent.str();
    }

    outFile.close();
    std::cout << "Order details saved to order_details.txt.\n";
}

double Order::calculateTotal() const { // Added const
    return total;
}

int Order::getTableNumber() const { // Added const
    return tableNumber;
}

void Order::loadOrdersFromFile(std::vector<Order>& orders) {
	ifstream inFile("kitchen_notifications.txt");// was order txt
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

bool Order::getActiveOrderForTable(int tableNum, const std::vector<Order>& orders, Order& result) {
    // Iterate through orders in reverse (assuming newer orders are added to the end)
    // This will find the most recent order for the table first
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

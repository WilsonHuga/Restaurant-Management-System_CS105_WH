#include "Order.h"
#include <fstream>
#include <iostream>

using namespace std;

OrderItem::OrderItem(MenuItem menuItem, int qty) {
    item = menuItem;
    quantity = qty;
}

void OrderItem::displayOrderItem() {
    cout << item.name << " x" << quantity << " - $" << item.price * quantity << endl;
}

Order::Order(int tableNum) {
    tableNumber = tableNum;
    status = "Pending";
    total = 0;
}

void Order::addItemToOrder(const OrderItem& orderItem) {
    items.push_back(orderItem);
    total += orderItem.item.price * orderItem.quantity;
    saveOrderToFile();
}

void Order::displayOrder() {
    cout << "Order for Table " << tableNumber << " (" << status << "):" << endl;
    for (auto& item : items) {
        item.displayOrderItem();
    }
    cout << "Total: $" << total << endl;
}

void Order::markAsCompleted() {  // Renamed from completeOrdering()
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
        for (auto& item : items) {
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

//void Order::loadOrdersFromFile(vector<Order>& orders) {
//    ifstream inFile("order_details.txt");
//    if (!inFile) {
//        cout << "Error: Unable to open file for reading.\n";
//        return;
//    }
//
//    orders.clear();
//    string line;
//    Order currentOrder(0);
//    bool readingOrder = false;
//
//    while (getline(inFile, line)) {
//        if (line.find("Ord`er for Table") != string::npos) {
//            int tableNum;
//            string status = line.substr(line.find("(") + 1, line.find(")") - line.find("(") - 1);
//            sscanf(line.c_str(), "Order for Table %d", &tableNum);
//            currentOrder = Order(tableNum);
//            currentOrder.status = status;
//            currentOrder.items.clear();
//            readingOrder = true;
//        }
//        else if (readingOrder && line.find("Total: $") != string::npos) {
//            sscanf(line.c_str(), "Total: $%lf", &currentOrder.total);
//            orders.push_back(currentOrder);
//            readingOrder = false;
//        }
//        else if (readingOrder) {
//            string name = line.substr(0, line.find(" x"));
//            int quantity;
//            double price;
//            sscanf(line.c_str() + name.length(), " x%d - $%lf", &quantity, &price);
//
//            MenuItem menuItem = { name, price / quantity };
//            currentOrder.items.emplace_back(menuItem, quantity);
//        }
//    }
//
//    inFile.close();
//    cout << "Orders loaded successfully.\n";
//}



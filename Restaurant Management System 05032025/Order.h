#ifndef ORDER_H
#define ORDER_H

#include "MenuItem.h"
#include <vector>
#include <string>

class OrderItem {
public:
    MenuItem item;
    int quantity;
    
    OrderItem(MenuItem menuItem, int qty);
    void displayOrderItem() const;
};

class Order {
public:
    int tableNumber;
    std::string status;
    double total;
    std::vector<OrderItem> items;
    
    //06042025
    /*string orderID;
    string orderTime;*/


    Order(int tableNum);
    void addItemToOrder(const OrderItem& orderItem);
    void displayOrder() const;
    void markAsCompleted();
    void markInPreparation();
    void payOrder();
    void saveOrderToFile();
    void updateOrderStatusInFile();
    double calculateTotal() const;
    int getTableNumber() const;
    
    // New static method to get the most recent active order for a table
    static bool getActiveOrderForTable(int tableNum, const std::vector<Order>& orders, Order& result);
    // Static method to load orders from file
    static void loadOrdersFromFile(std::vector<Order>& orders);
};

#endif
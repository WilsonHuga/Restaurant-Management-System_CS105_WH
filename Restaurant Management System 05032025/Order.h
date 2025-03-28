#ifndef ORDER_H
#define ORDER_H

#include <vector>
#include <string>
#include "MenuItem.h"

class OrderItem {
public:
    MenuItem item;
    int quantity;

    OrderItem(MenuItem menuItem, int qty);
    void displayOrderItem();
};

class Order {
public:
    int tableNumber;
    std::vector<OrderItem> items;
    std::string status;
    float total;

    Order(int tableNum);
    void addItemToOrder(const OrderItem& orderItem);
    void displayOrder();
    //void completeOrdering();
    void markInPreparation();
    void markAsCompleted();
    void saveOrderToFile();
    //void loadOrdersFromFile(vector<Order>& orders);
    void payOrder();
};

#endif
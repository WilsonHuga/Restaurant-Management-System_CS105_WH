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
    void displayOrderItem() const; // Added const
};

class Order {
public:
    int tableNumber;
    std::vector<OrderItem> items;
    std::string status;
    double total;
    Order(int tableNum);
    void addItemToOrder(const OrderItem& orderItem);
    void displayOrder() const; // Added const
    void completeOrdering();
    void markInPreparation();
    void markAsCompleted();
    void saveOrderToFile();
    void loadOrdersFromFile(std::vector<Order>& orders);
    void payOrder();

    double calculateTotal() const; // Added const
    int getTableNumber() const; // Added const
};
#endif
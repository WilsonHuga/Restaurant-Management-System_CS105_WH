#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <vector>
#include <string>
#include "MenuItem.h"   // Ensure this header exists
#include "Order.h"      // Ensure this header exists

class Restaurant {
public:
    std::vector<Appetizer> appetizers;
    std::vector<MainCourse> mainCourses;
    std::vector<Dessert> desserts;
    std::vector<Order> orders;

    Restaurant(int tableCount);

    /*Restaurant();
    void loadOrdersFromFile(const std::string& filename);
    void saveOrdersToFile(const std::string& filename);*/

    int tableCount;
    std::vector<bool> availableTables;

    Restaurant(int tableCount);
    void addAppetizer(Appetizer appetizer);
    void addMainCourse(MainCourse mainCourse);
    void addDessert(Dessert dessert);
    void displayMenu();
    void displayOrders();
    void createOrder(int tableNum);
    void addOrderItem(int tableNum, MenuItem* menuItem, int qty);
    void completeOrder();
    void markInPreparation(int tableNum);
    void showAvailableTables();

    // Manager Functions
    void addMenuItem();
    void removeMenuItem();
    void addSpecialMenu();
    void viewCustomerTablesAndBills();

};

#endif

#ifndef MENUITEM_H
#define MENUITEM_H
#include <string>
#include <iostream>

class MenuItem {
public:
    std::string name;
    double price;
    MenuItem();
    //MenuItem(std::string n, float p);
    MenuItem(const std::string& n, double p) : name(n), price(p) {}
    virtual void displayItem();
};

class Appetizer : public MenuItem {
public:
    Appetizer(std::string n, double p);
    void displayInfo() const;
    std::string getName() const; // Added const method
};

class MainCourse : public MenuItem {
public:
    MainCourse(std::string n, double p);
    void displayInfo() const;
    std::string getName() const; // Added const method
};

class Dessert : public MenuItem {
public:
    Dessert(std::string n, double p);
    void displayInfo() const;
    std::string getName() const; // Added const method
};
#endif
#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <iostream>

class MenuItem {
public:
    std::string name;
    double price;

    MenuItem();
    MenuItem(std::string n, double p) : name(n), price(p) {}

    void displayItem();
    virtual void displayInfo() const {
        std::cout << name << " - $" << price << std::endl;
    }

    virtual std::string getName() const {
        return name;
    }
    double getPrice() const;
};

class Appetizer : public MenuItem {
public:
    Appetizer(std::string n, double p) : MenuItem(n, p) {}
    void displayInfo() const override;
    std::string getName() const override;
};

class MainCourse : public MenuItem {
public:
    MainCourse(std::string n, double p) : MenuItem(n, p) {}
    void displayInfo() const override;
    std::string getName() const override;
};

class Dessert : public MenuItem {
public:
    Dessert(std::string n, double p) : MenuItem(n, p) {}
    void displayInfo() const override;
    std::string getName() const override;
};

#endif
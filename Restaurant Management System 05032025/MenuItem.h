#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <iostream>

class MenuItem {
public:
    std::string name;
    float price;

    MenuItem();
    //MenuItem(std::string n, float p);

    virtual void displayItem();
};

class Appetizer : public MenuItem {
public:
    Appetizer(std::string n, float p);
    void displayInfo() const;
};

class MainCourse : public MenuItem {
public:
    MainCourse(std::string n, float p);
    void displayInfo() const;
};

class Dessert : public MenuItem {
public:
    Dessert(std::string n, float p);
    void displayInfo() const;
};

#endif
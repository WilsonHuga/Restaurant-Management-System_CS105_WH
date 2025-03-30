#include "MenuItem.h"
using namespace std;

MenuItem::MenuItem() {
    name = "";
    price = 0.0;
}

//MenuItem::MenuItem() {
//    // Default constructor implementation
//}
//
//MenuItem::MenuItem(std::string n, float p) : name(n), price(p) {
//    // Constructor implementation
//}

void MenuItem::displayItem() {
    cout << name << " - $" << price << endl;
}

Appetizer::Appetizer(string n, double p) {
    name = n;
    price = p;
}

void Appetizer::displayInfo() const {
    cout << "Appetizer: " << name << " - $" << price << endl;
}

// Add getName implementation for Appetizer
std::string Appetizer::getName() const {
    return name;
}

MainCourse::MainCourse(string n, double p) {
    name = n;
    price = p;
}

void MainCourse::displayInfo() const {
    cout << "Main Course: " << name << " - $" << price << endl;
}

// Add getName implementation for MainCourse
std::string MainCourse::getName() const {
    return name;
}

Dessert::Dessert(string n, double p) {
    name = n;
    price = p;
}

void Dessert::displayInfo() const {
    cout << "Dessert: " << name << " - $" << price << endl;
}

// Add getName implementation for Dessert
std::string Dessert::getName() const {
    return name;
}
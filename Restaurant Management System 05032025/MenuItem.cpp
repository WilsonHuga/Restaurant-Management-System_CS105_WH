#include "MenuItem.h"
using namespace std;

MenuItem::MenuItem() {
    name = "";
    price = 0.0;
}

void MenuItem::displayItem() {
    cout << name << " - $" << price << endl;
}


void Appetizer::displayInfo() const {
    cout << "Appetizer: " << name << " - $" << price << endl;
}

// Add getName implementation for Appetizer
std::string Appetizer::getName() const {
    return name;
}


void MainCourse::displayInfo() const {
    cout << "Main Course: " << name << " - $" << price << endl;
}

// Add getName implementation for MainCourse
string MainCourse::getName() const {
    return name;
}



void Dessert::displayInfo() const {
    cout << "Dessert: " << name << " - $" << price << endl;
}

// Add getName implementation for Dessert
string Dessert::getName() const {
    return name;
}

double MenuItem::getPrice() const {
    return price;
}
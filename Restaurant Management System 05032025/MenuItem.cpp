#include "MenuItem.h"

using namespace std;

MenuItem::MenuItem() {
    name = "";
    price = 0;
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

Appetizer::Appetizer(string n, float p) {
    name = n;
    price = p;
}

void Appetizer::displayInfo() const {
    cout << "Appetizer: " << name << " - $" << price << endl;
}

MainCourse::MainCourse(string n, float p) {
    name = n;
    price = p;
}

void MainCourse::displayInfo() const {
    cout << "Main Course: " << name << " - $" << price << endl;
}

Dessert::Dessert(string n, float p) {
    name = n;
    price = p;
}

void Dessert::displayInfo() const {
    cout << "Dessert: " << name << " - $" << price << endl;
}
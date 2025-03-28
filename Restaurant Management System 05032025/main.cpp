#include <iostream>
#include "Restaurant.h"
#include "MenuFunctions.h"

using namespace std;

int main() {
    int userChoice;
    Restaurant restaurant(20);

    Appetizer appetizer1("Spring Rolls", 5.5);
    Appetizer appetizer2("Calamari", 7.5);
    MainCourse mainCourse1("Steak", 20.0);
    MainCourse mainCourse2("Salmon", 18.0);
    MainCourse mainCourse3("Piadina Ricotta", 15.0);
    Dessert dessert1("Chocolate Creme", 6.5);
    Dessert dessert2("Tiramisu", 7.5);
    Dessert dessert3("Cheesecake", 6.0);

    restaurant.addAppetizer(appetizer1);
    restaurant.addAppetizer(appetizer2);
    restaurant.addMainCourse(mainCourse1);
    restaurant.addMainCourse(mainCourse2);
    restaurant.addMainCourse(mainCourse3);
    restaurant.addDessert(dessert1);
    restaurant.addDessert(dessert2);
    restaurant.addDessert(dessert3);

    while (true) {
        cout << "Welcome to the Restaurant System!\n";
        cout << "Are you a (1) Kitchen Attendant, (2) Customer, or (3) Manager? (Enter 4 to Exit): ";
        cin >> userChoice;

        if (userChoice == 1) {
            kitchenAttendantMenu(restaurant);
        }
        else if (userChoice == 2) {
            customerMenu(restaurant);
        }
        else if (userChoice == 3) {
            managerMenu(restaurant);  // New Manager menu
        }
        else if (userChoice == 4) {
            cout << "Exiting the system.\n";
            break;  // Exit the loop
        }
        else {
            cout << "Invalid role choice. Please try again.\n";
        }
    }

    return 0;
}
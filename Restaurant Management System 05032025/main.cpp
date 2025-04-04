#include <iostream>
#include "Restaurant.h"
#include "MenuFunctions.h"

using namespace std;

int main() {
    int userChoice;
    Restaurant restaurant(20);

    restaurant.loadMenuFromFile();

    if (restaurant.appetizers.empty() && restaurant.mainCourses.empty() && restaurant.desserts.empty()) {
        cout << "Creating default menu...\n";
        // Add default menu items
        restaurant.addAppetizer(Appetizer("Spring Rolls", 5.5));
        restaurant.addAppetizer(Appetizer("Calamari", 7.5));
        restaurant.addMainCourse(MainCourse("Steak", 20.0));
        restaurant.addMainCourse(MainCourse("Salmon", 18.0));
        restaurant.addMainCourse(MainCourse("Piadina Ricotta", 15.0));
        restaurant.addDessert(Dessert("Chocolate Creme", 6.5));
        restaurant.addDessert(Dessert("Tiramisu", 7.5));
        restaurant.addDessert(Dessert("Cheesecake", 6.0));

        // Save the default menu to file
        restaurant.saveMenuToFile();
    }

    /*Appetizer appetizer1("Spring Rolls", 5.5);
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
    restaurant.addDessert(dessert3);*/

    restaurant.loadSavedOrders();

    while (true) {
        cout << "\n===================================\n";
        cout << "Welcome to the Restaurant System!\n";
        cout << "===================================\n";
        cout << "1. Kitchen Attendant\n";
        cout << "2. Customer\n";
        cout << "3. Manager\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
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
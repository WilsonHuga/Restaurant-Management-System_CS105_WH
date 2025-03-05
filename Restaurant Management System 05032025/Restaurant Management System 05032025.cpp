#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

// Menu Item class
class MenuItem {
public:
    string name;
    float price;

    MenuItem() {
        name = "";
        price = 0;
    }

    virtual void displayItem() {
        cout << name << " - $" << price << endl;
    }
};

class Appetizer : public MenuItem {
public:
    Appetizer(string n, float p) {
        name = n;
        price = p;
    }

    void displayInfo() const {
        cout << "Appetizer: " << name << " - $" << price << endl;
    }
};

class MainCourse : public MenuItem {
public:
    MainCourse(string n, float p) {
        name = n;
        price = p;
    }

    void displayInfo() const {
        cout << "Main Course: " << name << " - $" << price << endl;
    }
};

class Dessert : public MenuItem {
public:
    Dessert(string n, float p) {
        name = n;
        price = p;
    }

    void displayInfo() const {
        cout << "Dessert: " << name << " - $" << price << endl;
    }
};

// Order class
class OrderItem {
public:
    MenuItem item;
    int quantity;

    OrderItem(MenuItem menuItem, int qty) {
        item = menuItem;
        quantity = qty;
    }

    void displayOrderItem() {
        cout << item.name << " x" << quantity << " - $" << item.price * quantity << endl;
    }
};

class Order {
public:
    int tableNumber;
    vector<OrderItem> items;
    string status;
    float total;

    Order(int tableNum) {
        tableNumber = tableNum;
        status = "Pending";
    }

    void addItemToOrder(const OrderItem& orderItem) {
        items.push_back(orderItem);
    }

    void displayOrder() {
        std::cout << "Order for Table " << tableNumber << " (" << status << "):" << std::endl;
        for (auto& item : items) {
            item.displayOrderItem();
        }
    }

    void completeOrder() {
        status = "Completed";
        std::cout << "Order for Table " << tableNumber << " is now completed." << std::endl;
    }

    void markInPreparation() {
        status = "In Preparation";
        std::cout << "Order for Table " << tableNumber << " is now in preparation." << std::endl;
    }
};

class Restaurant {
public:
    vector<Appetizer> appetizers;
    vector<MainCourse> mainCourses;
    vector<Dessert> desserts;
    vector<Order> orders;
    int tableCount;

    Restaurant(int tableCount) {
        this->tableCount = tableCount;
    }

    void addAppetizer(Appetizer appetizer) {
        appetizers.push_back(appetizer);
    }

    void addMainCourse(MainCourse mainCourse) {
        mainCourses.push_back(mainCourse);
    }

    void addDessert(Dessert dessert) {
        desserts.push_back(dessert);
    }

    void displayMenu() {
        cout << "Menu:" << endl;

        cout << "Appetizers:" << endl;
        for (int i = 0; i < appetizers.size(); i++) {
            cout << (i + 1) << ". ";
            appetizers[i].displayInfo();
        }

        cout << "Main Courses:" << endl;
        for (int i = 0; i < mainCourses.size(); i++) {
            cout << (i + 1 + appetizers.size()) << ". ";
            mainCourses[i].displayInfo();
        }

        cout << "Desserts:" << endl;
        for (int i = 0; i < desserts.size(); i++) {
            cout << (i + 1 + appetizers.size() + mainCourses.size()) << ". ";
            desserts[i].displayInfo();
        }
    }

    void displayOrders() {
        cout << "Orders:" << endl;
        for (auto& order : orders) {
            order.displayOrder();
        }
    }

    void createOrder(int tableNum) {
        Order newOrder(tableNum);
        orders.push_back(newOrder);
        tableCount++;
        cout << "Order created for Table " << tableNum << endl;
    }

    void addOrderItem(int tableNum, MenuItem* menuItem, int qty) {
        for (auto& order : orders) {
            if (order.tableNumber == tableNum) {
                OrderItem orderItem(*menuItem, qty);
                order.addItemToOrder(orderItem);
                cout << "Item added to order for Table " << tableNum << endl;
                return;
            }
        }
        cout << "Table " << tableNum << " does not have an order." << endl;
    }

    void completeOrder(int tableNum) {
        for (auto& order : orders) {
            if (order.tableNumber == tableNum) {
                order.completeOrder();
                return;
            }
        }
        cout << "Table " << tableNum << " does not have an order." << endl;
    }

    void markInPreparation(int tableNum) {
        for (auto& order : orders) {
            if (order.tableNumber == tableNum) {
                order.markInPreparation();
                return;
            }
        }
    }
};

int main() {
    cout << "Hello World!\n";

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

    int tableNumber{};
    bool orderCreated = false;

    while (true) {
        cout << "\nRestaurant Menu:\n";
        cout << "1. Choose a table\n";
        cout << "2. View menu\n";
        cout << "3. Place an order\n";
        cout << "4. Complete an order\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        

        switch (choice) {
        case 1:
            cout << "Enter table number: ";
            cin >> tableNumber;
            restaurant.createOrder(tableNumber);
            orderCreated = true;
            break;

        case 2:
            cout << endl;
            restaurant.displayMenu();
            break;

        case 3:
            if (!orderCreated) {
                cout << "Please choose a table first.\n";
            }
            else {
                cout << endl;
				restaurant.displayMenu();


                int itemChoice, qty;
                cout << "Enter the number of the item you want to order: ";
                cin >> itemChoice;

                cout << "Enter the quantity: ";
                cin >> qty;

                MenuItem* selectedItem = nullptr;
                if (itemChoice >= 1 && itemChoice <= restaurant.appetizers.size()) {
                    selectedItem = &restaurant.appetizers[itemChoice - 1];
                }
                else if (itemChoice > restaurant.appetizers.size() && itemChoice <= restaurant.appetizers.size() + restaurant.mainCourses.size()) {
                    selectedItem = &restaurant.mainCourses[itemChoice - 1 - restaurant.appetizers.size()];
                }
                else if (itemChoice > restaurant.appetizers.size() + restaurant.mainCourses.size() && itemChoice <= restaurant.appetizers.size() + restaurant.mainCourses.size() + restaurant.desserts.size()) {
                    selectedItem = &restaurant.desserts[itemChoice - 1 - restaurant.appetizers.size() - restaurant.mainCourses.size()];
                }

                if (selectedItem) {
                    restaurant.addOrderItem(tableNumber, selectedItem, qty);
                }
                else {
                    cout << "Invalid item choice.\n";
                }
            }
            break;

        case 4:
            if (!orderCreated) {
                cout << "Please choose a table first.\n";
            }
            else {
                restaurant.completeOrder(tableNumber);
                orderCreated = false;
            }
            break;

        case 5:
            cout << "Exiting the system.\n";
            return 0;

        default:
            cout << "Invalid choice. Please select again.\n";
            break;
        }
    }

    return 0;
}

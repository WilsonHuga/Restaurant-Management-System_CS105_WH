#include "Restaurant.h"
#include "MenuItem.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <thread>
#include <ctime>    // For time functions
#include <map>         
#include <iomanip>     // For setw, setfill (formatting)
#include <sstream>     // For stringstream

using namespace std;
vector<MenuItem> specialMenuItems;

bool Restaurant::getActiveOrderForTable(int tableNum, Order& result) {
    return Order::getActiveOrderForTable(tableNum, orders, result);
}

// Load saved order
void Restaurant::loadSavedOrders() {
    ifstream file("kitchen_notifications.txt");
    if (!file) {
        cout << "No saved orders found.\n";
        return;
    }

    orders.clear();
    string line;
    Order* currentOrder = nullptr;
    bool readingOrder = false;

    while (getline(file, line)) {
        if (line.find("Order for Table") != string::npos) {
            if (currentOrder != nullptr) {
                orders.push_back(*currentOrder);
                delete currentOrder;
                currentOrder = nullptr;
            }

            int tableNum = 0;
            string statusStr;

            size_t tablePos = line.find("Table") + 6;
            size_t spacePos = line.find(" ", tablePos);
            if (spacePos == string::npos) {
                spacePos = line.find("(", tablePos);
            }
            string tableNumStr = line.substr(tablePos, spacePos - tablePos);
            tableNum = stoi(tableNumStr);

            size_t statusStart = line.find("(") + 1;
            size_t statusEnd = line.find(")");
            if (statusStart != string::npos && statusEnd != string::npos) {
                statusStr = line.substr(statusStart, statusEnd - statusStart);
            }
            else {
                statusStr = "Pending";
            }

            currentOrder = new Order(tableNum);
            currentOrder->status = statusStr;
            readingOrder = true;
        }
        else if (readingOrder && line.find("Total: $") != string::npos) {
            size_t dollarPos = line.find("$");
            if (dollarPos != string::npos) {
                string totalStr = line.substr(dollarPos + 1);
                try {
                    currentOrder->total = stod(totalStr);
                }
                catch (...) {
                    currentOrder->total = 0.0;
                }
            }
            readingOrder = false;

            if (currentOrder != nullptr) {
                orders.push_back(*currentOrder);
                delete currentOrder;
                currentOrder = nullptr;
            }
        }
        else if (readingOrder && !line.empty() && currentOrder != nullptr) {
            size_t quantityPos = line.find(" x");
            size_t pricePos = line.find(" - $");

            if (quantityPos != string::npos && pricePos != string::npos) {
                string itemName = line.substr(0, quantityPos);

                try {
                    string quantityStr = line.substr(quantityPos + 2, pricePos - quantityPos - 2);
                    int quantity = stoi(quantityStr);

                    string priceStr = line.substr(pricePos + 4);
                    double itemTotal = stod(priceStr);
                    double itemPrice = quantity > 0 ? itemTotal / quantity : 0;

                    MenuItem item(itemName, itemPrice);
                    OrderItem orderItem(item, quantity);
                    currentOrder->items.push_back(orderItem);
                }
                catch (...) {
                    cout << "Warning: Failed to parse order item: " << line << endl;
                }
            }
        }
    }

    if (currentOrder != nullptr) {
        orders.push_back(*currentOrder);
        delete currentOrder;
    }

    file.close();

    updateTableAvailability();

    cout << "Loaded " << orders.size() << " orders from file.\n";
}

void Restaurant::updateTableAvailability() {
    for (int i = 0; i < tableCount; i++) {
        availableTables[i] = true;
    }

    map<int, string> latestOrderStatus;

    for (const auto& order : orders) {
        latestOrderStatus[order.tableNumber] = order.status;
    }

    for (const auto& entry : latestOrderStatus) {
        int tableNum = entry.first;
        const string& status = entry.second;
        if (status != "Completed" && status != "Paid" &&
            tableNum > 0 && tableNum <= tableCount) {
            availableTables[tableNum - 1] = false;
        }
    }
}

void Restaurant::loadMenuFromFile() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "Menu file not found. Starting with empty menu.\n";
        return;
    }

    appetizers.clear();
    mainCourses.clear();
    desserts.clear();
    specialMenuItems.clear();

    string line;
    string type, name;
    double price;

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;

        getline(ss, type, '|');
        getline(ss, name, '|');
        getline(ss, token, '|');
        try {
            price = stod(token);
        }
        catch (...) {
            cout << "Error parsing price for item: " << name << endl;
            continue;
        }

        if (type == "Appetizer") {
            appetizers.push_back(Appetizer(name, price));
        }
        else if (type == "MainCourse") {
            mainCourses.push_back(MainCourse(name, price));
        }
        else if (type == "Dessert") {
            desserts.push_back(Dessert(name, price));
        }
        else if (type == "Special") {
            specialMenuItems.push_back(MenuItem(name, price));
        }
        else {
            cout << "Unknown menu item type: " << type << endl;
        }
    }

    file.close();
    cout << "Menu loaded successfully from file.\n";
}

void Restaurant::saveMenuToFile() {
    ofstream file("menu.txt");
    if (!file) {
        cout << "Error: Cannot open menu.txt for writing.\n";
        return;
    }

    for (const auto& item : appetizers) {
        file << "Appetizer|" << item.getName() << "|" << item.getPrice() << endl;
    }

    for (const auto& item : mainCourses) {
        file << "MainCourse|" << item.getName() << "|" << item.getPrice() << endl;
    }

    for (const auto& item : desserts) {
        file << "Dessert|" << item.getName() << "|" << item.getPrice() << endl;
    }

    for (const auto& item : specialMenuItems) {
        file << "Special|" << item.getName() << "|" << item.getPrice() << endl;
    }

    file.close();
    cout << "Menu saved successfully to file.\n";
}

Restaurant::Restaurant(int tableCount) {
    this->tableCount = tableCount;
    availableTables.resize(tableCount, true);
}

void Restaurant::addAppetizer(Appetizer appetizer) {
    appetizers.push_back(appetizer);
}

void Restaurant::addMainCourse(MainCourse mainCourse) {
    mainCourses.push_back(mainCourse);
}

void Restaurant::addDessert(Dessert dessert) {
    desserts.push_back(dessert);
}

void Restaurant::displayMenu() {
    cout << "Menu:\n";
    cout << "Appetizers:\n";
    for (size_t i = 0; i < appetizers.size(); i++) {
        cout << (i + 1) << ". ";
        appetizers[i].displayInfo();
    }
    cout << "Main Courses:\n";
    for (size_t i = 0; i < mainCourses.size(); i++) {
        cout << (i + 1 + appetizers.size()) << ". ";
        mainCourses[i].displayInfo();
    }
    cout << "Desserts:\n";
    for (size_t i = 0; i < desserts.size(); i++) {
        cout << (i + 1 + appetizers.size() + mainCourses.size()) << ". ";
        desserts[i].displayInfo();
    }
    if (!specialMenuItems.empty()) {
        cout << "Special Menu Items:\n";
        for (size_t i = 0; i < specialMenuItems.size(); i++) {
            cout << (i + 1 + appetizers.size() + mainCourses.size() + desserts.size()) << ". ";
            cout << specialMenuItems[i].getName() << " - $" << specialMenuItems[i].getPrice() << " (Special!)\n";
        }
    }
}

void Restaurant::displayOrders() {
    cout << "Orders:\n";
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }
    for (auto& order : orders) {
        order.displayOrder();
    }
}

void Restaurant::createOrder(int tableNum) {
    Order activeOrder(0);
    if (getActiveOrderForTable(tableNum, activeOrder)) {
        cout << "Table " << tableNum << " already has an active order. Please complete it first.\n";
        return;
    }

    Order newOrder(tableNum);
    orders.push_back(newOrder);
    availableTables[tableNum - 1] = false;
    cout << "Order created for Table " << tableNum << endl;
}

void Restaurant::addOrderItem(int tableNum, MenuItem* menuItem, int qty) {
    for (auto& order : orders) {
        if (order.tableNumber == tableNum &&
            order.status != "Completed" &&
            order.status != "Paid") {
            OrderItem orderItem(*menuItem, qty);
            order.addItemToOrder(orderItem);
            cout << "Item added to order for Table " << tableNum << endl;
            return;
        }
    }
    cout << "Table " << tableNum << " does not have an active order.\n";
}

void Restaurant::completeOrder() {
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }

    vector<int> validIndexes;
    cout << "Orders available to complete:\n";
    int index = 1;
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].status == "Pending" || orders[i].status == "In Preparation") {
            cout << index << ". Table " << orders[i].tableNumber << " (" << orders[i].status << ")\n";
            validIndexes.push_back(static_cast<int>(i));
            index++;
        }
    }

    if (validIndexes.empty()) {
        cout << "No pending or in-preparation orders available.\n";
        return;
    }

    int choice;
    cout << "Enter the number of the order to complete: ";
    cin >> choice;

    if (choice < 1 || choice > validIndexes.size()) {
        cout << "Invalid selection. Please try again.\n";
        return;
    }

    orders[validIndexes[choice - 1]].markAsCompleted();
}

void Restaurant::markInPreparation(int tableNum) {
    for (auto& order : orders) {
        if (order.tableNumber == tableNum) {
            order.markInPreparation();
            writeKitchenNotification(order, "STATUS CHANGE");
            return;
        }
    }
}

void Restaurant::showAvailableTables() {
    cout << "Available Tables: ";
    for (int i = 0; i < tableCount; i++) {
        if (availableTables[i]) {
            cout << (i + 1) << " ";
        }
    }
    cout << endl;
}

void Restaurant::viewCustomerTablesAndBills() {
    if (orders.empty()) {
        cout << "No orders available.\n";
        return;
    }

    for (const auto& order : orders) {
        cout << "Table " << order.getTableNumber() << " has the following order:\n";
        order.displayOrder();
        cout << "Total Bill: $" << order.calculateTotal() << "\n";
        cout << "\n";
    }
}

void Restaurant::addMenuItem() {
    string type, name;
    double price;

    cin.ignore();

    cout << "Enter menu type (appetizer, main, dessert): ";
    getline(cin, type);

    cout << "Enter item name: ";
    getline(cin, name);

    cout << "Enter price: $";
    cin >> price;

    if (type == "appetizer") {
        appetizers.push_back(Appetizer(name, price));
        cout << "Appetizer added successfully.\n";
    }
    else if (type == "main") {
        mainCourses.push_back(MainCourse(name, price));
        cout << "Main Course added successfully.\n";
    }
    else if (type == "dessert") {
        desserts.push_back(Dessert(name, price));
        cout << "Dessert added successfully.\n";
    }
    else {
        cout << "Invalid menu type. Please enter either 'appetizer', 'main', or 'dessert'.\n";
        return;
    }

    saveMenuToFile();
}

void Restaurant::removeMenuItem() {
    string type;

    if (appetizers.empty() && mainCourses.empty() && desserts.empty()) {
        cout << "The menu is currently empty. Nothing to remove.\n";
        return;
    }

    cout << "Select menu type to remove from:\n";
    cout << "1. Appetizer\n";
    cout << "2. Main Course\n";
    cout << "3. Dessert\n";
    cout << "4. Special Menu\n";
    cout << "5. Return to previous menu\n";
    cout << "Enter choice: ";

    int typeChoice;
    cin >> typeChoice;

    switch (typeChoice) {
    case 1:
        type = "appetizer";
        break;
    case 2:
        type = "main";
        break;
    case 3:
        type = "dessert";
        break;
    case 4:
        type = "special";
        break;
    case 5:
        cout << "Operation cancelled.\n";
        return;
    default:
        cout << "Invalid choice. Operation cancelled.\n";
        return;
    }

    if (type == "appetizer") {
        if (appetizers.empty()) {
            cout << "There are no appetizers to remove.\n";
            return;
        }

        cout << "\nAppetizers:\n";
        for (size_t i = 0; i < appetizers.size(); i++) {
            cout << (i + 1) << ". ";
            appetizers[i].displayInfo();
        }

        int index;
        cout << "\nEnter the number of the appetizer to remove (0 to cancel): ";
        cin >> index;

        if (index == 0) {
            cout << "Operation cancelled.\n";
            return;
        }

        if (index < 1 || index > static_cast<int>(appetizers.size())) {
            cout << "Invalid selection. Operation cancelled.\n";
            return;
        }

        string removedItem = appetizers[index - 1].getName();
        appetizers.erase(appetizers.begin() + index - 1);
        cout << "Appetizer '" << removedItem << "' removed successfully.\n";
    }

    else if (type == "main") {
        if (mainCourses.empty()) {
            cout << "There are no main courses to remove.\n";
            return;
        }

        cout << "\nMain Courses:\n";
        for (size_t i = 0; i < mainCourses.size(); i++) {
            cout << (i + 1) << ". ";
            mainCourses[i].displayInfo();
        }

        int index;
        cout << "\nEnter the number of the main course to remove (0 to cancel): ";
        cin >> index;

        if (index == 0) {
            cout << "Operation cancelled.\n";
            return;
        }

        if (index < 1 || index > static_cast<int>(mainCourses.size())) {
            cout << "Invalid selection. Operation cancelled.\n";
            return;
        }

        string removedItem = mainCourses[index - 1].getName();
        mainCourses.erase(mainCourses.begin() + index - 1);
        cout << "Main course '" << removedItem << "' removed successfully.\n";
    }

    else if (type == "dessert") {
        if (desserts.empty()) {
            cout << "There are no desserts to remove.\n";
            return;
        }

        cout << "\nDesserts:\n";
        for (size_t i = 0; i < desserts.size(); i++) {
            cout << (i + 1) << ". ";
            desserts[i].displayInfo();
        }

        int index;
        cout << "\nEnter the number of the dessert to remove (0 to cancel): ";
        cin >> index;

        if (index == 0) {
            cout << "Operation cancelled.\n";
            return;
        }

        if (index < 1 || index > static_cast<int>(desserts.size())) {
            cout << "Invalid selection. Operation cancelled.\n";
            return;
        }

        string removedItem = desserts[index - 1].getName();
        desserts.erase(desserts.begin() + index - 1);
        cout << "Dessert '" << removedItem << "' removed successfully.\n";
    }

    else if (type == "special") {
        if (specialMenuItems.empty()) {
            cout << "There are no special menu items to remove.\n";
            return;
        }

        cout << "\nSpecial Menu Items:\n";
        for (size_t i = 0; i < specialMenuItems.size(); i++) {
            cout << (i + 1) << ". " << specialMenuItems[i].getName()
                << " - $" << specialMenuItems[i].getPrice() << "\n";
        }

        int index;
        cout << "\nEnter the number of the special menu item to remove (0 to cancel): ";
        cin >> index;

        if (index == 0) {
            cout << "Operation cancelled.\n";
            return;
        }

        if (index < 1 || index > static_cast<int>(specialMenuItems.size())) {
            cout << "Invalid selection. Operation cancelled.\n";
            return;
        }

        string removedItem = specialMenuItems[index - 1].getName();
        specialMenuItems.erase(specialMenuItems.begin() + index - 1);
        cout << "Special menu item '" << removedItem << "' removed successfully.\n";
    }

    saveMenuToFile();
}

void Restaurant::addSpecialMenu() {
    string name;
    double price;

    cin.ignore();

    cout << "Enter special menu item name: ";
    getline(cin, name);

    cout << "Enter special item price: $";
    cin >> price;

    specialMenuItems.push_back(MenuItem(name, price));
    cout << "Special menu item added successfully.\n";

    saveMenuToFile();
}

void Restaurant::viewAllBills() {
    cout << "\n===== All Customer Bills =====\n";
    if (orders.empty()) {
        cout << "No orders/bills available to view.\n";
        return;
    }

    double totalRevenue = 0.0;

    cout << left << setw(10) << "Table"
        << setw(15) << "Status"
        << setw(10) << "Amount" << endl;
    cout << string(40, '-') << endl;

    for (const auto& order : orders) {
        cout << left << setw(10) << order.getTableNumber()
            << setw(15) << order.status
            << "$" << fixed << setprecision(2) << order.calculateTotal() << endl;

        if (order.status == "Completed" || order.status == "Paid") {
            totalRevenue += order.calculateTotal();
        }
    }

    cout << string(40, '-') << endl;
    cout << "Total Revenue (Completed/Paid Orders): $" << fixed << setprecision(2) << totalRevenue << endl;
}

void Restaurant::displaySalesAnalytics() {
    int totalOrders = 0;
    int completedOrders = 0;
    int pendingOrders = 0;
    double totalRevenue = 0.0;
    map<int, int> tableFrequency;

    for (const auto& order : orders) {
        totalOrders++;

        if (order.status == "Completed" || order.status == "Paid") {
            completedOrders++;
            totalRevenue += order.calculateTotal();
        }
        else if (order.status == "Pending" || order.status == "In Preparation") {
            pendingOrders++;
        }

        tableFrequency[order.getTableNumber()]++;
    }

    double averageOrderValue = completedOrders > 0 ? totalRevenue / completedOrders : 0;

    int mostPopularTable = 0;
    int maxOrderCount = 0;
    for (const auto& entry : tableFrequency) {
        if (entry.second > maxOrderCount) {
            mostPopularTable = entry.first;
            maxOrderCount = entry.second;
        }
    }

    cout << "SALES ANALYTICS:\n";
    cout << string(40, '-') << endl;
    cout << "Total Orders: " << totalOrders << endl;
    cout << "Completed Orders: " << completedOrders << endl;
    cout << "Pending Orders: " << pendingOrders << endl;
    cout << "Average Order Value: $" << fixed << setprecision(2) << averageOrderValue << endl;

    if (mostPopularTable > 0) {
        cout << "Most Active Table: Table " << mostPopularTable
            << " (" << maxOrderCount << " orders)" << endl;
    }

    cout << "\n========================================\n";
    cout << "          END OF REPORT\n";
    cout << "========================================\n";
}

void Restaurant::generateSalesReport() {
    cout << "\n========================================\n";
    cout << "           SALES REPORT\n";
    cout << "========================================\n\n";

    double totalRevenue = calculateTotalRevenue();
    cout << "TOTAL REVENUE: $" << fixed << setprecision(2) << totalRevenue << "\n\n";

    cout << "BREAKDOWN BY STATUS:\n";
    cout << string(40, '-') << endl;
    map<string, double> statusTotals;
    map<string, int> statusCounts;

    for (const auto& order : orders) {
        statusTotals[order.status] += order.calculateTotal();
        statusCounts[order.status]++;
    }

    for (const auto& entry : statusTotals) {
        cout << left << setw(15) << entry.first
            << setw(10) << statusCounts[entry.first] << " orders"
            << "$" << fixed << setprecision(2) << entry.second << endl;
    }
    cout << endl;

    map<string, double> itemSales = getItemSalesBreakdown();

    cout << "ITEM SALES BREAKDOWN:\n";
    cout << string(45, '-') << endl;
    cout << left << setw(25) << "Item"
        << setw(20) << "Revenue" << endl;
    cout << string(45, '-') << endl;

    vector<pair<string, double>> sortedItems;
    for (const auto& entry : itemSales) {
        sortedItems.push_back(entry);
    }

    sort(sortedItems.begin(), sortedItems.end(),
        [](const pair<string, double>& a, const pair<string, double>& b) {
            return a.second > b.second;
        });

    for (const auto& item : sortedItems) {
        cout << left << setw(25) << item.first
            << "$" << fixed << setprecision(2) << item.second << endl;
    }
    cout << endl;

    displaySalesAnalytics();
}

double Restaurant::calculateTotalRevenue() {
    double total = 0.0;
    for (const auto& order : orders) {
        if (order.status == "Completed" || order.status == "Paid") {
            total += order.calculateTotal();
        }
    }
    return total;
}

map<string, double> Restaurant::getItemSalesBreakdown() {
    map<string, double> itemSales;

    for (const auto& order : orders) {
        if (order.status == "Completed" || order.status == "Paid") {
            for (const auto& item : order.items) {
                itemSales[item.item.name] += item.item.price * item.quantity;
            }
        }
    }

    return itemSales;
}

void Restaurant::writeKitchenNotification(const Order& order, const string& action) {
    auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    tm timeinfo;
    localtime_s(&timeinfo, &now);

    ofstream outFile("kitchen_notifications.txt", ios::app);
    if (!outFile) {
        cerr << "Error: Unable to write kitchen notification.\n";
        return;
    }

    outFile << put_time(&timeinfo, "%H:%M:%S")
        << " | Action: " << action
        << " | Table: " << order.getTableNumber()
        << " | Status: " << order.status << "\n";

    outFile << "  Items:\n";
    for (const auto& item : order.items) {
        outFile << "    - " << item.item.name << " x" << item.quantity << "\n";
    }

    outFile << fixed << setprecision(2);
    outFile << "  Total: $" << order.calculateTotal() << "\n";

    outFile.close();
}

void Restaurant::monitorKitchenNotifications() {
    cout << "===== KITCHEN DISPLAY SYSTEM =====" << endl;
    cout << "Monitoring for new orders and updates..." << endl;
    cout << "Press [Enter] to stop monitoring.\n" << endl;

    ifstream file;
    vector<string> block;
    string line;
    long lastPosition = 0;

    thread inputThread([]() {
        cin.get();
        exit(0);
        });

    while (true) {
        file.open("kitchen_notifications.txt");
        if (file) {
            file.seekg(0, ios::end);
            streamoff currentSize = file.tellg();

            if (currentSize > lastPosition) {
                file.seekg(lastPosition, ios::beg);
                cout << "\n----- New Update -----\n";

                string line;
                block.clear();

                while (getline(file, line)) {
                    if (line.empty()) {
                        if (isValidNotificationBlock(block)) {
                            for (const auto& blockLine : block) {
                                cout << blockLine << endl;
                            }
                        }
                        block.clear();
                    }
                    else {
                        block.push_back(line);
                    }
                }

                if (!block.empty() && isValidNotificationBlock(block)) {
                    for (const auto& blockLine : block) {
                        cout << blockLine << endl;
                    }
                }

                cout << "----------------------\n";

                lastPosition = static_cast<long>(currentSize);
            }

            file.close();
        }
        else {
            cout << "Error opening kitchen_notifications.txt" << endl;
        }

        this_thread::sleep_for(chrono::seconds(2));
    }

    if (inputThread.joinable()) {
        inputThread.join();
    }
}

bool Restaurant::isValidNotificationBlock(const vector<string>& block) {
    if (block.size() < 4) {
        return false;
    }

    const string& header = block[0];
    if (header.length() < 8 || header.substr(2, 1) != ":" || header.substr(5, 1) != ":" ||
        header.find("| Action: ") == string::npos ||
        header.find("| Table: ") == string::npos ||
        header.find("| Status: ") == string::npos) {
        return false;
    }

    if (block[1] != "  Items:") {
        return false;
    }

    bool hasItem = false;
    size_t i = 2;
    for (; i < block.size(); ++i) {
        const string& line = block[i];
        if (line.find("    - ") == 0) {
            hasItem = true;
            size_t xPos = line.find(" x");
            if (xPos == string::npos || xPos + 2 >= line.length()) {
                return false;
            }
            string quantityStr = line.substr(xPos + 2);
            try {
                stoi(quantityStr);
            }
            catch (...) {
                return false;
            }
        }
        else {
            break;
        }
    }

    if (!hasItem) {
        return false;
    }

    const string& totalLine = block[i];
    if (totalLine.find("  Total: $") != 0) {
        return false;
    }

    string totalStr = totalLine.substr(10);
    try {
        stod(totalStr);
    }
    catch (...) {
        return false;
    }

    return true;
}
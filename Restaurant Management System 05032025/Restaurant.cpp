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
    std::ifstream file("kitchen_notifications.txt");// was order 
    if (!file) {
        std::cout << "No saved orders found.\n";
        return;
    }

    orders.clear();
    std::string line;
    Order* currentOrder = nullptr;
    bool readingOrder = false;

    while (getline(file, line)) {
        if (line.find("Order for Table") != std::string::npos) {
            // If we were reading an order, save it before starting a new one
            if (currentOrder != nullptr) {
                orders.push_back(*currentOrder);
                delete currentOrder;
                currentOrder = nullptr;
            }

            // Extract table number and status
            int tableNum = 0;
            std::string statusStr;

            // Parse the table number
            size_t tablePos = line.find("Table") + 6;
            size_t spacePos = line.find(" ", tablePos);
            if (spacePos == std::string::npos) {
                spacePos = line.find("(", tablePos);
            }
            std::string tableNumStr = line.substr(tablePos, spacePos - tablePos);
            tableNum = std::stoi(tableNumStr);

            // Parse the status
            size_t statusStart = line.find("(") + 1;
            size_t statusEnd = line.find(")");
            if (statusStart != std::string::npos && statusEnd != std::string::npos) {
                statusStr = line.substr(statusStart, statusEnd - statusStart);
            }
            else {
                statusStr = "Pending"; // Default status if not specified
            }

            // Create new order
            currentOrder = new Order(tableNum);
            currentOrder->status = statusStr;
            readingOrder = true;
        }
        else if (readingOrder && line.find("Total: $") != std::string::npos) {
            // Extract total
            size_t dollarPos = line.find("$");
            if (dollarPos != std::string::npos) {
                std::string totalStr = line.substr(dollarPos + 1);
                try {
                    currentOrder->total = std::stod(totalStr);
                }
                catch (...) {
                    currentOrder->total = 0.0; // Default if parsing fails
                }
            }
            readingOrder = false;

            // Add the order to our list
            if (currentOrder != nullptr) {
                orders.push_back(*currentOrder);
                delete currentOrder;
                currentOrder = nullptr;
            }
        }
        else if (readingOrder && !line.empty() && currentOrder != nullptr) {
            // Parse order items
            // Format: "Item Name xQ - $P"
            size_t quantityPos = line.find(" x");
            size_t pricePos = line.find(" - $");

            if (quantityPos != std::string::npos && pricePos != std::string::npos) {
                std::string itemName = line.substr(0, quantityPos);

                try {
                    std::string quantityStr = line.substr(quantityPos + 2, pricePos - quantityPos - 2);
                    int quantity = std::stoi(quantityStr);

                    std::string priceStr = line.substr(pricePos + 4);
                    double itemTotal = std::stod(priceStr);
                    double itemPrice = quantity > 0 ? itemTotal / quantity : 0;

                    // Create menu item and add to order
                    MenuItem item(itemName, itemPrice);
                    OrderItem orderItem(item, quantity);
                    currentOrder->items.push_back(orderItem);
                }
                catch (...) {
                    // Skip this item if parsing fails
                    std::cout << "Warning: Failed to parse order item: " << line << std::endl;
                }
            }
        }
    }

    // Add the last order if we haven't yet
    if (currentOrder != nullptr) {
        orders.push_back(*currentOrder);
        delete currentOrder;
    }

    file.close();

    // Update table availability
    updateTableAvailability();

    std::cout << "Loaded " << orders.size() << " orders from file.\n";
}

void Restaurant::updateTableAvailability() {
    // First, mark all tables as available
    for (int i = 0; i < tableCount; i++) {
        availableTables[i] = true;
    }

    // Keep track of the most recent order status for each table
    std::map<int, std::string> latestOrderStatus;

    // Check all orders in memory
    for (const auto& order : orders) {
        // Store the status of this order
        latestOrderStatus[order.tableNumber] = order.status;
    }

    // Now mark tables as unavailable based on the latest status
    for (const auto& entry : latestOrderStatus) {
        int tableNum = entry.first;
        const std::string& status = entry.second;
        if (status != "Completed" && status != "Paid" &&
            tableNum > 0 && tableNum <= tableCount) {
            availableTables[tableNum - 1] = false;
        }
    }
}

// Load menu from menu.txt
void Restaurant::loadMenuFromFile() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "Menu file not found. Starting with empty menu.\n";
        return;
    }

    // Clear existing menu items
    appetizers.clear();
    mainCourses.clear();
    desserts.clear();
    specialMenuItems.clear();

    string line;
    string type, name;
    double price;

    while (getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Parse the line (format: TYPE|NAME|PRICE)
        stringstream ss(line);
        string token;

        // Get type
        getline(ss, type, '|');

        // Get name
        getline(ss, name, '|');

        // Get price
        getline(ss, token, '|');
        try {
            price = stod(token);
        }
        catch (...) {
            cout << "Error parsing price for item: " << name << endl;
            continue;
        }

        // Add to appropriate vector based on type
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

    // Write appetizers
    for (const auto& item : appetizers) {
        file << "Appetizer|" << item.getName() << "|" << item.getPrice() << endl;
    }

    // Write main courses
    for (const auto& item : mainCourses) {
        file << "MainCourse|" << item.getName() << "|" << item.getPrice() << endl;
    }

    // Write desserts
    for (const auto& item : desserts) {
        file << "Dessert|" << item.getName() << "|" << item.getPrice() << endl;
    }

    //Write special menu
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
    // Check if there's already an active order for this table
    Order activeOrder(0);
    if (getActiveOrderForTable(tableNum, activeOrder)) {
        std::cout << "Table " << tableNum << " already has an active order. Please complete it first.\n";
        return;
    }

    // Create a new order since there's no active one
    Order newOrder(tableNum);
    orders.push_back(newOrder);
    availableTables[tableNum - 1] = false;
    cout << "Order created for Table " << tableNum << endl;

}

void Restaurant::addOrderItem(int tableNum, MenuItem* menuItem, int qty) {
    // Find the active order for this table
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
            //07042025
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
        cout << "\n"; // Fixed: Added newline
    }
}



// Manager usage

void Restaurant::addMenuItem() {
    string type, name;
    double price;

    cin.ignore(); // Clear input buffer

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

    // Save menu to file after adding an item
    saveMenuToFile();
}

void Restaurant::removeMenuItem() {
    string type;

    // First, check if there are any menu items to remove
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

    // Display items of the selected type and handle empty categories
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

    else if (type == "special") {  // Add handling for special menu items
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
    // Save menu to file after removing an item
    saveMenuToFile();
}

void Restaurant::addSpecialMenu() {
    string name;
    double price;

    cin.ignore(); // Clear input buffer

    cout << "Enter special menu item name: ";
    getline(cin, name);

    cout << "Enter special item price: $";
    cin >> price;

    specialMenuItems.push_back(MenuItem(name, price));
    cout << "Special menu item added successfully.\n";

    // Save menu to file after adding a special item
    saveMenuToFile();
}

void Restaurant::viewAllBills() {
    cout << "\n===== All Customer Bills =====\n";
    if (orders.empty()) {
        cout << "No orders/bills available to view.\n";
        return;
    }

    double totalRevenue = 0.0;

    cout << left << std::setw(10) << "Table"
        << setw(15) << "Status"
        << setw(10) << "Amount" << endl;
    cout << string(40, '-') << endl;

    for (const auto& order : orders) {
        cout << left << setw(10) << order.getTableNumber()
            << setw(15) << order.status
            << "$" << fixed << setprecision(2) << order.calculateTotal() << endl;

        // Only count completed or paid orders towards revenue
        if (order.status == "Completed" || order.status == "Paid") {
            totalRevenue += order.calculateTotal();
        }
    }

    cout << string(40, '-') << endl;
    cout << "Total Revenue (Completed/Paid Orders): $" << fixed << setprecision(2) << totalRevenue << endl;
}

void Restaurant::displaySalesAnalytics() {
    // Calculate some analytics
    int totalOrders = 0;
    int completedOrders = 0;
    int pendingOrders = 0;
    double totalRevenue = 0.0;
    std::map<int, int> tableFrequency;  // Table number -> count of orders
    
    for (const auto& order : orders) {
        totalOrders++;
        
        if (order.status == "Completed" || order.status == "Paid") {
            completedOrders++;
            totalRevenue += order.calculateTotal();
        } else if (order.status == "Pending" || order.status == "In Preparation") {
            pendingOrders++;
        }
        
        tableFrequency[order.getTableNumber()]++;
    }
    
    double averageOrderValue = completedOrders > 0 ? totalRevenue / completedOrders : 0;
    
    // Find most popular table
    int mostPopularTable = 0;
    int maxOrderCount = 0;
    for (const auto& entry : tableFrequency) {
        if (entry.second > maxOrderCount) {
            mostPopularTable = entry.first;
            maxOrderCount = entry.second;
        }
    }
    
    // Display analytics
    std::cout << "SALES ANALYTICS:\n";
    std::cout << std::string(40, '-') << std::endl;
    std::cout << "Total Orders: " << totalOrders << std::endl;
    std::cout << "Completed Orders: " << completedOrders << std::endl;
    std::cout << "Pending Orders: " << pendingOrders << std::endl;
    std::cout << "Average Order Value: $" << std::fixed << std::setprecision(2) << averageOrderValue << std::endl;
    
    if (mostPopularTable > 0) {
        std::cout << "Most Active Table: Table " << mostPopularTable 
                  << " (" << maxOrderCount << " orders)" << std::endl;
    }
    
    std::cout << "\n========================================\n";
    std::cout << "          END OF REPORT\n";
    std::cout << "========================================\n";
}

void Restaurant::generateSalesReport() {
    // Display header
    std::cout << "\n========================================\n";
    std::cout << "           SALES REPORT\n";
    std::cout << "========================================\n\n";

    // Calculate and display total revenue
    double totalRevenue = calculateTotalRevenue();
    std::cout << "TOTAL REVENUE: $" << std::fixed << std::setprecision(2) << totalRevenue << "\n\n";

    // Display breakdown by order status
    std::cout << "BREAKDOWN BY STATUS:\n";
    std::cout << std::string(40, '-') << std::endl;
    std::map<std::string, double> statusTotals;
    std::map<std::string, int> statusCounts;

    for (const auto& order : orders) {
        statusTotals[order.status] += order.calculateTotal();
        statusCounts[order.status]++;
    }

    for (const auto& entry : statusTotals) {
        std::cout << std::left << std::setw(15) << entry.first
            << std::setw(10) << statusCounts[entry.first] << " orders"
            << "$" << std::fixed << std::setprecision(2) << entry.second << std::endl;
    }
    std::cout << std::endl;

    // Display item sales breakdown
    std::map<std::string, double> itemSales = getItemSalesBreakdown();

    std::cout << "ITEM SALES BREAKDOWN:\n";
    std::cout << std::string(45, '-') << std::endl;
    std::cout << std::left << std::setw(25) << "Item"
        << std::setw(20) << "Revenue" << std::endl;
    std::cout << std::string(45, '-') << std::endl;

    // Sort items by sales amount (highest first)
    std::vector<std::pair<std::string, double>> sortedItems;
    for (const auto& entry : itemSales) {
        sortedItems.push_back(entry);
    }

    std::sort(sortedItems.begin(), sortedItems.end(),
        [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
            return a.second > b.second;
        });

    // Display all items
    for (const auto& item : sortedItems) {
        std::cout << std::left << std::setw(25) << item.first
            << "$" << std::fixed << std::setprecision(2) << item.second << std::endl;
    }
    std::cout << std::endl;

    // Display sales analytics
    displaySalesAnalytics();
}

double Restaurant::calculateTotalRevenue() {
    double total = 0.0;
    for (const auto& order : orders) {
        if (order.status == "Completed" || order.status == "Paid") {
            total += order.calculateTotal();
        }
    }6
    return total;
}

std::map<std::string, double> Restaurant::getItemSalesBreakdown() {
    std::map<std::string, double> itemSales;

    for (const auto& order : orders) {
        if (order.status == "Completed" || order.status == "Paid") {
            for (const auto& item : order.items) {
                itemSales[item.item.name] += item.item.price * item.quantity;
            }
        }
    }

    return itemSales;
}


//07042025
//Write Kitchen Notification
void Restaurant::writeKitchenNotification(const Order& order, const std::string& action) {
    // Get current time
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm timeinfo;
    localtime_s(&timeinfo, &now);

    std::ofstream outFile("kitchen_notifications.txt", std::ios::app);//  kitchen_notifications.txt
    if (!outFile) {
        std::cerr << "Error: Unable to write kitchen notification.\n";
        return;
    }

    // Write header with timestamp and action
    outFile << std::put_time(&timeinfo, "%H:%M:%S")
        << " | Action: " << action
        << " | Table: " << order.getTableNumber()
        << " | Status: " << order.status << "\n";

    // Write order items
    outFile << "  Items:\n";
    for (const auto& item : order.items) {
        outFile << "    - " << item.item.name << " x" << item.quantity << "\n";
    }

    // Write total with fixed precision
    outFile << std::fixed << std::setprecision(2);
    outFile << "  Total: $" << order.calculateTotal() << "\n";

   // outFile << "--------------------------------------------------\n";
}


// Static function to monitor kitchen notifications in a separate console
void Restaurant::monitorKitchenNotifications() {
    std::cout << "===== KITCHEN DISPLAY SYSTEM =====" << std::endl;
    std::cout << "Monitoring for new orders and updates..." << std::endl;
    std::cout << "Press [Enter] to stop monitoring.\n" << std::endl;

    std::ifstream file;
    std::vector<std::string> block;
    std::string line;
    long lastPosition = 0;

    std::thread inputThread([]() {
        std::cin.get(); // Wait for user to press Enter
        std::exit(0);   // Exit program gracefully
        });

    while (true) {
        file.open("kitchen_notifications.txt");//order_details.txt
       // file.open("kitchen_notifications.txt");
        if (file) {
            // Get current file size
            file.seekg(0, ios::end);
            std::streamoff currentSize = file.tellg();

            // If new content is available
            if (currentSize > lastPosition) {
                file.seekg(lastPosition, std::ios::beg);
                std::cout << "\n----- New Update -----\n";

                std::string line;
                block.clear(); // Clear the block for the new content

                // Read the file line by line
                while (std::getline(file, line)) {
                    // If the line is empty, we've reached the end of a block
                    if (line.empty()) {
                        // Check if the block matches the desired format
                        if (isValidNotificationBlock(block)) {
                            // Print the entire block
                            for (const auto& blockLine : block) {
                                std::cout << blockLine << std::endl;
                            }
                        }
                        block.clear(); // Clear for the next block
                    }
                    else {
                        block.push_back(line); // Add line to the current block
                    }
                }

                // Handle the last block if it exists (in case the file doesn't end with a newline)
                if (!block.empty() && isValidNotificationBlock(block)) {
                    for (const auto& blockLine : block) {
                        std::cout << blockLine << std::endl;
                    }
                }

                std::cout << "----------------------\n";

                // Update lastPosition to new end of file
                lastPosition = lastPosition = static_cast<long>(currentSize);
            }

            file.close();
        }
        else {
            std::cout << "Error opening kitchen_notifications.txt" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    // Cleanup
    if (inputThread.joinable()) {
        inputThread.join();
    }
}

//Function to check if a block of lines is a valid

bool Restaurant::isValidNotificationBlock(const vector<string>& block) {
    if (block.size() < 4) {
        return false; //
    }

    const std::string& header = block[0];
    if (header.length() < 8 || header.substr(2, 1) != ":" || header.substr(5, 1) != ":" ||
        header.find("| Action: ") == std::string::npos ||
        header.find("| Table: ") == std::string::npos ||
        header.find("| Status: ") == std::string::npos) {
        return false; // Header doesn't match the expected format
    }

    // Check the second line ("  Items:")
    if (block[1] != "  Items:") {
        return false;
    }

    bool hasItem = false;
    size_t i = 2;
    for (; i < block.size(); ++i) {
        const std::string& line = block[i];
        if (line.find("    - ") == 0) {
            hasItem = true;
            // Validate item format: "    - NAME xQUANTITY"
            size_t xPos = line.find(" x");
            if (xPos == std::string::npos || xPos + 2 >= line.length()) {
                return false;
            }
            std::string quantityStr = line.substr(xPos + 2);
            try {
                std::stoi(quantityStr); // Ensure quantity is a number
            }
            catch (...) {
                return false; // Quantity is not a valid number
            }
        }
        else {
            break; // No more item lines
        }
    }

    if (!hasItem) {
        return false; // No items found
    }

    // Check the last line ("  Total: $...")
    const std::string& totalLine = block[i];
    if (totalLine.find("  Total: $") != 0) {
        return false;
    }

    // Validate the total amount
    std::string totalStr = totalLine.substr(10); // After "  Total: $"
    try {
        std::stod(totalStr); // Ensure total is a valid number
    }
    catch (...) {
        return false; // Total is not a valid number
    }

    return true;
}

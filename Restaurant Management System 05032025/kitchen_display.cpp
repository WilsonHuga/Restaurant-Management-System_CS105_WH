#include <iostream>
#include "Restaurant.h"
#include <fstream>
using namespace std;

int main2() {
    cout << "\n=============================================\n";
    cout << "       KITCHEN DISPLAY SYSTEM\n";
    cout << "=============================================\n";
    cout << "Monitoring for new orders and status changes...\n\n";

    // Use the static method to monitor kitchen notifications
    Restaurant::monitorKitchenNotifications();

    return 0;
}
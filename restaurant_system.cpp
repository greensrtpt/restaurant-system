#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <limits>
using namespace std;

struct MenuItem {
    string allergy;
    double price;
};

map<string, MenuItem> menu; 
map<int, string> itemNumbers;
const string ownerPassword = "1234"; // รหัสผ่านของเจ้าของร้าน

void savemenu() {
    ofstream file("menu.txt");
    if (!file) {
        cout << "Error saving menu!\n";
        return;
    }

    for (const auto& item : menu) {
        file << item.first << "," << item.second.allergy << "," << item.second.price << "\n";
    }

    file.close();
}

void loadmenu() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "No existing menu file found. Starting fresh.\n";
        return;
    }

    menu.clear();
    string line, name, allergy, priceStr;

    while (getline(file, line)) {
        stringstream ss(line);
        if (getline(ss, name, ',') && getline(ss, allergy, ',') && getline(ss, priceStr)) {
            try {
                double price = stod(priceStr);
                menu[name] = {allergy, price};
            } catch (...) {
                cout << "Error loading line: " << line << " (Skipping)\n";
            }
        }
    }

    file.close();
}

void recheckItemNumbers() {
    itemNumbers.clear(); 
    int number = 1;
    for (const auto& item : menu) {
        itemNumbers[number] = item.first;
        number++;
    }
}

void menudisplay() {
    if (menu.empty()) {
        cout << "\nMenu is empty!\n";
    } else {
        cout << "\n------ Menu ------\n";
        int index = 1;
        for (const auto &item : menu) {
            cout << "[" << index << "] " << item.first 
                 << " - Allergy: " << item.second.allergy
                 << " - $" << fixed << setprecision(2) << item.second.price << "\n";
            index++;
        }
        recheckItemNumbers();
    }
}

void searchmenu() {
    string itemName;
    cout << "\nEnter the name of the menu item to search: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
    getline(cin, itemName);

    auto it = menu.find(itemName);
    if (it != menu.end()) {
        cout << "\nItem found: " << it->first 
             << " - Allergy: " << it->second.allergy
             << " - $" << fixed << setprecision(2) << it->second.price << "\n";
    } else {
        cout << "\nItem not found in the menu!\n";
    }
}

void recommendedmenu() {
    if (menu.empty()) {
        cout << "\nNo menu items available for recommendation.\n";
        return;
    }

    cout << "\n---- Recommended Menu ----\n";
    int count = 0;
    for (const auto &item : menu) {
        cout << "- " << item.first 
             << " (Allergy: " << item.second.allergy << ")"
             << " - $" << fixed << setprecision(2) << item.second.price << "\n";
        if (++count >= 3) break; 
    }
}

void orderfood() {
    if (menu.empty()) {
        cout << "\nMenu is empty! Cannot place an order.\n";
        return;
    }

    vector<pair<string, int>> order;
    int itemNumber;
    string itemName;
    int quantity;
    char more;

    menudisplay();
    recheckItemNumbers();

    cin.ignore(); // ใช้แค่ครั้งเดียวก่อนลูป
    do {
        cout << "\nEnter the menu item number or name to order: ";
        string input;
        getline(cin, input);

        try {
            itemNumber = stoi(input);
            if (itemNumbers.find(itemNumber) != itemNumbers.end()) {
                itemName = itemNumbers[itemNumber];
            } else {
                cout << "Invalid item number.\n";
                continue;
            }
        } catch (...) {
            itemName = input;
        }

        if (menu.find(itemName) != menu.end()) {
            cout << "Enter quantity: ";
            cin >> quantity;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            order.push_back({itemName, quantity});
            cout << "Added " << quantity << "x " << itemName 
                 << " (Allergy: " << menu[itemName].allergy << ")"
                 << " to your order.\n";
        } else {
            cout << "Item not found in the menu!\n";
        }

        cout << "Do you want to order more? (y/n): ";
        cin >> more;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ป้องกันการข้ามอินพุต
    } while (more == 'y' || more == 'Y');

    cout << "\n------ Your Order ------\n";
    double total = 0;
    for (const auto &item : order) {
        double price = menu[item.first].price * item.second;
        cout << item.second << "x " << item.first 
             << " - $" << fixed << setprecision(2) << price << "\n";
        total += price;
    }
    cout << "------------------------\n";
    cout << "Total: $" << fixed << setprecision(2) << total << "\n";
}

void customermode() {
    while (true) {
        char choice;
        cout << "\n-----   Customer Menu  -----";
        cout << "\n--- Please Make a Choice ---\n";
        cout << "[1] View Menu\n";
        cout << "[2] Order Food\n"; 
        cout << "[3] Search Menu\n";
        cout << "[4] Recommended Menu\n";
        cout << "[5] Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case '1': menudisplay();
            break;
            case '2': orderfood();
            break;
            case '3': searchmenu();
            break;
            case '4': recommendedmenu();
            break;
            case '5': return;
            default: cout << "Invalid choice! Please try again.\n";
        }
    }
}

void addmenuitem() {
    string itemName, allergy;
    double itemPrice;

    cout << "\nEnter new menu item name: ";
    cin.ignore(); 
    getline(cin, itemName);

    cout << "Enter allergy information (if none, type '-'): ";
    getline(cin, allergy);

    cout << "Enter price for " << itemName << ": ";
    cin >> itemPrice;

    menu[itemName] = {allergy, itemPrice};
    savemenu();
    cout << itemName << " has been added to the menu with a price of $" 
         << itemPrice << " and allergy information: " << allergy << "\n";
}

void removemenuitem() {
    int itemNumber;
    string itemName;
    menudisplay();
    cout << "\nEnter the name OR number of the menu item to remove: ";
    string input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, input);
    recheckItemNumbers();

    try {
        itemNumber = stoi(input); 
        if (itemNumbers.find(itemNumber) != itemNumbers.end()) {
            itemName = itemNumbers[itemNumber];
        } else {
            cout << "Invalid item number.\n";
            return;
        }
    } catch (const std::invalid_argument& e) {
        itemName = input; 
    } catch (const std::out_of_range& e) {
        cout << "Invalid item number (out of range).\n";
        return;
    }
    if (menu.erase(itemName)) { 
        savemenu();
        cout << itemName << " has been removed from the menu.\n";
    } else {
        cout << "Item not found!\n";
    }
}

void changeprice() {
    int itemNumber;
    string itemName;
    double newPrice;
    menudisplay();
    cout << "\nEnter the name OR number of the menu item to change the price: ";

    string input;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    getline(cin, input);

    recheckItemNumbers();

    try {
        itemNumber = stoi(input); 
        if (itemNumbers.find(itemNumber) != itemNumbers.end()) {
            itemName = itemNumbers[itemNumber];
        } else {
            cout << "Invalid item number.\n";
            return;
        }
    } catch (const std::invalid_argument& e) {
        itemName = input; 
    } catch (const std::out_of_range& e) {
        cout << "Invalid item number (out of range).\n";
        return;
    }

    if (menu.find(itemName) != menu.end()) { 
        cout << "Enter new price for " << itemName << ": ";
        cin >> newPrice;         
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        menu[itemName].price = newPrice;  // แก้ไขเฉพาะสมาชิก price
        savemenu();
        cout << fixed << setprecision(2);
        cout << "Price for " << itemName << " has been updated to $" << newPrice << "\n";
    } else {
        cout << "Item not found!\n";
    }
}

void ownermode() {
    string password;
    cout << "Enter Owner Password: ";
    cin >> password;

    if (password != ownerPassword) {
        cout << "Incorrect password, GET OUT!!!\n";
        return;
    }

    while (true) {
        char choice;
        cout << "\n------   Owner Menu   ------";
        cout << "\n--- Please Make a Choice ---\n";
        cout << "[1] Add menu item\n";
        cout << "[2] Remove menu item\n";
        cout << "[3] Change menu price\n";
        cout << "[4] Exit owner mode\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case '1': addmenuitem();
            break;
            case '2': removemenuitem();
            break;
            case '3': changeprice();
            break;
            case '4': return;
            default: cout << "Invalid input! Please try again.\n";
        }
    }
}

int main() {
    loadmenu();
    while (true) {
        char mode;
        cout << "\n---- Restaurant System -----";
        cout << "\n--- Please Make a Choice ---\n";
        cout << "[1] Customer\n";
        cout << "[2] Owner\n";
        cout << "[3] Exit Program\n";
        cout << "Enter choice: ";
        cin >> mode;

        switch (mode) {
            case '1': customermode();
            break;
            case '2': ownermode(); 
            break;
            case '3': cout << "Exiting program...\n"; 
            return 0;
            default: cout << "Invalid input! Please try again.\n";
        }
    }
}

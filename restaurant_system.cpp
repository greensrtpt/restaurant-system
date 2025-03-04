#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <vector>
#include <limits>
#include <string>
#include <random> // ใช้ random_device
#include <chrono> // ใช้สำหรับสุ่ม seed

using namespace std;

struct MenuItem {
    string allergy;
    double price;
    
    MenuItem() : allergy(""), price(0.0) {}
    MenuItem(const string &a, double p) : allergy(a), price(p) {}
};

map<string, MenuItem> menu;         // key: ชื่อเมนู
map<int, string> itemNumbers;         // key: ลำดับที่, value: ชื่อเมนู

const string ownerPassword = "1234";

// ฟังก์ชันบันทึกเมนูลงไฟล์
void saveMenu() {
    ofstream file("menu.txt");
    if (!file) {
        cout << "Error saving menu!" << endl;
        return;
    }

    for (map<string, MenuItem>::const_iterator it = menu.begin(); it != menu.end(); ++it) {
        file << it->first << "," << it->second.allergy << "," << it->second.price << "\n";
    }
}

// ฟังก์ชันโหลดเมนูจากไฟล์
void loadMenu() {
    ifstream file("menu.txt");
    if (!file) {
        cout << "No existing menu file found. Starting fresh." << endl;
        return;
    }
    menu.clear();
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string name, allergy, priceStr;
        if (getline(ss, name, ',') && getline(ss, allergy, ',') && getline(ss, priceStr)) {
            try {
                double price = stod(priceStr);
                // ใช้คอนสตรัคเตอร์แทน initializer list
                menu[name] = MenuItem(allergy, price);
            } catch (...) {
                cout << "Error parsing line: " << line << ". Skipping." << endl;
            }
        }
    }
}

// อัปเดตลำดับหมายเลขของรายการในเมนู
void updateItemNumbers() {
    itemNumbers.clear();
    int index = 1;
    for (map<string, MenuItem>::const_iterator it = menu.begin(); it != menu.end(); ++it) {
        itemNumbers[index++] = it->first;
    }
}

// ฟังก์ชันรับบรรทัดจากผู้ใช้ พร้อมแสดงข้อความ prompt (ถ้ามี)
string getInputLine(const string &prompt = "") {
    if (!prompt.empty())
        cout << prompt;
    string input;
    getline(cin, input);
    return input;
}

// พยายามแปลง string เป็น int
bool tryParseInt(const string &input, int &number) {
    try {
        number = stoi(input);
        return true;
    } catch (...) {
        return false;
    }
}

// รับชื่อรายการเมนูจากอินพุต ซึ่งอาจเป็นตัวเลข (ลำดับ) หรือชื่อรายการ
string getMenuItemNameFromInput() {
    string input = getInputLine();
    int num;
    if (tryParseInt(input, num)) {
        updateItemNumbers();
        if (itemNumbers.find(num) != itemNumbers.end())
            return itemNumbers[num];
        else {
            cout << "Invalid item number." << endl;
            return "";
        }
    }
    return input;
}


// แสดงรายการเมนูทั้งหมด
void displayMenu() {
    if (menu.empty()) {
        cout << "\nMenu is empty!" << endl;
        return;
    }

    // จัดหัวตารางให้อ่านง่ายขึ้น
    cout << "\n" << left << setw(5) << "No." 
         << setw(20) << "Item Name" 
         << setw(15) << "Allergy" 
         << setw(10) << "Price ($)" << endl;
    cout << string(50, '-') << endl; // เส้นคั่น

    int index = 1;
    for (map<string, MenuItem>::iterator it = menu.begin(); it != menu.end(); ++it) {
        cout << left << setw(5) << index++  // หมายเลขเมนู
             << setw(20) << it->first       // ชื่อเมนู
             << setw(15) << it->second.allergy // ข้อมูลแพ้อาหาร
             << "$" << fixed << setprecision(2) << setw(8) << it->second.price // ราคา
             << endl;
    }
    updateItemNumbers();
}

// ค้นหารายการเมนูตามชื่อ
void searchMenu() {
    string name = getInputLine("\nEnter the name of the menu item to search: ");
    map<string, MenuItem>::iterator it = menu.find(name);
    if (it != menu.end()) {
        cout << "Item found: " << name 
             << " - Allergy: " << it->second.allergy 
             << " - $" << fixed << setprecision(2) << it->second.price << endl;
    } else {
        cout << "Item not found in the menu!" << endl;
    }
}

void recommendMenu() {
    if (menu.empty()) {
        cout << "\nNo menu items available for recommendation." << endl;
        return;
    }

    vector<pair<string, MenuItem> > menuList(menu.begin(), menu.end());

    // สุ่ม seed หนึ่งครั้งจาก random_device
    random_device rd;
    mt19937 g(rd());

    // ใช้ shuffle() เพื่อกระจายรายการแบบสุ่ม
    shuffle(menuList.begin(), menuList.end(), g);

    cout << "\n---- Recommended Menu (Try Something New!) ----" << endl;
    for (int i = 0; i < min(3, (int)menuList.size()); i++) {
        cout << "- " << menuList[i].first 
             << " (Allergy: " << menuList[i].second.allergy << ")"
             << " - $" << fixed << setprecision(2) << menuList[i].second.price << endl;
    }
}

// ฟังก์ชันสั่งอาหาร
void orderFood() {
    if (menu.empty()) {
        cout << "\nMenu is empty! Cannot place an order." << endl;
        return;
    }

    // เปลี่ยนจาก initializer list เป็นการใช้ make_pair และเว้นวรรคระหว่าง > >
    vector< pair<string, int> > order;
    displayMenu();
    
    while (true) {
        cout << "\nEnter the menu item number or name to order: ";
        string itemName = getMenuItemNameFromInput();
        if (itemName == "")
            continue;
        if (menu.find(itemName) != menu.end()) {
            cout << "Enter quantity: ";
            int quantity;
            cin >> quantity;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            order.push_back(make_pair(itemName, quantity));
            cout << "Added " << quantity << "x " << itemName << " to your order." << endl;
        } else {
            cout << "Item not found in the menu!" << endl;
        }
        char more;
        cout << "Do you want to order more? (y/n): ";
        cin >> more;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (more != 'y' && more != 'Y')
            break;
    }
    
    cout << "\n------ Your Order ------" << endl;
    double total = 0;
    for (vector< pair<string, int> >::const_iterator it = order.begin(); it != order.end(); ++it) {
        double price = menu[it->first].price * it->second;
        cout << it->second << "x " << it->first 
             << " - $" << fixed << setprecision(2) << price << endl;
        total += price;
    }
    cout << "Total: $" << fixed << setprecision(2) << total << endl;
}

// เมนูสำหรับลูกค้า
void customerMode() {
    while (true) {
        cout << "\n----- Customer Menu -----" << endl;
        cout << "[1] View Menu" << endl;
        cout << "[2] Order Food" << endl;
        cout << "[3] Search Menu" << endl;
        cout << "[4] Recommended Menu" << endl;
        cout << "[5] Back to Main Menu" << endl;
        cout << "Enter choice: ";
        char choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
            case '1': 
                displayMenu(); 
                break;
            case '2': 
                orderFood(); 
                break;
            case '3': 
                searchMenu(); 
                break;
            case '4': 
                recommendMenu(); 
                break;
            case '5': 
                return;
            default: 
                cout << "Invalid choice! Please try again." << endl;
        }
    }
}


// เพิ่มรายการเมนูใหม่
void addMenuItem() {
    string name = getInputLine("\nEnter new menu item name: ");
    string allergy = getInputLine("Enter allergy information (if none, type '-'): ");
    cout << "Enter price for " << name << ": ";
    double price;
    cin >> price;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    menu[name] = MenuItem(allergy, price);
    saveMenu();
    cout << name << " has been added to the menu." << endl;
}

// ลบรายการเมนู
void removeMenuItem() {
    displayMenu();
    cout << "\nEnter the name OR number of the menu item to remove: ";
    string itemName = getMenuItemNameFromInput();
    if (itemName == "")
        return;
    if (menu.erase(itemName)) {
        saveMenu();
        cout << itemName << " has been removed from the menu." << endl;
    } else {
        cout << "Item not found!" << endl;
    }
}

// เปลี่ยนราคาเมนู
void changePrice() {
    displayMenu();
    cout << "\nEnter the name OR number of the menu item to change the price: ";
    string itemName = getMenuItemNameFromInput();
    if (itemName == "")
        return;
    if (menu.find(itemName) != menu.end()) {
        cout << "Enter new price for " << itemName << ": ";
        double newPrice;
        cin >> newPrice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        menu[itemName].price = newPrice;
        saveMenu();
        cout << "Price for " << itemName << " has been updated." << endl;
    } else {
        cout << "Item not found!" << endl;
    }
}

// เมนูสำหรับเจ้าของร้าน (ต้องใส่รหัสผ่าน)
void ownerMode() {
    cout << "Enter Owner Password: ";
    string password;
    cin >> password;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (password != ownerPassword) {
        cout << "Incorrect password, GET OUT!!!" << endl;
        return;
    }
    while (true) {
        cout << "\n------ Owner Menu ------" << endl;
        cout << "[1] Add menu item" << endl;
        cout << "[2] Remove menu item" << endl;
        cout << "[3] Change menu price" << endl;
        cout << "[4] Exit owner mode" << endl;
        cout << "Enter choice: ";
        char choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (choice) {
            case '1': 
                addMenuItem(); 
                break;
            case '2': 
                removeMenuItem(); 
                break;
            case '3': 
                changePrice(); 
                break;
            case '4': 
                return;
            default: 
                cout << "Invalid input! Please try again." << endl;
        }
    }
}

int main() {
    loadMenu();
    while (true) {
        cout << "\n---- Restaurant System ----" << endl;
        cout << "[1] Customer" << endl;
        cout << "[2] Owner" << endl;
        cout << "[3] Exit Program" << endl;
        cout << "Enter choice: ";
        char mode;
        cin >> mode;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (mode) {
            case '1': 
                customerMode(); 
                break;
            case '2': 
                ownerMode(); 
                break;
            case '3': 
                cout << "Exiting program..." << endl; 
                return 0;
            default: 
                cout << "Invalid input! Please try again." << endl;
        }
    }
}

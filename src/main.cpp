#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <map>
#include <unordered_map>
#include <limits>
#include <unistd.h>
#include <iomanip> // For setprecision
#include <windows.h>
using namespace std;

const int screenWidth = 80;  // Assume the screen width is 80 characters

// Colors using Windows Console
enum Color {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    YELLOW = 6,
    WHITE = 7
};

void setColor(Color color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printCentered(const string& text) {
    int padding = (screenWidth - text.length()) / 2;
    if (padding > 0) {
        cout << string(padding, ' ') << text << endl;
    } else {
        cout << text << endl;
    }
}

class User {
public:
    string username;
    string user;
    int pin;
    int pinHash; // Store hashed PIN for security
    double balance;
    vector<string> transactionHistory;

    User() : user(""), pin(0), balance(0.0) {}  // Initialize with default values

    User(string user, int pin, double initialBalance) {
        username = user;
        pinHash = hash<int>()(pin); // Hash the PIN
        balance = initialBalance;
    }

    bool authenticate(int inputPin) {
        return pinHash == hash<int>()(inputPin); // Compare hash values
    }

    void addTransaction(const string& description) {
        time_t now = time(0);
        char* dt = ctime(&now);
        string transactionEntry = description + " at " + string(dt);
        transactionHistory.push_back(transactionEntry);
    }
};

class ATM {
private:
    unordered_map<string, User> users; // Manage multiple users
    map<string, double> exchangeRates;

    void loadUsersFromFile() {
        ifstream file("users.txt");
        if (file.is_open()) {
            string username;
            int pinHash;
            double balance;
            while (file >> username >> pinHash >> balance) {
                users[username] = User(username, pinHash, balance);
            }
            file.close();
        }
    }

    void saveUsersToFile() {
        ofstream file("users.txt");
        if (file.is_open()) {
            for (auto& pair : users) {
                file << pair.second.username << " " << pair.second.pinHash << " " << pair.second.balance << endl;
            }
            file.close();
        }
    }

public:
    ATM() {
        exchangeRates["USD"] = 0.012;
        exchangeRates["EUR"] = 0.011;
        exchangeRates["GBP"] = 0.009;
        loadUsersFromFile();
    }

    ~ATM() {
        saveUsersToFile(); // Save users to file when ATM shuts down
    }

    void createUser(const string& username, int pin, double initialBalance) {
        if (users.find(username) == users.end()) {
            users[username] = User(username, pin, initialBalance);
        }
    }

    User* login(const string& username, int inputPin) {
        if (users.find(username) != users.end() && users[username].authenticate(inputPin)) {
            return &users[username];
        }
        return nullptr;
    }

    void showMenu() {
        setColor(WHITE); // Set color to WHITE
        printCentered("*** Main Menu ***");
        setColor(CYAN); // Reset to cyan
        printCentered("1- Balance Inquiry");
        printCentered("2- Deposit");
        printCentered("3- Withdraw");
        printCentered("4- Transfer Funds");
        printCentered("5- Transaction History");
        printCentered("6- Currency Conversion");
        setColor(YELLOW); //Reset to yellow
        printCentered("8- Exit");
        cout << "Selection: ";
    }

    void clearScreen() {
        system("cls");  // Clear the screen for Windows
    }

    void balanceInquiry(User* user) {
        clearScreen();  // Clear the screen after balance inquiry
        setColor(GREEN); // Set color to green
        printCentered("Your current balance is: PKR " + to_string(user->balance));
        setColor(WHITE); // Reset to white
        user->addTransaction("Balance Inquiry");
        system("pause");
        clearScreen();
    }

    void deposit(User* user, double amount) {
        clearScreen();  // Clear the screen after deposit
        setColor(CYAN); //set color to cyan
        printCentered("Depositing amount...Please wait...");
        sleep(2);
        setColor(GREEN); // Set color to green
        user->balance += amount;
        printCentered("PKR " + to_string(amount) + " deposited successfully.");
        setColor(WHITE); // Reset to white
        user->addTransaction("Deposit: PKR " + to_string(amount));
        system("pause");
        clearScreen();
    }

    void withdraw(User* user, double amount) {
        clearScreen();  // Clear the screen after withdraw
        if (amount > user->balance) {
            setColor(RED); // Set color to red
            printCentered("Insufficient balance!");
            setColor(WHITE); // Reset to white
        } else {
            user->balance -= amount;
            setColor(CYAN); //set color to cyan
            printCentered("Withdrawing amount...Please wait...");
            sleep(2);
            setColor(GREEN); // Set color to green
            printCentered("PKR " + to_string(amount) + " withdrawn successfully.");
            setColor(WHITE); // Reset to white
            user->addTransaction("Withdrawal: PKR " + to_string(amount));
        }
        system("pause");
        clearScreen();
    }

    void transferFunds(User* user, const string& recipientName, double amount) {
        clearScreen();  // Clear the screen after transfer
        if (users.find(recipientName) == users.end()) {
            setColor(RED); // Set color to red
            printCentered("Recipient not found.");
            setColor(WHITE); // Reset to white
        } else if (amount > user->balance) {
            setColor(RED); // Set color to red
            printCentered("Insufficient balance!");
            setColor(WHITE); // Reset to white
        } else {
            user->balance -= amount;
            users[recipientName].balance += amount;
            setColor(CYAN);
            printCentered("Funds Transferring...Please wait...");
            sleep(2);
            setColor(GREEN); // Set color to green
            printCentered("PKR " + to_string(amount) + " transferred to " + recipientName + " successfully.");
            setColor(WHITE); // Reset to white
            user->addTransaction("Transfer to " + recipientName + ": PKR " + to_string(amount));
        }
        system("pause");
        clearScreen();
    }

    void viewTransactionHistory(User* user) {
        clearScreen();  // Clear the screen after transaction history
        if (user->transactionHistory.empty()) {
            setColor(YELLOW); // Set color to yellow
            printCentered("No transactions yet.");
            setColor(WHITE); // Reset to white
        } else {
            setColor(CYAN); // Set color to cyan
            printCentered("*** Transaction History ***");
            setColor(WHITE); // Reset to white
            for (const string& entry : user->transactionHistory) {
                printCentered(entry);
            }
        }
        sleep(3);  // Wait 5 seconds before returning to the main menu
        system("pause");
        clearScreen();
    }

    void currencyConversion(User* user, const string& currency) {
        clearScreen();  // Clear the screen after currency conversion
        if (exchangeRates.find(currency) != exchangeRates.end()) {
            double convertedAmount = user->balance * exchangeRates[currency];
            setColor(GREEN); // Set color to green
            printCentered("Your balance in " + currency + " is: " + to_string(convertedAmount) + " " + currency);
            setColor(WHITE); // Reset to white
            system("pause");
            user->addTransaction("Currency Conversion to " + currency);
        } else {
            setColor(RED); // Set color to red
            printCentered("Invalid currency.");
            setColor(WHITE); // Reset to white
            system("pause");
        }
        sleep(3);  // Wait 3 seconds before returning to the main menu
        clearScreen();
    }
};

int main() {
    ATM atm;
    
    atm.createUser("Waseem", 1234, 1000.00); // Initial user for demo
    atm.createUser("Anus", 4321, 550.00);
    atm.createUser("Ali", 1122, 1150.00);
    atm.createUser("Salar", 1515, 900.00);

    string inputUser;
    int inputPin;
    int attemptCount = 0;
    const int maxAttempts = 3;  // Max login attempts

    setColor(CYAN); // Set color to cyan
    printCentered("*** Welcome to the IWY ATM Application ***");
    setColor(WHITE); // Reset to white

    User* user = nullptr;
    // User login
    while (attemptCount < maxAttempts) {
        cout << "Enter your name: ";
        cin >> inputUser;
        cout << "Enter your PIN: ";
        cin >> inputPin;

        user = atm.login(inputUser, inputPin);

        if (user == nullptr) {
            attemptCount++;
            atm.clearScreen();  // Clear screen after failed attempt
            if (attemptCount < maxAttempts) {
                setColor(RED); // Set color to red
                printCentered("Authentication failed. Please try again.");
                setColor(WHITE); // Reset to white
            } else {
                setColor(RED); // Set color to red
                printCentered("Authentication failed 3 times.");
                printCentered("Please visit the nearest bank for assistance.");
                setColor(WHITE); // Reset to white
                return 0;  // Exit the program after 3 failed attempts
            }
        } else {
            atm.clearScreen();  // Clear screen after successful authentication
            setColor(GREEN); // Set color to green
            printCentered("Authentication granted.");
            setColor(WHITE); // Reset to white
            sleep(1);  // Wait for a second before proceeding
            break;     // Exit the loop if authentication is successful
        }
    }

    int choice;
    while (true) {
        atm.clearScreen();  // Clear the screen after every menu display
        atm.showMenu();

        // Ensure the input is numeric
        if (!(cin >> choice)) {
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore invalid input
            setColor(RED); // Set color to red
            printCentered("Invalid choice, please try again.");
            setColor(WHITE); // Reset to white
            sleep(2); // Pause for 2 seconds before showing the menu again
            continue; // Skip the rest of the loop and show the menu again
        }

        // Execute action based on choice
        if (choice == 1) {
            atm.balanceInquiry(user);
        } else if (choice == 2) {
            double depositAmount;
            cout << "Enter deposit amount: ";
            cin >> depositAmount;
            atm.deposit(user, depositAmount);
        } else if (choice == 3) {
            double withdrawAmount;
            cout << "Enter withdrawal amount: ";
            cin >> withdrawAmount;
            atm.withdraw(user, withdrawAmount);
        } else if (choice == 4) {
            string recipient;
            double transferAmount;
            cout << "Enter recipient's name: ";
            cin >> recipient;
            cout << "Enter amount to transfer: ";
            cin >> transferAmount;
            atm.transferFunds(user, recipient, transferAmount);
        } else if (choice == 5) {
            atm.viewTransactionHistory(user);
        } else if (choice == 6) {
            string currency;
            cout << "Enter currency (e.g., USD, EUR, GBP): ";
            cin >> currency;
            atm.currencyConversion(user, currency);
        } else if (choice == 8) {
            atm.clearScreen();  // Clear the screen before exiting
            setColor(CYAN); // Set color to cyan
            printCentered("Exiting...");
            setColor(WHITE); // Reset to white
            sleep(2);
            printCentered("Thank you for using the ATM.");
            break;
        } else {
            setColor(RED); // Set color to red
            printCentered("Invalid choice, please try again.");
            setColor(WHITE); // Reset to white
            sleep(2); // Pause for 2 seconds before showing the menu again
        }
    }
system("pause");
    return 0;
}

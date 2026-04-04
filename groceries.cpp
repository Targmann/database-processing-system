//
//  main.cpp
//  groceries
//
//  Created by Jacob Steimle on 3/19/26.
//
//  Run code:
//  clang++ groceries.cpp -o groceries && ./groceries
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <list>
#include "split.h"

using namespace std;

// Customer struct
struct Customer {
    int ID;
    string name;
    string address;
    string city;
    string state;
    string zip_code;
    string phone_num;
    string email;
};
// Read customer file into customer data
vector<Customer> customer_data;
void read_customers(string input_file) {
    // Open file
    ifstream customer_info(input_file);
    if (!customer_info.is_open()) {
        cerr << "Unable to open file" << endl;
        return;
    }
    // Put customer records into global vector
    string line;
    while (getline(customer_info, line)) {
        vector<string> data = split(line, ',');
        Customer c;
        c.ID = stoi(data[0]);
        c.name = data[1];
        c.address = data[2];
        c.city = data[3];
        c.state = data[4];
        c.zip_code = data[5];
        c.phone_num = data[6];
        c.email = data[7];
        customer_data.push_back(c);
    }
    // Close file
    customer_info.close();
}

// Item struct
struct Item {
    int ID;
    string description;
    double price;
};
// Read item file into item data
vector<Item> item_data;
void read_items(string input_file) {
    // Open file
    ifstream item_info(input_file);
    if (!item_info.is_open()) {
        cerr << "Unable to open file(s)" << endl;
        return;
    }
    // Put item records into global vector
    string line;
    while (getline(item_info, line)) {
        vector<string> data = split(line, ',');
        Item i;
        i.ID = stoi(data[0]);
        i.description = data[1];
        i.price = stod(data[2]);
        item_data.push_back(i);
    }
    // Close file
    item_info.close();
}

// LineItem class
class LineItem {
private:
    int item_id;
    int qty;
public:
    LineItem(int id, int q) {
        item_id = id;
        qty = q;
    }
    double sub_total() const {
        double total_price = 0;
        for (const Item& i : item_data) {
            if (i.ID == item_id) {
                total_price += i.price * qty;
            }
        }
        return total_price;
    }
    string print_item_info() const {
        ostringstream item_info;
        for (const Item& i : item_data) {
            if (i.ID == item_id) {
                item_info << "Item " << i.ID << ": \""<< i.description << "\", " << qty << " @ " << fixed << setprecision(2) << i.price << "\n";
            }
        }
        return item_info.str();
    }
    friend bool operator<(const LineItem&, const LineItem&);
};
bool operator<(const LineItem& item1, const LineItem& item2) {
    return item1.item_id < item2.item_id;
}

// Payment class
class Payment {
protected:
    double amount;
public:
    virtual string print_detail() const = 0;
    void set_amount(double amt) {
        amount = amt;
    }
};
// Credit card
class Credit : public Payment {
private:
    string card_number;
    string expiration;
public:
    Credit(string card_num, string exp) {
        card_number = card_num;
        expiration = exp;
    }
    string print_detail() const override {
        ostringstream details;
        details << "Amount: $" << amount << ", Paid by Credit card " << card_number << ", exp. " << expiration;
        return details.str();
    }
};
// PayPal
class PayPal : public Payment {
private:
    string paypal_id;
public:
    PayPal(string id) {
        paypal_id = id;
    }
    string print_detail() const override {
        ostringstream details;
        details << "Amount: $" << amount << ", Paid by Paypal ID: " << paypal_id;
        return details.str();
    }
};
// Wire transfer
class WireTransfer : public Payment {
private:
    string bank_id;
    string account_id;
public:
    WireTransfer(string bank, string account) {
        bank_id = bank;
        account_id = account;
    }
    string print_detail() const override {
        ostringstream details;
        details << "Amount: $" << amount << ", Paid by Wire transfer from Bank ID " << bank_id << ", Account# " << account_id;
        return details.str();
    }
};

// Order class
class Order {
private:
    int order_id;
    string order_date;
    int cust_id;
    vector<LineItem> line_items;
    Payment* payment;
public:
    Order(int id, string date, int cust) {
        order_id = id;
        order_date = date;
        cust_id = cust;
    }
    // Calculate total price
    double total() const {
        double total_price = 0;
        for (const LineItem& item : line_items) {
            total_price += item.sub_total();
        }
        return total_price;
    }
    // Set payment method
    void set_payment_method(Payment* pay_method) {
        // Set new payment information
        payment = pay_method;
    }
    // Print the order
    string print_order() const {
        ostringstream receipt;
        // Create pointer to customer to access attributes
        Customer* cust = nullptr;
        for (auto& c : customer_data) {
            if (c.ID == cust_id) {
                cust = &c;
            }
        }
        // Set the payment amount
        payment->set_amount(total());
        // Construct the formatted string to print
        receipt << "===========================\n";
        receipt << "Order #" << order_id << ", Date: " << order_date << "\n";
        receipt << payment->print_detail() << "\n\n";
        receipt << "Customer ID #" << cust_id << ":\n";
        receipt << cust->name << ", ph. " << cust->phone_num << ", email: " << cust->email << "\n" << cust->address << "\n" << cust->city << ", " << cust->state << " " << cust->zip_code << "\n\n";
        receipt << "Order Detail:\n";
        for (const LineItem& i : line_items) {
            receipt << "       " << i.print_item_info();
        }
        return receipt.str();
    }
    // Add a line item
    void add_line_item(const LineItem& item) {
        line_items.push_back(item);
    }
    // Sort the items
    void sort_line_item() {
        sort(line_items.begin(), line_items.end());
    }
};
list<Order> orders;

// Ordering function
void one_customer_order() {
    // Print totals
    cout << "Number of customers: " << customer_data.size() << endl;
    cout << "Number of items: " << item_data.size() << endl;
    // Get customer ID from user
    cout << "Enter a customer ID: ";
    int customerID;
    cin >> customerID;
    // Whether customer and item are found
    bool customer_found = false;
    bool item_found;
    // Running variables
    int num_items = 0;
    double total_price = 0;
    for (size_t i = 0; i < customer_data.size(); ++i) {
        // Check for valid customer ID
        if (customer_data.at(i).ID == customerID) {
            customer_found = true;
            int item_num = -1;
            while (item_num != 0) {
                // Get item number from user
                item_found = false;
                cout << "Enter an item number: ";
                cin >> item_num;
                for (size_t j = 0; j < item_data.size(); ++j) {
                    // Check for valid item number
                    if (item_data.at(j).ID == item_num) {
                        item_found = true;
                        cout << "Name: " << split(item_data.at(j).description, '-')[0] << endl;
                        cout << "Description:" << split(item_data.at(j).description, '-')[1] << endl;
                        double price = item_data.at(j).price;
                        cout << "Price: " << price << endl;
                        num_items++;
                        total_price += price;
                    }
                }
                if (!item_found && item_num != 0) {
                    cout << "Item not found" << endl;
                }
            }
        }
    }
    // Print information if customer isn't found
    if (!customer_found) {
        cout << "Customer ID not found" << endl;
        return;
    }
    // Print receipt info
    cout << "Number of items purchased: " << num_items << "\n";
    cout << "Total price: $" << total_price << endl;
}

// Read orders
void read_orders(string input_file) {
    // Open file
    ifstream orders_info(input_file);
    if (!orders_info.is_open()) {
        cerr << "Unable to open file" << endl;
        return;
    }
    // Put order into global list
    string line;
    while (getline(orders_info, line)) {
        vector<string> data = split(line, ',');
        int cust_id = stoi(data[0]);
        int order_id = stoi(data[1]);
        string date = data[2];
        Order order(order_id, date, cust_id);
        // Get the item and quantity info
        for (size_t i = 3; i < data.size(); ++i) {
            vector<string> line_data = split(data[i], '-');
            int item_id = stoi(line_data[0]);
            int qty = stoi(line_data[1]);
            LineItem item(item_id, qty);
            order.add_line_item(item);
        }
        // Sort
        order.sort_line_item();
        // Set payment info
        getline(orders_info, line);
        vector<string> payment_data = split(line, ',');
        Payment* payment = nullptr;
        switch(stoi(payment_data[0])) {
            case 1:
                payment = new Credit(payment_data[1], payment_data[2]);
                break;
            case 2:
                payment = new PayPal(payment_data[1]);
                break;
            case 3:
                payment = new WireTransfer(payment_data[1], payment_data[2]);
                break;
        }
        order.set_payment_method(payment);
        // Add the order
        orders.push_back(order);
    }
    // Close file
    orders_info.close();
}

int main() {
    read_customers("customers.txt");
    read_items("items.txt");
    read_orders("orders.txt");
    
    //one_customer_order();
    ofstream ofs("order_report.txt");
    for (const auto& order: orders) {
        ofs << order.print_order() << endl;
    }
    return 0;
}

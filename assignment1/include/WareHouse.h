#pragma once
#include <string>
#include <vector>
using namespace std;

#include "../include/Order.h"
#include "../include/Customer.h"
#include "../include/Volunteer.h"
#include "../include/Action.h"

class Action;
class Volunteer;

// Warehouse responsible for Volunteers, Customers Actions, and Orders.
class WareHouse {

    public:
        WareHouse(const string &configFilePath);
        void start();
        const vector<Action*> & getActions() const;
        void addOrder(Order* order);//לא לשכוח להגדיל את הקאונטר של האורדר וגם להכניס את ההזמנה לרשימה של הפנדינג 
        void addAction(Action* action);
        Customer &getCustomer(int customerId) const;
        Volunteer &getVolunteer(int volunteerId) const;
        Order &getOrder(int orderId) const;
        void close();//is open = false and freeing all the memory
        void open();
        const vector<Order*>& getPendingOrders() const;//auxiliary function
        const vector<Order*>& getInProcessOrders() const;//auxiliary function
        const vector<Order*>& getCompletedOrders() const;//auxiliary function
        const vector<Volunteer*>& getVolunteersVector() const;//auxiliary function
        const vector<Customer*>& getCustomerVector() const;//auxiliary function
        void moveOrder(std::string vector);//auxiliary function - move to the next vector 
        int getOrderCounter() const;
        int getCustomerCounter() const;
        int getVolunteerCounter() const;
        void addCustomer(Customer* customer);//increase the index of the counter and add to customers vector
        void deleteVolunteers();
        WareHouse* clone() const;
        //rule of 5
        WareHouse(const WareHouse& other);
        WareHouse& operator = (const WareHouse& other);
        virtual ~WareHouse();
        WareHouse(WareHouse&& other);
        WareHouse& operator = (WareHouse&& other);
    private:
        bool isOpen;
        vector<Action*> actionsLog;
        vector<Volunteer*> volunteers;
        vector<Order*> pendingOrders;
        vector<Order*> inProcessOrders;
        vector<Order*> completedOrders;
        vector<Customer*> customers;
        int customerCounter; //For assigning unique customer IDs
        int volunteerCounter; //For assigning unique volunteer IDs
        int orderCounter;//For assigning unique order IDs
        SoldierCustomer* nullCustomer;
        CollectorVolunteer* nullVolunteer;
        Order* nullOrder;
};
extern WareHouse* backup;
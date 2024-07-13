#include "../include/WareHouse.h"
#include <iostream>
#include <sstream>
#include <fstream>
WareHouse* backup = nullptr;

WareHouse::WareHouse (const string &configFilePath):isOpen(false), actionsLog(), volunteers(), pendingOrders(), 
inProcessOrders(), completedOrders(), customers(), customerCounter(0), volunteerCounter(0), orderCounter(0), nullCustomer(new SoldierCustomer(-1,"nf",-1,-1)),
nullVolunteer(new CollectorVolunteer(-1,"nf", -1)),nullOrder(new Order(-1,-1,-1)){
    // Open the configuration file
    std::ifstream configFile(configFilePath);
    if (configFile.is_open()) {
        // Process each line in the configuration file
        std::string line;
        while (std::getline(configFile, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // Parse each line using a string stream
            std::istringstream iss(line);
            std::string type;
            iss >> type;

            // Depending on the type, create Customer or Volunteer objects
            if (type == "customer") {
                // Parse customer data and create Customer object
                string name, customerType;
                int distance, maxOrders;
                iss >> name >> customerType >> distance >> maxOrders;
                if(customerType == "civilian")
                    customers.push_back(new CivilianCustomer(customerCounter, name, distance, maxOrders));
                else
                    customers.push_back(new SoldierCustomer(customerCounter, name, distance, maxOrders));
                customerCounter++;
            } else if (type == "volunteer") {
                // Parse volunteer data and create Volunteer object
                std::string name, role;
                int coolDown, maxDistance, distancePerStep, maxOrders;
                iss >> name >> role;
                if (role == "driver"){
                    iss >> maxDistance >> distancePerStep;
                    volunteers.push_back(new DriverVolunteer(volunteerCounter, name, maxDistance, distancePerStep));
                }
                if(role == "limited_driver"){
                    iss >> maxDistance >> distancePerStep >> maxOrders;
                    volunteers.push_back(new LimitedDriverVolunteer(volunteerCounter, name, maxDistance, distancePerStep, maxOrders));
                }
                if (role == "collector"){
                    iss >> coolDown;
                    volunteers.push_back(new CollectorVolunteer(volunteerCounter, name, coolDown));
                }
                if (role == "limited_collector"){
                    iss >> coolDown >> maxOrders;
                    volunteers.push_back(new LimitedCollectorVolunteer(volunteerCounter, name, coolDown, maxOrders));
                }
                volunteerCounter++;
            }
        }
        configFile.close();
    }
    
    else{
        std::cerr << "Error: Unable to open configuration file." << std::endl;
    }
        
}

void WareHouse:: start(){
    open();
    cout << "WareHouse is Open!" << endl;
    while(isOpen){
        string input;
        string temp;
        int num;
        string customerName;
        string customerType;
        int distance;
        int maxOrders;
        // Get user input
        std::cout << "Enter your command: ";
        std::getline(std::cin, input);
        std::istringstream iss(input);
        iss >> temp;
        if(temp == "step"){
            iss >> num;
            SimulateStep* step = new SimulateStep(num);
            step->act(*this);
        }
        else if(temp == "order"){
            iss >> num;
            AddOrder* order = new AddOrder(num);    
            order->act(*this);
        }
        else if(temp == "customer"){
            iss >> customerName >> customerType >> distance >> maxOrders;
            AddCustomer* customer = new AddCustomer(customerName,customerType,distance,maxOrders);
            customer->act(*this);
        }
        else if(temp == "orderStatus"){
            iss >> num;
            PrintOrderStatus* orderStat = new PrintOrderStatus(num);
            orderStat->act(*this);
        }
        else if(temp == "customerStatus"){
            iss >> num;
            PrintCustomerStatus* customerStat = new PrintCustomerStatus(num);
            customerStat->act(*this);
        }
        else if(temp == "volunteerStatus"){
            iss >> num;
            PrintVolunteerStatus* volunteerStat = new PrintVolunteerStatus(num);
            volunteerStat->act(*this);
        }
        else if(temp == "log"){
            PrintActionsLog* actions = new PrintActionsLog(); //לבדוק האם צריך להגדיר על הערימה או על המחסנית כי לא מוסיפים לוקטור
            actions->act(*this);
        }
        else if(temp == "close"){
            Close* c = new Close();
            c->act(*this);
        }
        else if(temp == "backup"){  
            BackupWareHouse* backup = new BackupWareHouse();
            backup->act(*this); 
        }            
        else if(temp == "restore"){  
            RestoreWareHouse* restore = new RestoreWareHouse();
            restore->act(*this); 
        }  
        else{
            std::cout << "Invalid command" << std::endl; 
        }
    }
}

Customer & WareHouse:: getCustomer(int customerId) const{
    for (Customer* c: customers){
        if(c->getId() == customerId)
            return *c;
    }
    return *nullCustomer;
}

Volunteer & WareHouse:: getVolunteer(int volunteerId) const{
    for (Volunteer* v: volunteers){
        if(v->getId() == volunteerId) //problem
            return *v;
    }
    return *nullVolunteer;
}

Order & WareHouse:: getOrder(int orderId) const{
    for (Order* o: pendingOrders){
        if(o->getId() == orderId)
            return *o;
    }
    for (Order* o: inProcessOrders){
        if(o->getId() == orderId)
            return *o;
    }
    for (Order* o: completedOrders){
        if(o->getId() == orderId)
            return *o;
    }
    return *nullOrder;
}
int WareHouse:: getOrderCounter() const{return orderCounter;}
int WareHouse:: getCustomerCounter() const{return customerCounter;}
int WareHouse:: getVolunteerCounter() const{return volunteerCounter;}
const vector<Action*> & WareHouse:: getActions() const{return actionsLog;}
const vector<Order*>& WareHouse:: getPendingOrders() const{return pendingOrders;}//auxiliary function
const vector<Order*>& WareHouse:: getInProcessOrders() const{return inProcessOrders;}//auxiliary function
const vector<Order*>& WareHouse:: getCompletedOrders() const{return completedOrders;}//auxiliary function
const vector<Volunteer*>& WareHouse:: getVolunteersVector() const{return volunteers;}//auxiliary function
const vector<Customer*>& WareHouse:: getCustomerVector() const{return customers;}//auxiliary function
void WareHouse:: addOrder(Order* order){
    orderCounter ++;
    pendingOrders.push_back(order);
}//לא לשכוח להגדיל את הקאונטר של האורדר וגם להכניס את ההזמנה לרשימה של הפנדינג 
void WareHouse:: addAction(Action* action){actionsLog.push_back(action);}
void WareHouse:: close(){//להשלים את הפונקציה
    isOpen = false;
}
void WareHouse:: open(){isOpen = true;}

void WareHouse:: moveOrder(std::string vector){
    
    std::vector<Order*> vectorToReplace;
    if(vector == "Pending"){
        for (Order* o : pendingOrders) {
            if (!o->getNeedToMove()) 
                vectorToReplace.push_back(o);            
            else{
                inProcessOrders.push_back(o);
                o->setNeedToMove();    
            }
        }
        pendingOrders = vectorToReplace;
    }
    else if (vector == "inProcess"){
        for (Order* o : inProcessOrders) {
            if (!o->getNeedToMove()) 
                vectorToReplace.push_back(o);            
            else if (o->getStatus() == OrderStatus::COLLECTING){
                pendingOrders.push_back(o);
                o->setNeedToMove();    
            }
            else{ //OrderStatus is Delivering
                completedOrders.push_back(o);
                o->setNeedToMove(); 
                o->setStatus(OrderStatus::COMPLETED);
            }
        }
        inProcessOrders = vectorToReplace;
    }
    
}//auxiliary function - move to the next vector 
void WareHouse:: addCustomer(Customer* customer){
    customers.push_back(customer);
    customerCounter ++;
}//increase the index of the counter and add to customers vector
void WareHouse:: deleteVolunteers(){
    for (auto it = volunteers.begin(); it != volunteers.end();) {
            if (!(*it)->hasOrdersLeft() && !(*it)->isBusy()) {
                delete *it;
                it = volunteers.erase(it);
            }
            else 
                ++it;
    }
}
WareHouse* WareHouse:: clone() const{return new WareHouse(*this);}
//rule of 5
WareHouse& WareHouse:: operator = (const WareHouse& other){
    if(&other != this){
        isOpen = other.isOpen;
        for(Action* a: actionsLog)
            delete a;
        actionsLog.clear();
        for(Action* a: other.actionsLog)
            actionsLog.push_back(a->clone());
        for(Volunteer* v: volunteers)
            delete v;
        volunteers.clear();
        for(Volunteer* v: other.volunteers)
            volunteers.push_back(v->clone());
        for(Order* o : pendingOrders)
            delete o;
        pendingOrders.clear();
        for(Order* o : other.pendingOrders)
            pendingOrders.push_back(o->clone());
        for(Order* o: inProcessOrders)
            delete o;
        inProcessOrders.clear();
        for(Order* o: other.inProcessOrders)
            inProcessOrders.push_back(o->clone());
        for(Order* o: completedOrders)
            delete o;
        completedOrders.clear();
        for(Order *o: other.completedOrders)
            completedOrders.push_back(o->clone());
        for(Customer* c: customers)
            delete c;
        customers.clear();
        for(Customer* c: other.customers)
            customers.push_back(c->clone());  
        
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        //nullCustomer, Volunteer, Order is identitcal in all WareHouses - no need to copy                                                    
    }
    return *this;
}

WareHouse:: ~WareHouse(){
    for(Action* a: actionsLog)
        delete a;
    actionsLog.clear();
    for(Volunteer* v: volunteers)
        delete v;
    volunteers.clear();
    for(Order* o : pendingOrders)
        delete o;
    pendingOrders.clear();
    for(Order* o: inProcessOrders)
        delete o;
    inProcessOrders.clear();
    for(Order* o: completedOrders)
        delete o;
    completedOrders.clear();
    for(Customer* c: customers)
        delete c;
    customers.clear();
    delete nullCustomer;
    delete nullVolunteer;
    delete nullOrder;
}

WareHouse::WareHouse(const WareHouse& other):isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(),
inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter),
    volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter), nullCustomer(other.nullCustomer->clone()),
    nullVolunteer(other.nullVolunteer->clone()), nullOrder(other.nullOrder->clone()) {
        for(Action* a: other.actionsLog)
            actionsLog.push_back(a->clone());
        for(Volunteer* v: other.volunteers)
            volunteers.push_back(v->clone());    
        for(Order* o : other.pendingOrders)
            pendingOrders.push_back(o->clone());
        for(Order* o: other.inProcessOrders)
            inProcessOrders.push_back(o->clone());
        for(Order *o: other.completedOrders)
            completedOrders.push_back(o->clone());    
        for(Customer* c: other.customers)
            customers.push_back(c->clone()); 
}

WareHouse:: WareHouse(WareHouse&& other) :isOpen(other.isOpen), actionsLog(), volunteers(), pendingOrders(),
inProcessOrders(), completedOrders(), customers(), customerCounter(other.customerCounter),
    volunteerCounter(other.volunteerCounter), orderCounter(other.orderCounter), nullCustomer(other.nullCustomer->clone()),
    nullVolunteer(other.nullVolunteer->clone()), nullOrder(other.nullOrder->clone()){
        for(Action* a: other.actionsLog){
            actionsLog.push_back(a->clone());
            a = nullptr;
        }
        other.actionsLog.clear();
        for(Volunteer* v: other.volunteers){
            volunteers.push_back(v->clone());
            v = nullptr;
        }
        other.volunteers.clear();        
        for(Order* o : other.pendingOrders){
            pendingOrders.push_back(o->clone());
            o = nullptr;
        }
        other.pendingOrders.clear();    
        for(Order* o: other.inProcessOrders){
            inProcessOrders.push_back(o->clone());
            o = nullptr;
        }
        other.inProcessOrders.clear();    
        for(Order *o: other.completedOrders){
            completedOrders.push_back(o->clone());
            o = nullptr;
        }
        other.completedOrders.clear();        
        for(Customer* c: other.customers){
            customers.push_back(c->clone());
            c = nullptr;
        }
        other.completedOrders.clear();
        other.isOpen = false;
        other.customerCounter = 0;
        other.volunteerCounter = 0;
        other.orderCounter = 0;
        other.nullCustomer = nullptr;
        other.nullVolunteer = nullptr;
        other.nullCustomer = nullptr;
}          

WareHouse& WareHouse:: operator = (WareHouse&& other) {
    if(&other != this){
        isOpen = other.isOpen;
        for(Action* a: actionsLog)
            delete a;
        actionsLog.clear();
        for(Action* a: other.actionsLog){
            actionsLog.push_back(a->clone());
            delete a;
        }
        other.actionsLog.clear();    
        for(Volunteer* v: volunteers)
            delete v;
        volunteers.clear();
        for(Volunteer* v: other.volunteers){
            volunteers.push_back(v->clone());
            delete v;
        }
        other.volunteers.clear();    
        for(Order* o : pendingOrders)
            delete o;
        pendingOrders.clear();
        for(Order* o : other.pendingOrders){
            pendingOrders.push_back(o->clone());
            delete o;
        }
        other.pendingOrders.clear();    
        for(Order* o: inProcessOrders)
            delete o;
        inProcessOrders.clear();
        for(Order* o: other.inProcessOrders){
            inProcessOrders.push_back(o->clone());
            delete o;
        }
        other.inProcessOrders.clear();    
        for(Order* o: completedOrders)
            delete o;
        completedOrders.clear();
        for(Order *o: other.completedOrders){
            completedOrders.push_back(o->clone());
            delete o;
        }
        other.completedOrders.clear();    
        for(Customer* c: customers)
            delete c;
        customers.clear();
        for(Customer* c: other.customers){
            customers.push_back(c->clone()); 
            delete c;
        }
        other.customers.clear();
        customerCounter = other.customerCounter;
        volunteerCounter = other.volunteerCounter;
        orderCounter = other.orderCounter;
        isOpen = false;
        customerCounter = 0;
        volunteerCounter = 0;
        orderCounter = 0;
        other.nullCustomer = nullptr;
        other.nullOrder = nullptr;
        other.nullVolunteer = nullptr;
    }
    return *this;
}       




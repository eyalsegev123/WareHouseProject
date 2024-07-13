#include "../include/Action.h"
#include <iostream>

//Action
Action:: Action(): errorMsg(""), status(ActionStatus::ERROR){}

ActionStatus Action:: getStatus() const{return status;}

string Action:: printStatus() const{
    if(getStatus() == ActionStatus::COMPLETED)
        return " COMPLETED";
    else
        return " ERROR " + getErrorMsg();
}

void Action:: complete(){status = ActionStatus::COMPLETED;}

void Action:: error(string errorMsg){
    status = ActionStatus::ERROR;
    this->errorMsg = errorMsg;
    std::cout <<"Error: " << errorMsg << std::endl;
}

string Action:: getErrorMsg() const{return errorMsg;}

Action:: ~Action(){}

//SimulateStep
SimulateStep:: SimulateStep(int numOfSteps):Action(), numOfSteps(numOfSteps){}

void SimulateStep::act(WareHouse &wareHouse) {
    
    for(int m = 0; m < numOfSteps; m++) { 
        // Taking care of the pending orders
        for (Order* pendingOrder : wareHouse.getPendingOrders()) {
            for (Volunteer* volunteer : wareHouse.getVolunteersVector()) {
                if (volunteer->canTakeOrder(*pendingOrder)) {
                    volunteer->acceptOrder(*pendingOrder);
                    pendingOrder->setNeedToMove();
                    if(pendingOrder->getStatus() == OrderStatus::PENDING){
                        pendingOrder->setStatus(OrderStatus::COLLECTING);
                        pendingOrder->setCollectorId(volunteer->getId());
                    }
                    else{
                        pendingOrder->setStatus(OrderStatus::DELIVERING);
                        pendingOrder->setDriverId(volunteer->getId());
                    }
                    break; // Only one volunteer can get the order
                }
            }
        }
        //move order for orders that got a volunteer
        wareHouse.moveOrder("Pending");

        for (Volunteer* v : wareHouse.getVolunteersVector()){
            v->step();
        }
        // Taking care of the inProcess orders that need to move back to pending and completed
        for (Order* inProcessOrder : wareHouse.getInProcessOrders()) {
            if (inProcessOrder->getStatus() == OrderStatus::COLLECTING && wareHouse.getVolunteer(inProcessOrder->getCollectorId()).getCompletedOrderId() != NO_ORDER)  { //its an Order that the collector just finished
                inProcessOrder->setNeedToMove(); //need to move to pendingOrders
                wareHouse.getVolunteer(inProcessOrder->getCollectorId()).setFree();
            }
            else if (inProcessOrder->getStatus() == OrderStatus::DELIVERING && wareHouse.getVolunteer(inProcessOrder->getDriverId()).getCompletedOrderId() != NO_ORDER){
                inProcessOrder->setNeedToMove(); //need to move to completedOrders
                wareHouse.getVolunteer(inProcessOrder->getDriverId()).setFree();
            }
        }
        //move order for orders that finished the process in the inprocess (Pending and completed)
        wareHouse.moveOrder("inProcess");
        wareHouse.deleteVolunteers();
    }

    complete();
    wareHouse.addAction(this);
}

std::string SimulateStep:: toString() const{
    return "simulateStep " + std::to_string(numOfSteps) + " " + "COMPLETED";
}

SimulateStep* SimulateStep:: clone() const{return new SimulateStep(*this);}

//AddOrder

AddOrder:: AddOrder(int id): customerId(id){}

void AddOrder:: act(WareHouse &wareHouse){
    bool canAddOrder = false;
    for(Customer* c: wareHouse.getCustomerVector()){
        if(c->getId() == customerId){
            if(c->canMakeOrder()){
                c->addToOrdersId(wareHouse.getOrderCounter());
                c->decreaseNumOrdersLeft();
                wareHouse.addOrder(new Order(wareHouse.getOrderCounter(), customerId, c->getCustomerDistance()));
                canAddOrder = true;
                complete();
            }
            break; 
        }
    }
    if(!canAddOrder)
        error("Cannot place this order");
    wareHouse.addAction(this);    
}

string AddOrder:: toString() const{return "order " + std::to_string(customerId) + " " + printStatus();}

AddOrder* AddOrder:: clone() const{return new AddOrder(*this);}

//AddCustomer 
AddCustomer:: AddCustomer(const string& customerName, const string& customerType, int distance, int maxOrders):customerName(customerName), customerType(customerType == "soldier" ? CustomerType::Soldier : CustomerType::Civilian), distance(distance), maxOrders(maxOrders){}

void AddCustomer:: act(WareHouse &wareHouse) {
    if(customerType == CustomerType::Civilian){
        wareHouse.addCustomer(new CivilianCustomer(wareHouse.getCustomerCounter(), customerName, distance, maxOrders));
    }
    else 
        wareHouse.addCustomer(new SoldierCustomer(wareHouse.getCustomerCounter(), customerName, distance, maxOrders));
    complete();  
    wareHouse.addAction(this);  
}

AddCustomer* AddCustomer:: clone() const{return new AddCustomer(*this);}

string AddCustomer:: toString() const{
    return "customer " + customerName + " " + (customerType == CustomerType::Soldier ? "Soldier" : "Civilian")+ " "  +
    std::to_string(distance) + " " + std::to_string(maxOrders) + " " + "COMPLETED";
}

//PrintOrderStatus
PrintOrderStatus:: PrintOrderStatus(int id):orderId(id){}

bool PrintOrderStatus:: iterateOverVector(const vector<Order*>& v) const{
    bool found = false;
    for(Order* o: v){
        if(o->getId() == orderId){
            std::cout << o->toString() << std::endl;
            found = true;
            break; 
        }
    }
    return found;
}

void PrintOrderStatus:: act(WareHouse &wareHouse){
    bool found(iterateOverVector(wareHouse.getPendingOrders()));
    if(!found)
        found = iterateOverVector(wareHouse.getInProcessOrders());
    if(!found)
        found = iterateOverVector(wareHouse.getCompletedOrders());
    !found ?  error("Order doesn't exist") : complete();
    wareHouse.addAction(this);
}

PrintOrderStatus * PrintOrderStatus:: clone() const{return new PrintOrderStatus(*this);}

string PrintOrderStatus:: toString() const{
    return "orderStatus " + std::to_string(orderId) + " " + printStatus();
}

//PrintCustomerStatus
PrintCustomerStatus:: PrintCustomerStatus(int customerId):customerId(customerId){}

void PrintCustomerStatus:: act(WareHouse &wareHouse){
    bool found = false;
    for(Customer* c: wareHouse.getCustomerVector()){
        if(c->getId() == customerId){
            cout <<  "CustomerID: " + std::to_string(customerId) << endl;
            for (int orderId : c->getOrdersIds())
            {
                cout << "OrderID: " + std::to_string(orderId) +
                "\nOrderStatus: " + wareHouse.getOrder(orderId).toStringOrderStatus(wareHouse.getOrder(orderId).getStatus()) << endl;
            }
            cout << "numOrdersLeft: " + std::to_string(c->getNumOrdersLeft()) << endl;
            complete();
            found = true;
            break;
        }
    }
    if(!found)
        error("Customer doesn't exist");
    wareHouse.addAction(this);    
}

PrintCustomerStatus * PrintCustomerStatus:: clone() const{return new PrintCustomerStatus(*this);}

string PrintCustomerStatus:: toString() const{
    return "customerStatus " + std::to_string(customerId) + " " + printStatus();
}

//PrintVolunteerStatus
PrintVolunteerStatus:: PrintVolunteerStatus(int id): volunteerId(id){}

void PrintVolunteerStatus:: act(WareHouse &wareHouse){
    bool found = false;
    for(Volunteer* v: wareHouse.getVolunteersVector()){
        if(v->getId() == volunteerId){
            cout << v->toString() << endl; 
            found = true;
            complete();
            break;
        }
    }
    if(!found)
        error("Volunteer doesn't exist");
    wareHouse.addAction(this);    
}

PrintVolunteerStatus * PrintVolunteerStatus:: clone() const{return new PrintVolunteerStatus(*this);}

string PrintVolunteerStatus:: toString() const{
    return "volunteerStatus " + std::to_string(volunteerId) + " " + printStatus();
}

//PrintActionsLog
PrintActionsLog:: PrintActionsLog(){}

void PrintActionsLog:: act(WareHouse &wareHouse){
    for (Action* a : wareHouse.getActions())
        cout << a->toString() << endl;
    complete();
    wareHouse.addAction(this);    
}

PrintActionsLog * PrintActionsLog::clone() const{return new PrintActionsLog(*this);}

string PrintActionsLog:: toString() const{return "log COMPLETED";}

//Close
Close:: Close(){}

Close * Close:: clone() const{return new Close(*this);}

void Close:: act(WareHouse &wareHouse){
    for(Order* o: wareHouse.getPendingOrders()){
        cout << "OrderID: " + std::to_string(o->getId()) +" , CustomerID: " +
        std::to_string(o->getCustomerId()) + ", OrderStatus: " + o->toStringOrderStatus(o->getStatus()) <<endl;
    }
    for(Order* o: wareHouse.getInProcessOrders()){
        cout << "OrderID: " + std::to_string(o->getId()) +" , CustomerID: " +
        std::to_string(o->getCustomerId()) + ", OrderStatus: " + o->toStringOrderStatus(o->getStatus()) <<endl;
    }
    for(Order* o: wareHouse.getCompletedOrders()){
        cout << "OrderID: " + std::to_string(o->getId()) +" , CustomerID: " +
        std::to_string(o->getCustomerId()) + ", OrderStatus: " + o->toStringOrderStatus(o->getStatus()) <<endl;
    }
    wareHouse.addAction(this);
    wareHouse.close();
    complete();
}

string Close:: toString() const{return "close COMPLETED";}

//BackupWareHouse
BackupWareHouse:: BackupWareHouse(){}

void BackupWareHouse:: act(WareHouse & wareHouse){
    if (backup != nullptr)
        delete backup;
    backup = wareHouse.clone();
    complete();
    wareHouse.addAction(this);
}

BackupWareHouse * BackupWareHouse:: clone() const{return new BackupWareHouse(*this);}

string BackupWareHouse:: toString() const{return "backup COMPLETED";}

//RestoreWareHouse
RestoreWareHouse:: RestoreWareHouse(){}

void RestoreWareHouse:: act(WareHouse &wareHouse){
    if(backup == nullptr)
        error("No backup available");
    else{
        wareHouse = *backup;
        complete();
    }
    wareHouse.addAction(this);
}

RestoreWareHouse * RestoreWareHouse::clone() const{return new RestoreWareHouse(*this);}

string RestoreWareHouse:: toString() const{return "restore " + printStatus();}
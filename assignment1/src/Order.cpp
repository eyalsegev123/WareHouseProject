#include "../include/Order.h"

Order:: Order(int id, int customerId, int distance) : id(id), customerId(customerId), distance(distance), status(OrderStatus::PENDING)
, collectorId(NO_VOLUNTEER), driverId(NO_VOLUNTEER), needToMove(false){}

int Order:: getDistance() const{return distance;}

int Order:: getId() const{return id;}

int Order:: getCustomerId() const{return customerId;}

void Order:: setStatus(OrderStatus status){this->status = status;}

void Order:: setCollectorId(int collectorId){this->collectorId = collectorId;}

void Order:: setDriverId(int driverId){this->driverId = driverId;}

int Order:: getCollectorId() const{return collectorId;}

int Order:: getDriverId() const{return driverId;}

string Order:: toStringOrderStatus(OrderStatus os) const{
    if(os == OrderStatus::PENDING)
        return"PENDING";
    else if(os == OrderStatus::COLLECTING)  
        return "COLLECTING";
    else if(os == OrderStatus::DELIVERING) 
        return "DELIVERING";
    else
        return "COMPLETED";    
    return "";
}

bool Order:: getNeedToMove(){
    return needToMove;
}

void Order:: setNeedToMove(){
    needToMove == false ? needToMove = true : needToMove = false;
}

OrderStatus Order:: getStatus() const{return status;}

const string Order:: toString() const{
    string ret = "OrderId: " + std::to_string(getId()) +
    "\nOrderStatus: " + toStringOrderStatus(getStatus()) +
    "\nCustomerId: " + std::to_string(getCustomerId()) +
    "\nCollector: ";
    string collector = std::to_string(getCollectorId());
    if(collector == "-1")
        collector = "None";
    ret = ret + collector + "\nDriver: ";
    string driver = std::to_string(getDriverId());
    if(driver == "-1")
        driver = "None";
    ret = ret + driver;
    return ret;    

}
Order* Order:: clone() const{return new Order(*this);}
Order:: ~Order(){}
#include "../include/Volunteer.h"
#include <iostream>
//Volunteer:

Volunteer:: Volunteer(int id, const string &name): completedOrderId(NO_ORDER), activeOrderId(NO_ORDER), id(id), name(name) {}

int Volunteer:: getId() const{return id;}

const string & Volunteer:: getName() const{return name;}

int Volunteer:: getActiveOrderId() const{return activeOrderId;}

int Volunteer:: getCompletedOrderId() const{return completedOrderId;}//its the last order that he completed

bool Volunteer:: isBusy() const{return activeOrderId != NO_ORDER;}

void Volunteer:: setFree(){
    activeOrderId = NO_ORDER;
    completedOrderId = NO_ORDER;
}

Volunteer:: ~Volunteer(){}

//CollectorVolunteer

CollectorVolunteer:: CollectorVolunteer(int id, const string &name, int coolDown): Volunteer(id, name), coolDown(coolDown), timeLeft(NO_ORDER){}

CollectorVolunteer* CollectorVolunteer:: clone() const {return new CollectorVolunteer(*this);}

void CollectorVolunteer:: step(){
    if(timeLeft != NO_ORDER){
        if (decreaseCoolDown())
            this->completedOrderId = getActiveOrderId();
    }
}

int CollectorVolunteer:: getCoolDown() const{return coolDown;}

int CollectorVolunteer:: getTimeLeft() const{return timeLeft;}

bool CollectorVolunteer:: decreaseCoolDown(){
    timeLeft -= 1;
    return getTimeLeft() == 0;
}

bool CollectorVolunteer:: hasOrdersLeft() const{return true;}

bool CollectorVolunteer:: canTakeOrder(const Order &order) const {return !isBusy() && order.getStatus() == OrderStatus::PENDING;}

void CollectorVolunteer:: acceptOrder(const Order &order){
    if(canTakeOrder(order)){//check the condition
        activeOrderId = order.getId();
        timeLeft = getCoolDown();
    }
}

string CollectorVolunteer:: toString() const
{return "VolunteerId: " + std::to_string(getId()) +
           "\nisBusy: " + (isBusy() == false ? "False" : "True") +
           "\norderId: " + (getActiveOrderId() == NO_ORDER ? "None": std::to_string(getActiveOrderId())) +
           "\ntimeLeft: " + (getTimeLeft() == NO_ORDER ? "None": std::to_string(getTimeLeft())) + 
           "\nordersLeft: No Limit";
}    

void CollectorVolunteer:: setFree(){
    Volunteer::setFree();
    timeLeft = NO_ORDER;
}

//LimitedCollectorVolunteer:

LimitedCollectorVolunteer:: LimitedCollectorVolunteer(int id, const string &name, int coolDown ,int maxOrders): CollectorVolunteer(id, name, coolDown), maxOrders(maxOrders), ordersLeft(maxOrders){}

LimitedCollectorVolunteer* LimitedCollectorVolunteer:: clone() const{return new LimitedCollectorVolunteer(*this);} 

bool LimitedCollectorVolunteer:: hasOrdersLeft() const{return ordersLeft > 0;}

bool LimitedCollectorVolunteer:: canTakeOrder(const Order &order) const{return CollectorVolunteer::canTakeOrder(order) && hasOrdersLeft();}

void LimitedCollectorVolunteer:: acceptOrder(const Order &order){
    CollectorVolunteer::acceptOrder(order);
    ordersLeft -= 1;
}

int LimitedCollectorVolunteer:: getMaxOrders() const{return maxOrders;}

int LimitedCollectorVolunteer:: getNumOrdersLeft() const{return ordersLeft;}

string LimitedCollectorVolunteer:: toString() const{
    return (CollectorVolunteer:: toString()).substr(0, CollectorVolunteer:: toString().length() - 8) + std::to_string(getNumOrdersLeft());
}

//DriverVolunteer:

DriverVolunteer:: DriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep): Volunteer(id, name), maxDistance(maxDistance), distancePerStep(distancePerStep), distanceLeft(NO_ORDER){}

DriverVolunteer * DriverVolunteer:: clone() const {return new DriverVolunteer(*this);}

int DriverVolunteer:: getDistanceLeft() const{return distanceLeft;}

int DriverVolunteer:: getMaxDistance() const{return maxDistance;}

int DriverVolunteer:: getDistancePerStep() const{return distancePerStep;}

bool DriverVolunteer:: decreaseDistanceLeft(){
    distanceLeft -= getDistancePerStep();
    if(distanceLeft < 0)// distanceLeft cant be negative number
        distanceLeft = 0;
    return getDistanceLeft() == 0;    
}

bool DriverVolunteer:: hasOrdersLeft() const{return true;}

bool DriverVolunteer:: canTakeOrder(const Order &order) const{return !isBusy() && order.getDistance() <= maxDistance && order.getStatus() == OrderStatus::COLLECTING;} // Signal if the volunteer is not busy and the order is within the maxDistance

void DriverVolunteer:: acceptOrder(const Order &order){
    if(canTakeOrder(order)){
        distanceLeft = order.getDistance();
        activeOrderId = order.getId();
    }
}

void DriverVolunteer:: step(){
    if(distanceLeft != NO_ORDER){
        if(decreaseDistanceLeft()){
            completedOrderId = getActiveOrderId();
        }
    }
   
}

string DriverVolunteer:: toString() const{
    return "VolunteerId: " + std::to_string(getId()) +
           "\nisBusy: " + (isBusy() == false ? "False" : "True") +
           "\norderId: " + (getActiveOrderId() == NO_ORDER ? "None": std::to_string(getActiveOrderId())) +
           "\ndistanceLeft: " + (getDistanceLeft() == NO_ORDER ? "None": std::to_string(getDistanceLeft())) +
           "\nordersLeft: No Limit";
}

void DriverVolunteer:: setFree(){
    Volunteer::setFree();
    distanceLeft = NO_ORDER;
}

//LimitedDriverVolunteer:

LimitedDriverVolunteer:: LimitedDriverVolunteer(int id, const string &name, int maxDistance, int distancePerStep,int maxOrders): DriverVolunteer(id, name, maxDistance, distancePerStep), maxOrders(maxOrders), ordersLeft(maxOrders){}

LimitedDriverVolunteer* LimitedDriverVolunteer:: clone() const{return new LimitedDriverVolunteer(*this);}

int LimitedDriverVolunteer:: getMaxOrders() const{return maxOrders;}

int LimitedDriverVolunteer:: getNumOrdersLeft() const{return ordersLeft;}

bool LimitedDriverVolunteer:: hasOrdersLeft() const{return getNumOrdersLeft() > 0;}

bool LimitedDriverVolunteer:: canTakeOrder(const Order &order) const{return DriverVolunteer::canTakeOrder(order) && hasOrdersLeft();}  // Signal if the volunteer is not busy, the order is within the maxDistance and have orders left

void LimitedDriverVolunteer:: acceptOrder(const Order &order){
    DriverVolunteer::acceptOrder(order);
    ordersLeft -= 1;
} // Reset distanceLeft to orderDistance and decrease ordersLeft

string LimitedDriverVolunteer:: toString() const{
    return (DriverVolunteer::toString()).substr(0, DriverVolunteer:: toString().length() - 8) + std::to_string(getNumOrdersLeft());
}


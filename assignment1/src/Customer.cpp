#include "../include/Customer.h"

Customer::Customer(int id, const string & name, int locationDistance, int maxOrders) : id(id), name(name), locationDistance(locationDistance)
, maxOrders(maxOrders), ordersId(), numOrdersLeft(maxOrders){}
 
const string & Customer:: getName() const{return name;}

int Customer:: getId() const{return id;}

int Customer:: getCustomerDistance() const{return locationDistance;}

int Customer:: getMaxOrders() const{return maxOrders;}

int Customer:: getNumOrders() const{return maxOrders-numOrdersLeft;}

int Customer:: getNumOrdersLeft(){
    return numOrdersLeft;
}

void Customer:: decreaseNumOrdersLeft(){
    numOrdersLeft--;
}

bool Customer:: canMakeOrder() const{return numOrdersLeft > 0;}

const vector<int> & Customer:: getOrdersIds() const{return ordersId;}

int Customer:: addOrder(int orderId){
    if(canMakeOrder()){
        ordersId.push_back(orderId);
        return orderId;
    }
    return -1;
}

void Customer:: addToOrdersId(int orderId){
    ordersId.push_back(orderId);
}

Customer:: ~Customer(){}

SoldierCustomer:: SoldierCustomer(int id, const string &name, int locationDistance, int maxOrders) : Customer(id, name, locationDistance, maxOrders){}

SoldierCustomer* SoldierCustomer:: clone() const {return new SoldierCustomer(*this);}

CivilianCustomer:: CivilianCustomer(int id, const string &name, int locationDistance, int maxOrders) : Customer(id, name, locationDistance, maxOrders){}

CivilianCustomer* CivilianCustomer:: clone() const {return new CivilianCustomer(*this);}


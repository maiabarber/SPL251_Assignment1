#include <string>
#include <iostream>
#include "Settlement.h"
#include <vector>
#include <cstring>
#include <utility>
#include <stdexcept>
using std::string;
using std::vector;


//Constructor
Settlement::Settlement(const std::string &name, SettlementType type): name(name), type(type){}

//Methods
const std::string& Settlement::getName() const{
    return name;
}

SettlementType Settlement::getType() const{
    return type;
}

const int Settlement::constructionLimit() const {
    switch (type){
        case SettlementType::VILLAGE:
            return 1;
        case SettlementType::CITY:
            return 2;
        case SettlementType::METROPOLIS:
            return 3;
    }
    return 0;
}

const std::string Settlement::toString() const{
    string typeSettlement;
    switch (type){
        case SettlementType::VILLAGE:
            typeSettlement = "Village";
            break;
        case SettlementType::CITY:
            typeSettlement = "City";
            break;
        case SettlementType::METROPOLIS:
            typeSettlement = "Metropolis";
            break;
    }
    return name + typeSettlement + " Settlement";
}



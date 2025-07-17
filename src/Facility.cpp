#include "Facility.h"

// Constructor
Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score), 
      settlementName(settlementName),
      status(FacilityStatus::UNDER_CONSTRUCTIONS), 
      timeLeft(price) {}

Facility::Facility(const FacilityType &type, const string &settlementName):FacilityType(type.getName(),type.getCategory(),type.getCost(),type.getLifeQualityScore(),type.getEconomyScore(),type.getEnvironmentScore()),settlementName(settlementName),status(FacilityStatus::UNDER_CONSTRUCTIONS),timeLeft(type.getCost()) {}

// Methods
const string &Facility::getSettlementName() const {
    return settlementName;
}

const int Facility::getTimeLeft() const {
    return timeLeft;
}

FacilityStatus Facility::step() {
    if (status == FacilityStatus::UNDER_CONSTRUCTIONS) {
        timeLeft--;
        if (timeLeft == 0) {
            status = FacilityStatus::OPERATIONAL;
        }
    }
    return status;
}

void Facility::setStatus(FacilityStatus status) {
    this->status = status;
}

const FacilityStatus& Facility::getStatus() const {
    return status;
}

const string Facility::toString() const {
    return (status == FacilityStatus::UNDER_CONSTRUCTIONS ? "UNDER_CONSTRUCTION" : "OPERATIONAL");
}




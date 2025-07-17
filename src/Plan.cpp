#include "Plan.h"
#include <iostream>

// Constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId), settlement(settlement), selectionPolicy(selectionPolicy),status(PlanStatus::AVALIABLE),facilities(),underConstruction(), facilityOptions(facilityOptions), life_quality_score(0), economy_score(0), environment_score(0) {
        if (selectionPolicy == nullptr) {
        throw std::runtime_error("Selection policy is null");
    }
}

Plan::Plan(const Plan& other, const Settlement &otherSettlement, const vector<FacilityType> otherFacilityOptions): plan_id(other.plan_id), settlement(otherSettlement), selectionPolicy(other.selectionPolicy -> clone()),status(other.status),facilities(),underConstruction(), facilityOptions(otherFacilityOptions), life_quality_score(other.life_quality_score), economy_score(other.economy_score), environment_score(other.environment_score) {
    for (Facility* facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }
    for (Facility* facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
}

// Copy constructor 
Plan::Plan(const Plan& other): plan_id(other.plan_id), settlement(other.settlement), selectionPolicy(other.selectionPolicy -> clone()),status(other.status),facilities(),underConstruction(), facilityOptions(other.facilityOptions), life_quality_score(other.life_quality_score), economy_score(other.economy_score), environment_score(other.environment_score) {
    for (Facility* facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }
    for (Facility* facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
}

// Destructor
Plan::~Plan() {
    delete selectionPolicy;
    for (size_t i = 0; i < facilities.size(); i++) {
        delete facilities.at(i);
    }
    for (size_t i = 0; i < underConstruction.size(); i++) {
        delete underConstruction.at(i);
    }
}

// Move constructor
Plan::Plan(Plan&& other) noexcept : plan_id(other.plan_id), settlement(other.settlement), selectionPolicy(other.selectionPolicy),status(other.status),facilities(),underConstruction(), facilityOptions(other.facilityOptions), life_quality_score(other.life_quality_score), economy_score(other.economy_score), environment_score(other.environment_score) {
    for(size_t i = 0; i < other.facilities.size(); i++){
        facilities.push_back(other.facilities.at(i));
        other.facilities.at(i) = nullptr;
    }
    for (size_t i = 0; i < other.underConstruction.size(); i++) {
        underConstruction.push_back(other.underConstruction.at(i));
        other.underConstruction.at(i) = nullptr;
    }
    other.selectionPolicy = nullptr;
}

// Methods

const int Plan::getId() const {
    return plan_id;
}

const Settlement &Plan::getSettlement() const {
    return settlement;
}

const int Plan::getlifeQualityScore() const {
    return life_quality_score;
}

const int Plan::getEconomyScore() const {
    return economy_score;
}

const int Plan::getEnvironmentScore() const {
    return environment_score;
}

const string Plan::getStatus() const {
    return (status == PlanStatus::AVALIABLE ? "AVALIABLE" : "BUSY");
}

const SelectionPolicy* Plan::getSelectionPolicy() const {
    return selectionPolicy;
}

void Plan::setSelectionPolicy(SelectionPolicy *newSelectionPolicy) {
    if (newSelectionPolicy == nullptr) {
        throw std::runtime_error("Selection policy is null");
    }
    if (selectionPolicy != nullptr) {
        delete selectionPolicy;
    }
    selectionPolicy = newSelectionPolicy;
}

void Plan::step() {
    if (status == PlanStatus::AVALIABLE) {
        int to_build = settlement.constructionLimit() - underConstruction.size();
        for(int i = 0; i < to_build; i++){
            FacilityType facility1 = selectionPolicy->selectFacility(facilityOptions);
            Facility* facility = new Facility(facility1.getName(), settlement.getName(), facility1.getCategory(), facility1.getCost(), facility1.getLifeQualityScore(), facility1.getEconomyScore(), facility1.getEnvironmentScore());
            underConstruction.push_back(facility);
            }
    }
    for(size_t i = 0; i < underConstruction.size(); i++){
        if (underConstruction[i]->step() == FacilityStatus::OPERATIONAL){
            facilities.push_back(underConstruction[i]);
            underConstruction.erase(underConstruction.begin() + i);
            life_quality_score += facilities[i]->getLifeQualityScore();
            economy_score += facilities[i]->getEconomyScore();
            environment_score += facilities[i]->getEnvironmentScore();
            }
    }
    if (underConstruction.size() == static_cast<size_t>(settlement.constructionLimit())) {
        status = PlanStatus::BUSY;
    }
    else {
        status = PlanStatus::AVALIABLE;
    }
}

void Plan::printStatus() {
    std::cout << (status == PlanStatus::AVALIABLE ? "AVALIABLE" : "BUSY") << std::endl;
}

const vector<Facility*> &Plan::getFacilities() const {
    return facilities;
}

const vector<Facility*> &Plan::getUnderConstruction() const {
    return underConstruction;
}

void Plan::addFacility(Facility* facility) {
    if (facility == nullptr) {
        throw std::runtime_error("Facility is null");
    }
    facilities.push_back(facility);
}

const string Plan::toString() const {
    return "Plan ID: " + std::to_string(plan_id) + ", Status: " + (status == PlanStatus::AVALIABLE ? "Available" : "Busy");
}
#include "Simulation.h"
#include "Action.h"
#include "SelectionPolicy.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "Auxiliary.h"

Simulation::Simulation(const string &configFilePath) : isRunning(false), planCounter(0),actionsLog(),plans(),settlements(),facilitiesOptions() {
    // Load configuration from file
    std::ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw std::runtime_error("Unable to open config file");
    }
    for (string line; std::getline(configFile, line);) {
        std::vector<std::string> cur_line = Auxiliary::parseArguments(line);
        if (cur_line[0] == "settlement") {
            SettlementType settlementType = static_cast<SettlementType>(std::stoi(cur_line[2]));
            Settlement *settlement = new Settlement(cur_line[1], settlementType);
            addSettlement(settlement);
        }
        else if (cur_line[0] == "facility") {
            FacilityType facillity = FacilityType(cur_line[1], static_cast<FacilityCategory>(std::stoi(cur_line[2])), std::stoi(cur_line[3]), std::stoi(cur_line[4]), std::stoi(cur_line[5]), std::stoi(cur_line[6]));
            addFacility(facillity);
        }
        else if (cur_line[0] == "plan") {
            Settlement &settlement = getSettlement(cur_line[1]);
            if (cur_line[2] == "bal") {
                addPlan(settlement, new BalancedSelection(0, 0, 0));
            } else if (cur_line[2] == "eco") {
                addPlan(settlement, new EconomySelection());
            } else if (cur_line[2] == "nve") {
                addPlan(settlement, new NaiveSelection());
            } else if (cur_line[2] == "env") {
                addPlan(settlement, new SustainabilitySelection());
            } else {
                throw std::runtime_error("Cannot create this plan");
            }
        }else if (cur_line[0] == "#"){
            continue;
        }
    }
}

// Copy Constructor
Simulation::Simulation(const Simulation& other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions(other.facilitiesOptions) {
    for (size_t i = 0; i < other.actionsLog.size(); i++) {
        actionsLog.push_back(other.actionsLog[i]->clone());
    }
    for (size_t i = 0; i < other.settlements.size(); i++) {
        settlements.push_back(new Settlement(*other.settlements[i]));
    }
    for (Plan plan : other.plans) {
        plans.emplace_back(plan, getSettlement(plan.getSettlement().getName()), facilitiesOptions);
    }
}

// Move Constructor
Simulation::Simulation(Simulation&& other) noexcept
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions(other.facilitiesOptions) {
        
        for (size_t i = 0; i < other.actionsLog.size(); i++) {
            actionsLog.push_back(other.actionsLog.at(i));
            other.actionsLog.at(i) = nullptr;
        }
        for (size_t i = 0; i < other.settlements.size(); i++) {
            settlements.push_back(other.settlements.at(i));
            other.settlements.at(i) = nullptr;
        }

        for (size_t i = 0; i < other.plans.size(); i++) {
            plans.push_back((Plan(other.plans.at(i), getSettlement(other.plans.at(i).getSettlement().getName()), facilitiesOptions)));
        }
        
    other.isRunning = false;
    other.planCounter = 0;
    other.actionsLog.clear();
    other.plans.clear();
    other.settlements.clear();
    other.facilitiesOptions.clear();
}

// Copy Assignment Operator
Simulation& Simulation::operator=(const Simulation& other) {
    if (this != &other) {
        isRunning = other.isRunning;
        planCounter = other.planCounter;
        facilitiesOptions.clear();
        facilitiesOptions = other.facilitiesOptions;
        plans.clear();
        
        for (size_t i = 0; i < actionsLog.size(); i++) {
            delete actionsLog[i];
        }
        actionsLog.clear();

        for (size_t i = 0; i < settlements.size(); i++) {
            delete settlements[i];
        }
        settlements.clear();
        
        for (size_t i = 0; i < other.actionsLog.size(); i++) {
            actionsLog.push_back(other.actionsLog[i]->clone()); //deep copy
        }
        
        for (size_t i = 0; i < other.settlements.size(); i++) {
            settlements.push_back(new Settlement(*other.settlements[i])); //deep copy
        } 

        for (Plan plan : other.plans) {
            plans.push_back(Plan(plan, getSettlement(plan.getSettlement().getName()), facilitiesOptions));
        }

    }
    return *this;
}


// Move Assignment Operator
Simulation& Simulation::operator=(Simulation&& other) noexcept {
    if (this != &other) {
        for (size_t i = 0; i < other.actionsLog.size(); i++) {
            actionsLog.push_back(other.actionsLog.at(i));
            other.actionsLog.at(i) = nullptr;
        }
        for (size_t i = 0; i < other.settlements.size(); i++) {
            settlements.push_back(other.settlements.at(i));
            other.settlements.at(i) = nullptr;
        }

        for (size_t i = 0; i < other.plans.size(); i++) {
            plans.push_back((Plan(other.plans.at(i), getSettlement(other.plans.at(i).getSettlement().getName()), facilitiesOptions)));
        }
        
    other.isRunning = false;
    other.planCounter = 0;
    other.actionsLog.clear();
    other.plans.clear();
    other.settlements.clear();
    other.facilitiesOptions.clear();
    }
    return *this;
}

// Destructor
Simulation::~Simulation() {
    for (size_t i = 0; i < actionsLog.size(); i++) {
        delete actionsLog.at(i);
    }
    actionsLog.clear();

    for (size_t i = 0; i < settlements.size(); i++) {
        delete settlements.at(i);
    }
    settlements.clear();
}


void Simulation::start() {
    open();
    while (isRunning){
        string Input;
        std::getline(std::cin, Input);
        std::vector<std::string> cur_line = Auxiliary::parseArguments(Input);
        string command = cur_line[0];
        if (command == "step"){
            int steps = std::stoi(cur_line[1]);
            addAction(new SimulateStep(steps));
        }
        else if (command=="plan"){
            string settlementName = cur_line[1];
            string selectionPolicy = cur_line[2];
            addAction(new AddPlan(settlementName, selectionPolicy));
        }
        else if (command=="settlement"){
            string settlementName = cur_line[1];
            SettlementType settlementType = static_cast<SettlementType>(std::stoi(cur_line[2]));
            addAction(new AddSettlement(settlementName, settlementType));
        }
        else if (command=="facility"){
            addAction(new AddFacility(cur_line[1], static_cast<FacilityCategory>(std::stoi(cur_line[2])), std::stoi(cur_line[3]), std::stoi(cur_line[4]), std::stoi(cur_line[5]), std::stoi(cur_line[6])));
        }
        else if (command=="planStatus"){
            int planID = std::stoi(cur_line[1]);
            addAction(new PrintPlanStatus(planID));
        }
        else if (command=="changePolicy"){
            int planID = std::stoi(cur_line[1]);
            string selectionPolicy = cur_line[2];
            addAction(new ChangePlanPolicy(planID, selectionPolicy));
        }
        else if (command=="log"){
            addAction(new PrintActionsLog());
        }
        else if (command=="close"){
            addAction(new Close());
        }
        else if (command=="backup"){
            addAction(new BackupSimulation());
        }
        else if (command=="restore"){
            addAction(new RestoreSimulation());
        }
        else{
            std::cout << "Invalid command" << std::endl;
        }
       }
}

void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy) {
    if (selectionPolicy == nullptr) {
        throw std::runtime_error("Selection policy is null");
    }
    plans.push_back(Plan(planCounter++, settlement, selectionPolicy, facilitiesOptions));
    
}

void Simulation::addAction(BaseAction *action) {
    if (action == nullptr) {
        throw std::runtime_error("Action is null");
    }
    action->act(*this);
    actionsLog.push_back(action);
}

bool Simulation::addSettlement(Settlement *settlement) {
    if (settlement == nullptr) {
        throw std::runtime_error("Selection policy is null");
    }
    if (isSettlementExists(settlement->getName())) {
        return false;
    }
    settlements.push_back(settlement);
    return true;
}

bool Simulation::addFacility(FacilityType facility) {
    if (isFacilityExists(facility.getName())) {
        throw std::runtime_error("Facility already exists");
        return false;
    }
    facilitiesOptions.push_back(facility);
    return true;
}

bool Simulation::isSettlementExists(const string &settlementName) {
    for (Settlement *settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return true;
        }
    }
    return false;
}

bool Simulation::isFacilityExists(const string &facilityName) {
    for (FacilityType facility : facilitiesOptions) {
        if (facility.getName() == facilityName) {
            return true;
        }
    }
    return false;
}

Settlement &Simulation::getSettlement(const string &settlementName) {
    for (Settlement *settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return *settlement;
        }
    }
    throw std::runtime_error("Cannot create this plan");
}

const vector<Settlement*> Simulation::getSettlements() {
    return settlements;
}

vector<BaseAction*> Simulation::getActionsLog() {
    return actionsLog;
}

bool Simulation::planExists(const int planID) {
    for (auto& plan : plans) {
        if (plan.getId() == planID) {
            return true;
        }
    }
    return false;
}

Plan &Simulation::getPlan(const int planID) {
    for (auto& plan : plans) {
        if (plan.getId() == planID) {
            return plan;
        }
    }
    throw std::runtime_error("Plan doesn't exist");
}

void Simulation::step() {
    if (!isRunning) {
        throw std::runtime_error("Simulation is not running");
    }
    for (Plan &plan : plans) {
        plan.step();
    }
}

void Simulation::close() {
    isRunning = false;
    for (Plan &plan : plans) {
        std::cout << "PlanID: " + std::to_string(plan.getId()) << std::endl;
        std::cout << "SettlementName: " + plan.getSettlement().getName() << std::endl;
        std::cout << "LifeQualityScore: " + std::to_string(plan.getlifeQualityScore()) << std::endl;
        std::cout << "EconomyScore: " + std::to_string(plan.getEconomyScore()) << std::endl;
        std::cout << "EnvironmentScore: " + std::to_string(plan.getEnvironmentScore()) << std::endl;
    }
}

void Simulation::open() {
    isRunning = true;
    std::cout << "The simulation has started" << std::endl;
}

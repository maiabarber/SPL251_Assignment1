#include "Action.h"
#include "Simulation.h"
#include <iostream>

// BaseAction implementation
BaseAction::BaseAction() : errorMsg(""),status(ActionStatus::COMPLETED) {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

string BaseAction::getStringStatus() const {
    if (status == ActionStatus::COMPLETED) {
        return " COMPLETED";
    } else {
        return " ERROR";
    }
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg) {
    this->errorMsg = errorMsg;
    status = ActionStatus::ERROR;
    std::cerr << "Error: " << errorMsg << std::endl;
}

const string &BaseAction::getErrorMsg() const {
    return errorMsg;
}




// SimulateStep implementation - inherit from BaseAction
SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation) {
    for (int i = 0; i < numOfSteps; i++) {
        simulation.step();
    }
    complete();
    simulation.getActionsLog().push_back(this);
}

const string SimulateStep::toString() const {
    return "step "+ std::to_string(numOfSteps)+ getStringStatus();
}

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}




// AddPlan implementation - inherit from BaseAction
AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {
    }

void AddPlan::act(Simulation &simulation) {
    if (!simulation.isSettlementExists(settlementName)) {
        this->error("Cannot create this plan");        
        simulation.getActionsLog().push_back(this);
        return;
    }
    Settlement& settlement = simulation.getSettlement(settlementName);  
    
    if (selectionPolicy == "bal") {
        simulation.addPlan(settlement,new BalancedSelection(0, 0, 0)); 
        complete(); 
    } else if (selectionPolicy == "eco") {

        simulation.addPlan(settlement,new EconomySelection()); 
        complete(); 
    } else if (selectionPolicy == "nve") {
        simulation.addPlan(settlement, new NaiveSelection()); 
        complete(); 
    } else if (selectionPolicy == "env") {
        simulation.addPlan(settlement, new SustainabilitySelection()); 
        complete(); 
    }else{
        this->error("Cannot create this plan");
    }
    simulation.getActionsLog().push_back(this);
}

const string AddPlan::toString() const {
    return "plan " + settlementName + " " + selectionPolicy+getStringStatus();
}

AddPlan *AddPlan::clone() const {
    return new AddPlan(*this);
}




// AddSettlement implementation - inherit from BaseAction
AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
    : settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation) {
    if (simulation.addSettlement(new Settlement(settlementName, settlementType))){
        complete();
    }else{
        this->error("Settlement already exists");
    } 
    simulation.getActionsLog().push_back(this);
}

const string AddSettlement::toString() const {
    return "settlement " + settlementName+ " "+ std::to_string(static_cast<int>(settlementType)) + getStringStatus();
}

AddSettlement *AddSettlement::clone() const {
    return new AddSettlement(*this);
}




// AddFacility implementation - inherit from BaseAction
AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory, const int price, const int lifeQualityScore, const int economyScore, const int environmentScore)
    : facilityName(facilityName), facilityCategory(facilityCategory), price(price), lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation) {
    FacilityType newFacility(facilityName, facilityCategory, price, lifeQualityScore, economyScore, environmentScore);
    if (simulation.addFacility(newFacility)){
        complete();}
    else{ this->error("Facility already exists");}
    simulation.getActionsLog().push_back(this);
}

const string AddFacility::toString() const {
    return "facility " + facilityName + " " + std::to_string(static_cast<int>(facilityCategory)) + " " + std::to_string(price) + " " + std::to_string(lifeQualityScore) + " " + std::to_string(economyScore) + " " + std::to_string(environmentScore)+getStringStatus();
}

AddFacility *AddFacility::clone() const {
    return new AddFacility(*this);
}




// PrintPlanStatus implementation - inherit from BaseAction
PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation) {
    if (!simulation.planExists(planId)) {
        this->error("Plan doesn't exist");
        simulation.getActionsLog().push_back(this);
        return;
    }
    std::cout << "PlanID: " + std::to_string(simulation.getPlan(planId).getId()) << std::endl;
    std::cout << "SettlementName: " + simulation.getPlan(planId).getSettlement().getName() << std::endl;
    std::cout << "PlanStatus: " + simulation.getPlan(planId).getStatus() << std::endl;
    std::cout << "SelectionPolicy: " + simulation.getPlan(planId).getSelectionPolicy()->toString() << std::endl;
    std::cout << "LifeQualityScore: " + std::to_string(simulation.getPlan(planId).getlifeQualityScore()) << std::endl;
    std::cout << "EconomyScore: " + std::to_string(simulation.getPlan(planId).getEconomyScore()) << std::endl;
    std::cout << "EnvironmentScore: " + std::to_string(simulation.getPlan(planId).getEnvironmentScore()) << std::endl;
    for (Facility* facility : simulation.getPlan(planId).getFacilities()) {
        std::cout << "FacilityName: " + facility->getName() << std::endl;
        std::cout << "FacilityStatus: " + facility->toString() << std::endl;
    }
    for (Facility* facility : simulation.getPlan(planId).getUnderConstruction()) {
        std::cout << "FacilityName: " + facility->getName() << std::endl;
        std::cout << "FacilityStatus: " + facility->toString() << std::endl;
    }
    complete();
    simulation.getActionsLog().push_back(this);
}

const string PrintPlanStatus::toString() const {
    return "planStatus " + std::to_string(planId)+ getStringStatus();
}

PrintPlanStatus *PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this);
}




// ChangePlanPolicy implementation - inherit from BaseAction
ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy)
    : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation) {
    Plan& plan = simulation.getPlan(planId);
    SelectionPolicy* policy = nullptr;
    const SelectionPolicy* oldPolicy = plan.getSelectionPolicy();
    if (!simulation.planExists(planId)) {
        this->error("Cannot change selection policy");
        simulation.getActionsLog().push_back(this);
        return;
    }
    if (newPolicy == "bal" and oldPolicy->toString() != "BalancedSelection") {
        policy = new BalancedSelection(0, 0, 0);
        plan.setSelectionPolicy(policy);
        complete();
        simulation.getActionsLog().push_back(this);
    } else if (newPolicy == "eco" and oldPolicy->toString() != "EconomySelection") {
        policy = new EconomySelection();
        plan.setSelectionPolicy(policy);
        complete();
        simulation.getActionsLog().push_back(this);
    } else if (newPolicy == "nve" and oldPolicy->toString() != "NaiveSelection") {
        policy = new NaiveSelection();
        plan.setSelectionPolicy(policy);
        complete();
        simulation.getActionsLog().push_back(this);
    } else if (newPolicy == "env" and oldPolicy->toString() != "SustainabilitySelection") {
        policy = new SustainabilitySelection();
        plan.setSelectionPolicy(policy);
        complete();
        simulation.getActionsLog().push_back(this);
    }
    else{
        this->error("Cannot change selection policy");
        simulation.getActionsLog().push_back(this);}
}

const string ChangePlanPolicy::toString() const {
    return "changePolicy " + std::to_string(planId) + " " + newPolicy+" "+getStringStatus();
}

ChangePlanPolicy *ChangePlanPolicy::clone() const {
    return new ChangePlanPolicy(*this);
}




// PrintActionsLog implementation - inherit from BaseAction
PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Simulation &simulation) {
    for (BaseAction* action : simulation.getActionsLog()) {
        std::cout << action->toString() << std::endl;
    }
    complete();
    simulation.getActionsLog().push_back(this);
}

const string PrintActionsLog::toString() const {
    return "log+"+getStringStatus();
}

PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}




// Close implementation - inherit from BaseAction
Close::Close() {}

void Close::act(Simulation &simulation) {
    simulation.close();
    complete();
    simulation.getActionsLog().push_back(this);
}

const string Close::toString() const {
    return "close"+getStringStatus();
}

Close *Close::clone() const {
    return new Close(*this);
}




// BackupSimulation implementation - inherit from BaseAction
BackupSimulation::BackupSimulation() {}

void BackupSimulation::act(Simulation &simulation) {
    if (backup != nullptr) {
        delete backup;
    }
    backup = new Simulation(simulation);
    complete();
    simulation.getActionsLog().push_back(this);
}

const string BackupSimulation::toString() const {
    return "backup"+getStringStatus();
}

BackupSimulation *BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}




// RestoreSimulation implementation - inherit from BaseAction
RestoreSimulation::RestoreSimulation() {}

void RestoreSimulation::act(Simulation &simulation) {
    if (backup == nullptr) {
        this->error("No backup available");
        simulation.getActionsLog().push_back(this);
        return;
    }
    simulation = *backup;
    complete();
    simulation.getActionsLog().push_back(this);
}

const string RestoreSimulation::toString() const {
    return "RestoreSimulation";
}

RestoreSimulation *RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}
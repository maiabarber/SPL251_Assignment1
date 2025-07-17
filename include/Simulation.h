#pragma once
#include <string>
#include <vector>
#include "Facility.h"
#include "Plan.h"
#include "Settlement.h"
using std::string;
using std::vector;

class BaseAction;
class SelectionPolicy;

class Simulation {
public:
    Simulation(const string& configFilePath);

    // Rule of 5
    Simulation(const Simulation& other);  // Copy Constructor
    Simulation(Simulation&& other) noexcept;  // Move Constructor
    Simulation& operator=(const Simulation& other);  // Copy Assignment Operator
    Simulation& operator=(Simulation&& other) noexcept;  // Move Assignment Operator
    ~Simulation();  // Destructor

    // Methods
    void start();
    void addPlan(const Settlement& settlement, SelectionPolicy* selectionPolicy);
    void addAction(BaseAction* action);
    bool addSettlement(Settlement* settlement);
    bool addFacility(FacilityType facility);
    bool isSettlementExists(const string& settlementName);
    bool isFacilityExists(const string& facilityName);  
    const vector<Settlement*> getSettlements();  
    vector<BaseAction*> getActionsLog();  
    Settlement& getSettlement(const string& settlementName);
    bool planExists(const int planID);
    Plan& getPlan(const int planID);
    void step();
    void close();
    void open();

private:
    bool isRunning;
    int planCounter;  // For assigning unique plan IDs
    vector<BaseAction*> actionsLog;
    vector<Plan> plans;
    vector<Settlement*> settlements;
    vector<FacilityType> facilitiesOptions;
};



#pragma once
#include <vector>
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
using std::vector;

enum class PlanStatus {
    AVALIABLE,
    BUSY,
};

class Plan {
    public:
        Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions);
        Plan(const Plan& other, const Settlement &otherSettlement, const vector<FacilityType> otherFacilityOptions);//another copy constructor

        // Rule of 5 without Copy Assignment Operator,Move Constructor and Move Assignment Operator, as there is a const member variable
        Plan(const Plan& other); // Copy constructor
        ~Plan(); // Destructor
        Plan& operator=(const Plan& other) = delete; // Copy assignment operator
        Plan& operator=(Plan&& other) = delete; // Move assignment operator
        Plan(Plan&& other) noexcept; // Move constructor


        // Methods
        const int getId() const;
        const Settlement &getSettlement() const;
        const int getlifeQualityScore() const;
        const int getEconomyScore() const;
        const int getEnvironmentScore() const;
        const string getStatus() const;
        const SelectionPolicy* getSelectionPolicy() const;
        void setSelectionPolicy(SelectionPolicy *selectionPolicy);
        void step();
        void printStatus();
        const vector<Facility*> &getFacilities() const;
        const vector<Facility*> &getUnderConstruction() const;
        void addFacility(Facility* facility);
        const string toString() const;

    private:
        int plan_id;
        const Settlement &settlement;
        SelectionPolicy *selectionPolicy;
        PlanStatus status;
        vector<Facility*> facilities;
        vector<Facility*> underConstruction;
        const vector<FacilityType> &facilityOptions;
        int life_quality_score, economy_score, environment_score;
};

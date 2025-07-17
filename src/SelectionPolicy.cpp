#include <iostream>
#include "SelectionPolicy.h"
#include "Facility.h"
#include <string>
#include <limits>


// NaiveSelection implementation
NaiveSelection::NaiveSelection() : lastSelectedIndex(0) {}

// Methods
const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (!facilitiesOptions.empty()) {
        if (lastSelectedIndex >= facilitiesOptions.size()) {
            lastSelectedIndex = 0;
        }
        return facilitiesOptions[lastSelectedIndex++]; 
    }
    throw std::runtime_error("No facilities available.");
}

const string NaiveSelection::toString() const {
    return "nve";
}

NaiveSelection* NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}


// BalancedSelection implementation
BalancedSelection::BalancedSelection(int lifeQualityScore, int economyScore, int environmentScore)
    : LifeQualityScore(lifeQualityScore), EconomyScore(economyScore), EnvironmentScore(environmentScore) {}

const FacilityType& BalancedSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (!facilitiesOptions.empty()) {
        int smallestDistanceIndex = 0;
        int distance = std::numeric_limits<int>::max();

        for (size_t i = 0; i < facilitiesOptions.size(); i++) {
            int lifeQuality = facilitiesOptions[i].getLifeQualityScore() + LifeQualityScore;
            int economy = facilitiesOptions[i].getEconomyScore() + EconomyScore;
            int environment = facilitiesOptions[i].getEnvironmentScore() + EnvironmentScore;

            int maxScore = std::max(lifeQuality, std::max(economy, environment));
            int minScore = std::min(lifeQuality, std::min(economy, environment));

            if (maxScore - minScore < distance) {
                distance = maxScore - minScore;
                smallestDistanceIndex = i;
            }
        }
        LifeQualityScore += facilitiesOptions[smallestDistanceIndex].getLifeQualityScore();
        EconomyScore += facilitiesOptions[smallestDistanceIndex].getEconomyScore();
        EnvironmentScore += facilitiesOptions[smallestDistanceIndex].getEnvironmentScore();
        return facilitiesOptions[smallestDistanceIndex];
    } else {
        throw std::invalid_argument("No facilities available.");
    }
}

const string BalancedSelection::toString() const {
    return "bal";
}

BalancedSelection* BalancedSelection::clone() const {
    return new BalancedSelection(*this);
}


// EconomySelection implementation
EconomySelection::EconomySelection() : lastSelectedIndex(0) {}

const FacilityType& EconomySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (lastSelectedIndex >= facilitiesOptions.size()) {
        lastSelectedIndex = 0;
    }
    for (size_t i = static_cast<size_t>(lastSelectedIndex); i < facilitiesOptions.size(); i++) {
        if (facilitiesOptions[i].getCategory() == FacilityCategory::ECONOMY) {
            lastSelectedIndex = i + 1;
            return facilitiesOptions[i];
        }
    }
    for (size_t i = 0; i < static_cast<size_t>(lastSelectedIndex); i++) {
        if (facilitiesOptions[i].getCategory() == FacilityCategory::ECONOMY) {
            lastSelectedIndex = i + 1;
            return facilitiesOptions[i];
        }
    }
    throw std::runtime_error("No ECONOMY facilities available.");
}

const string EconomySelection::toString() const {
    return "eco";
}

EconomySelection* EconomySelection::clone() const {
    return new EconomySelection(*this);
}


// SustainabilitySelection implementation
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(0) {}

const FacilityType& SustainabilitySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (lastSelectedIndex >= facilitiesOptions.size()) {
        lastSelectedIndex = 0;
        }
    for (size_t i = lastSelectedIndex; i < facilitiesOptions.size(); i++) {
        if (facilitiesOptions[i].getCategory() == FacilityCategory::ENVIRONMENT) {
            lastSelectedIndex = i + 1;
            return facilitiesOptions[i];
        }
    }
    for (size_t i = 0; i < lastSelectedIndex; i++) {
        if (facilitiesOptions[i].getCategory() == FacilityCategory::ENVIRONMENT) {
            lastSelectedIndex = i + 1;
            return facilitiesOptions[i];
        }
    }
    throw std::runtime_error("No ENVIRONMENT facilities available.");
}

const string SustainabilitySelection::toString() const {
    return "env";
}

SustainabilitySelection* SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}

#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;


class Facility;

enum class SettlementType {
    VILLAGE,
    CITY,
    METROPOLIS,
};

class Settlement {
    public:
        //Constructor
        Settlement(const string &name, SettlementType type);
        //Methods
        const string &getName() const;
        SettlementType getType() const;
        const int constructionLimit() const;
        const string toString() const;

    private:
        const string name;
        SettlementType type;
};
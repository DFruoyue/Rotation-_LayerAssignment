#include "database.h"
#include <iostream>
#include <fstream>
#include <vector>

namespace XZA{
    class LayerDistributor{
    private:
        Database& db;
    public:
        LayerDistributor(Database& database): db(database){};
        
    };
}
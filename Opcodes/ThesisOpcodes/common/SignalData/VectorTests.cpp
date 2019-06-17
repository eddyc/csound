#include "catch.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

using namespace Signals;
TEST_CASE( "Vector Core") {
    
    SECTION( "Shift functions" ) {
        
        GIVEN("The following vector data and result data") {
            
            double dataA[4] = {1, 2, 3, 4};
            double dataB[4] = {1, 2, 3, 4};
            double correctA[4] = {0, 1, 2, 3};

            Vector<double> vecA(dataA, 4);
            Vector<double> vecB(dataB, 4);
            Vector<double> correctResultA(correctA, 4);

            
            WHEN("Vector A is shifted by 1") {
                
                vecA.print();
                vecA.shift(-1);
                vecA.print();
                THEN("The result must match the correct result data") {
                    
                    REQUIRE(Vector<double>::equals(vecA, correctResultA) == true);
                }
            }
        }
    }
}

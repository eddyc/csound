#include "catch.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

using namespace Signals;
TEST_CASE( "Vectors") {
    
    SECTION( "Maths functions" ) {
        
        GIVEN("The following vector data and result data") {
            
            double dataA[4] = {1, 2, 3, 4};
            double dataB[4] = {1, 2, 3, 4};
            double dataC[4 * 4] = {0};
            double correctResultData[4 * 4] = {1, 2, 3, 4, 2, 4, 6, 8, 3, 6, 9, 12, 4, 8, 12, 16};
            Vector<double> vecA(dataA, 4);
            Vector<double> vecB(dataB, 4);
            Matrix<double> matC(dataC, 4, 4);
            Matrix<double> correctResult(correctResultData, 4, 4);
            
            WHEN("Outer product function is performed") {
                
                matC.print();
                Vector<double>::outerProduct(vecA, vecB, matC);
                matC.print();
                THEN("The result must match the correct result data") {
                    
                    REQUIRE(Vector<double>::equals(matC, correctResult) == true);
                }
            }
        }
    }
}

//
//  main.cpp
//  UtilityClasses
//
//  Created by Edward Costello on 10/06/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "Signal.hpp"

int main() {
    
    double numbers[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    Matrix<double> mat(numbers, 3, 3);
    
    mat.print();
    
    for (auto i : mat) {
        
        i.print();
    }
}

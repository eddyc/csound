//
//  StridedVectorMaths.cpp
//  Signals
//
//  Created by Edward Costello on 17/01/2018.
//  Copyright © 2018 Edward Costello. All rights reserved.
//


#include "Vector.hpp"

using namespace Signals;

template<typename T>
StridedVector<T> &StridedVector<T>::fill(T input)
{
    for (size_t i = 0; i < elementCount; ++i) {
        
        (*this)[i] = input;
    }
    
    return *this;
}


template class Signals::StridedVector<double>;
template class Signals::StridedVector<float>;

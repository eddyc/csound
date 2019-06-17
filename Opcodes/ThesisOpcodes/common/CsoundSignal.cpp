//
//  CsoundSignal.cpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 15/09/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "CsoundSignal.hpp"
#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif

template<typename T>
CsVector<T>::CsVector(csnd::Csound *csound, size_t elementCount) :
Vector<T>(CsUtilities<T>::allocate(csound, &memory, elementCount), elementCount)
{}

template<>
CsVector<double>::CsVector(csnd::Csound *csound, Vector<double> &original) :
CsVector<double>(csound, original.elementCount)
{
#ifdef __APPLE__
    cblas_dcopy((int)original.elementCount, (const double *)original.data, (int)original.stride, (double *)data, (int)stride);
#else

#endif
}


template<typename T>
CsVector<T>::CsVector(csnd::Csound *csound, std::string path, std::string dataset) :
Vector<T>(path, dataset, [csound, this](size_t elementCount){
    return CsUtilities<T>::allocate(csound, &memory, elementCount);
})
{}

template<typename T>
T *CsUtilities<T>::allocate(csnd::Csound *csound, AUXCH *memory, size_t count) {

    size_t bytes = count * sizeof(T);
    CSOUND *cs = (CSOUND *)csound;
    if (memory->auxp == nullptr || memory->size < bytes) {

        cs->AuxAlloc(cs, bytes, memory);
    }

    return (T *)memory->auxp;
}

template<typename T>
CsMatrix<T>::CsMatrix(csnd::Csound *csound, size_t elementCount) :
Matrix<T>(CsUtilities<T>::allocate(csound, &memory, elementCount * elementCount), elementCount)
{}

template<typename T>
CsMatrix<T>::CsMatrix(csnd::Csound *csound, size_t rowCount, size_t columnCount) :
Matrix<T>(CsUtilities<T>::allocate(csound, &memory, rowCount * columnCount), rowCount, columnCount)
{}

template class CsVector<double>;
template class CsMatrix<double>;


//
//  CsoundSignal.hpp
//  KalmanFormantOpcode
//
//  Created by Edward Costello on 15/09/2017.
//  Copyright © 2017 Edward Costello. All rights reserved.
//

#include "Signal.hpp"
#include <plugin.h>

template <typename T>
class CsVector : public Vector<T> {

    AUXCH memory;

public:

    CsVector<T>(csnd::Csound *csound, size_t elementCount);
    CsVector<T>(csnd::Csound *csound, Vector<T> &original);
    CsVector<T>(csnd::Csound *csound, std::string path, std::string dataset);
};

template <typename T>
class CsMatrix : public Matrix<T> {

    AUXCH memory;
public:

    CsMatrix<T>(csnd::Csound *csound, size_t dimensionElements);
    CsMatrix<T>(csnd::Csound *csound, size_t rowCount, size_t columnCount);
    CsMatrix<T>(csnd::Csound *csound, std::string path, std::string dataset);
};


template<typename T>
struct CsUtilities
{
    static T *allocate(csnd::Csound *csound, AUXCH *memory, size_t count);
};

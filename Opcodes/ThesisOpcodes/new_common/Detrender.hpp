#include "Vector.hpp"
#include <functional>
#include <iostream>
using namespace std;

template <typename T>
class Detrender
{
private:
    size_t lbp;
    size_t binCount;
    Vector<T> work;
    Matrix<T> inputCopy1, inputCopy2, setA1, a1, a2, a3, a4;

public:
    Detrender(function<T*(size_t)> allocator, const size_t binCount);
    Detrender();
    ~Detrender();
    void operator()(const Vector<T>& input, Vector<T>& output);
};

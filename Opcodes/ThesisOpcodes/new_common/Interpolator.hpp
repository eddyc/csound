#include "Vector.hpp"
#include <functional>
#include <iostream>
using namespace std;

template <typename T>
class Interpolator {
private:
    Vector<T> sortedIndices;

public:
    Interpolator(function<T*(size_t)> allocator, const Vector<T>& indices);
    Interpolator();
    ~Interpolator();
    void operator()(const Vector<T>& input, Vector<T>& output);
};

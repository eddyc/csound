#include "LogYPsd.hpp"

template <typename T>
LogYPsd<T>::FilterArgument::FilterArgument(function<T*(size_t)> allocator, size_t frameSize)
    : currentState(allocator, frameSize), previousState(allocator, frameSize)
{
}

const double tau = 200;

template <typename T>
LogYPsd<T>::LogYPsd(function<T*(size_t)> allocator, size_t Fs, size_t frameSize, size_t hopSize)
    : ramp(allocator, frameSize),
      interpScale(allocator, frameSize),
      detrended(allocator, frameSize),
      interpOut(allocator, frameSize),
      nom(allocator, frameSize),
      denom(allocator, frameSize),
      alpha(1. / (tau * (double)Fs / 1000. / (double)hopSize))
{
    double erbMax = 21.4 * log10(0.00437 * (double)Fs / 2 + 1);
    double erbStep = erbMax / (double)frameSize;

    interpScale.ramp(erbStep, erbStep * (double)frameSize).multiply(1. / 21.4);
    ramp.ramp(0, frameSize);
    Vector<T> tens(allocator, frameSize);
    tens.fill(10)
        .power(interpScale)
        .add(-1)
        .multiply(1. / 0.00437)
        .multiply(2. / Fs)
        .multiply(frameSize)
        .add(-1);
    Vector<T>::copy(tens, interpScale);

    new (&interpolator) Interpolator<T>(allocator, interpScale);
}

template <typename T>
void LogYPsd<T>::operator()(const Vector<T>& magnitudes, const Vector<T>& output)
{
    // Vector<T>::multiply(magnitudes, magnitudes, magnitudes);
    magnitudes.add(1e-9);

    Plot<T>::x(magnitudes);
    interpolator(magnitudes, interpOut);
    Plot<T>::x(interpOut);
}

template class LogYPsd<double>;
template class LogYPsd<float>;

#include "LogYPsd.hpp"

template <typename T>
LogYPsd<T>::FilterArgument::FilterArgument(function<T*(size_t)> allocator, size_t binCount)
    : currentState(allocator, binCount), previousState(allocator, binCount)
{
}

const double tau = 200;

template <typename T>
LogYPsd<T>::LogYPsd(function<T*(size_t)> allocator, size_t Fs, size_t binCount, size_t hopSize)
    : ramp(allocator, binCount),
      interpScale(allocator, binCount),
      magnitudes(allocator, binCount),
      interpOut(allocator, binCount),
      detrended(allocator, binCount),
      tens(allocator, binCount),
      nom(allocator, binCount),
      denom(allocator, binCount),
      alpha(1. / (tau * (double)Fs / 1000. / (double)hopSize))
{
    double erbMax = 21.4 * log10(0.00437 * (double)Fs / 2 + 1);
    double erbStep = erbMax / (double)binCount;

    interpScale.ramp(erbStep, erbStep * (double)binCount).multiply(1. / 21.4);
    ramp.ramp(0, binCount);
    tens.fill(10)
        .power(interpScale)
        .add(-1)
        .multiply(1. / 0.00437)
        .multiply(2. / Fs)
        .multiply(binCount)
        .add(-1);
    Vector<T>::copy(tens, interpScale);
    tens.fill(10);
    new (&interpolator) Interpolator<T>(allocator, interpScale);
    new (&detrender) Detrender<T>(allocator, binCount);
}

template <typename T>
void LogYPsd<T>::filter(const FilterArgument& data, const T alpha)
{
    data.currentState.multiply(alpha);
    data.previousState.multiply(1 - alpha);
    data.currentState.add(data.previousState);
    Vector<T>::copy(data.currentState, data.previousState);
}

template <typename T>
void LogYPsd<T>::operator()(const Vector<T>& real, const Vector<T>& imag, const Vector<T>& output)
{
    Vector<T>::magnitudeSquared(real, imag, magnitudes);
    magnitudes.add(1e-9);
    interpolator(magnitudes, interpOut);
    Vector<T>::copy(interpOut, nom.currentState);
    Vector<T>::copy(interpOut, denom.currentState);
    filter(nom, alpha);
    denom.currentState.add(1e-15);
    denom.currentState.log10();
    detrender(denom.currentState, detrended);
    Vector<T>::subtract(detrended, denom.currentState, denom.currentState);
    Vector<T>::power(tens, denom.currentState, denom.currentState);
    denom.currentState.add(1e-9);
    filter(denom, alpha);
    Vector<T>::divide(denom.currentState, nom.currentState, output);
}

template class LogYPsd<double>;
template class LogYPsd<float>;
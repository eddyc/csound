#include "CentreOfGravity.hpp"

template <typename T>
CentreOfGravity<T>::CentreOfGravity(function<T*(size_t)> allocator, size_t Fs, size_t binCount)
{
    logSpectralBinsCount = binCount;
    T erbMax = 21.4 * log10(0.00437 * Fs / 2 + 1);
    T bwidth_ERB = 0.33;
    halfStep = round(bwidth_ERB / 2. / erbMax * logSpectralBinsCount);
    step = round(bwidth_ERB / erbMax * logSpectralBinsCount);
    erbStep = erbMax / (T)logSpectralBinsCount;
    T startValue = halfStep + 1;
    T endValue = logSpectralBinsCount - halfStep;
    T remainder = fabs(endValue - startValue);
    T increment = round(halfStep);
    T size = remainder / increment;
    maximumPeakCount = floor(size) + 1;

    new (&peaktag) ResizableVector<T>(allocator, (const size_t)maximumPeakCount);
    new (&pidx_offs) ResizableVector<T>(allocator, maximumPeakCount);
    new (&pidx_diff) ResizableVector<T>(allocator, maximumPeakCount);
    new (&pidx_offs_prev) ResizableVector<T>(allocator, maximumPeakCount);
    new (&weights) ResizableVector<T>(allocator, maximumPeakCount);
    new (&peaktag) Vector<T>(allocator, maximumPeakCount);
    new (&proximity) Vector<T>(allocator, maximumPeakCount);
    new (&tens) Vector<T>(allocator, maximumPeakCount);
    tens.fill(10);

    new (&index_offs) Vector<T>(allocator, step);
    index_offs.ramp(-halfStep, halfStep);
    new (&segment) Vector<T>(allocator, step);

    firstFrame = true;
}

template <typename T>
void CentreOfGravity<T>::getWeights(Vector<T> logYPsd,
                                    ResizableVector<T> peakidx,
                                    ResizableVector<T> weightsOut)
{
    for (size_t i = 0; i < peakidx.elementCount; ++i) {
        T startIndex = peakidx[i];
        startIndex = round(startIndex - 1 - halfStep);
        T sum = logYPsd.sum(startIndex, step);
        weightsOut[i] = sum;
    }
}

template <typename T>
void CentreOfGravity<T>::operator()(const Vector<T>& logYPsdFrame,
                                    const ResizableVector<T>& peakidx,
                                    const ResizableVector<T>& last_peakidx)
{
    pidx_offs.clear();
    pidx_offs_prev.clear();
    weights.clear();
    peaktag.setElementCount(peakidx.elementCount);
    peaktag.fill(1);
    pidx_offs.setElementCount(peakidx.elementCount);

    for (size_t i = 0; i < maximumPeakCount; ++i) {
        weights.setElementCount(peakidx.elementCount);
        pidx_offs_prev.setElementCount(pidx_offs.elementCount);
        Vector<T>::copy(pidx_offs, pidx_offs_prev);

        pidx_offs.setElementCount(peakidx.elementCount);
        pidx_diff.setElementCount(pidx_offs_prev.elementCount);

        getWeights(logYPsdFrame, peakidx, weights);
    }
}

template class CentreOfGravity<double>;
template class CentreOfGravity<float>;
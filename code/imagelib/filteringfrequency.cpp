#include "image.hpp"

using std::cout;
using std::endl;

void Image::FilterInFrequency(Filter Filter, FilterType type, FilterStage stage, double radius, uint16 n)
{
    cout << "Filter:" << Filter << endl;
    PadImage(2, 2);
    ShiftPeriodicity();
    DFT();
    uint32 imgSize = getImageSize();
    float *filter = new float[imgSize];

    float max = std::numeric_limits<float>::min();
    float min = std::numeric_limits<float>::max();

    for (int y = 0; y < _height; y++)
        for (int x = 0; x < _width; x++)
        {
            uint32 index = y * _width + x;
            double D = sqrt(pow((double)x - (double)_width / 2, 2) + pow((double)y - (double)_height / 2, 2));
            switch (Filter)
            {
            case Ideal:
                filter[index] = IdealFilter(type, radius, D);
                break;
            case Butterworth:
                filter[index] = ButterworthFilter(type, radius, D, n);
                break;
            case Gaussian:
                filter[index] = GaussianFilter(type, radius, D);
            default:
                break;
            }

            if (filter[index] > max)
            {
                max = filter[index];
            }
            if (filter[index] < min)
            {
                min = filter[index];
            }

            // Multiply each pixel by filter
            _complexData[index][REAL] *= filter[index];
            _complexData[index][IMAGINARY] *= filter[index];
        }

    if (stage == FilterStandalone)
    {
        uint16 L = pow(2, _bps);
        float filterPixel;
        for (uint32 i = 0; i < imgSize; i++)
        {
            filterPixel = ((L - 1) * (filter[i] - min)) / (max - min);
            _data[i] = static_cast<unsigned char>((int)round(filterPixel));
        }
        updateHistogram();
        return;
    }

    if (stage == FilterApplied)
    {
        ComplexToData(0.3);
        updateHistogram();
        return;
    }

    IDFT();
    PadImage(0.5, 0.5);
    updateHistogram();
}

float Image::IdealFilter(FilterType type, float D0, float D)
{
    bool filterPx = type == Low ? true : false;q
    return D <= D0 ? filterPx : !filterPx;
}
float Image::ButterworthFilter(FilterType type, float D0, float D, int n)
{
    return type == Low ? (1 / (1 + pow(D / D0, 2 * n))) : (1 / (1 + pow(D0 / D, 2 * n)));
}
float Image::GaussianFilter(FilterType type, float D0, float D)
{
    return type == Low ? (exp(-((D * D) / (2.0f * (D0 * D0))))) : (1 - exp(-((D * D) / (2.0f * (D0 * D0)))));
}

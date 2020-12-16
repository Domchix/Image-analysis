#include "image.hpp"

using Eigen::Vector2f;
using std::cout;
using std::endl;

void Image::updateHistogram()
{
  uint32 imgSize = getImageSize();
  for (uint32 i = 0; i < 256; i++)
  {
    _histogram[i] = 0;
  }
  for (uint32 i = 0; i < imgSize; i++)
  {
    uint32 intensity = (int)_data[i];
    _histogram[intensity]++;
  }
}

void Image::remapPixels()
{
  uint32 imgSize = _height * _width * _channels;
  for (uint32 i = 0; i < imgSize; i++)
  {
    if (round(_lookupTable[(int)_data[i]]) > pow(2, _bps) - 1)
    {
      _data[i] = static_cast<unsigned char>(255);
    }
    else
    {
      _data[i] = static_cast<unsigned char>(round(_lookupTable[(int)_data[i]]));
    }
  }
}

void Image::intensityNegate()
{
  uint16 L = pow(2, _bps);
  for (uint16 i = 0; i < L; i++)
  {
    _lookupTable[i] = (L - 1) - i;
  }
  UpdateIntensityMetadata();
}

void Image::UpdateIntensityMetadata()
{
  remapPixels();
  updateHistogram();
}

void Image::intensityPowerLawFloat(float gamma)
{
  uint16 L = pow(2, _bps);
  for (uint32 i = 0; i < getImageSize(); i++)
  {
    float scaledPixel = _fData[i] / (L - 1);
    _fData[i] = pow(scaledPixel, gamma) * (L - 1);
  }
}

void Image::intensityPowerLawInt(float gamma)
{
  uint16 L = pow(2, _bps);
  _lookupTable[0] = pow(0.5 / 255.0, gamma) * 255;
  for (uint16 i = 1; i < L; i++)
  {
    float scaledPixel = (float)i / (float)(L - 1);
    _lookupTable[i] = pow(scaledPixel, gamma) * 255;
  }
  UpdateIntensityMetadata();
}

void Image::contrastStretching(int nrOfValues, float *values, uint8 algorithm)
{
  int numberOfSlopeChangePoints = nrOfValues / 2;
  float *slopeChangeFractionPoints = new float[numberOfSlopeChangePoints];
  float *desiredValueFractionsAtPoints = new float[numberOfSlopeChangePoints];
  int j = 0;
  for (int i = 0; i < nrOfValues; i++)
  {
    if (i % 2 == 0)
      slopeChangeFractionPoints[j] = values[i];
    else
    {
      desiredValueFractionsAtPoints[j] = values[i];
      j++;
    }
  }

  uint16 L = pow(2, _bps);
  Interval *intervals = new Interval[numberOfSlopeChangePoints + 1];

  switch (numberOfSlopeChangePoints)
  {
  case 1:
  {
    Vector2f middle = Vector2f{round(slopeChangeFractionPoints[0] * (float)L),
                               round(desiredValueFractionsAtPoints[0] * (float)L)};

    intervals[0] = Interval(middle, "r", L);
    intervals[1] = Interval(middle, "l", L);
    break;
  }
  case 2:
  {
    Vector2f point1 = Vector2f{round(slopeChangeFractionPoints[0] * (float)L),
                               round(desiredValueFractionsAtPoints[0] * (float)L)};
    Vector2f point2 = Vector2f{round(slopeChangeFractionPoints[1] * (float)L),
                               round(desiredValueFractionsAtPoints[1] * (float)L)};
    intervals[0] = Interval(point1, "r", L);
    intervals[1] = Interval(point1, point2);
    intervals[2] = Interval(point2, "l", L);

    break;
  }
  default:
  {
    Vector2f point1 = Vector2f{round(slopeChangeFractionPoints[0] * (float)L),
                               round(desiredValueFractionsAtPoints[0] * (float)L)};
    Vector2f point2 = Vector2f{round(slopeChangeFractionPoints[numberOfSlopeChangePoints - 1] * (float)L),
                               round(desiredValueFractionsAtPoints[numberOfSlopeChangePoints - 1] * (float)L)};

    intervals[0] = Interval(point1, "r", L);
    intervals[numberOfSlopeChangePoints] = Interval(point2, "l", L);

    for (uint16 i = 1; i < numberOfSlopeChangePoints; i++)
    {
      Vector2f left = Vector2f{round(slopeChangeFractionPoints[i - 1] * (float)L),
                               round(desiredValueFractionsAtPoints[i - 1] * (float)L)};
      Vector2f right = Vector2f{round(slopeChangeFractionPoints[i] * (float)L),
                                round(desiredValueFractionsAtPoints[i] * (float)L)};

      intervals[i] = Interval(left, right);
    }
    break;
  }
  }

  uint16 point = 0;
  for (uint16 i = 0; i < L; i++)
  {
    if ((float)i == intervals[point].getRight()(0) &&
        intervals[point].getRight()(0) != 255 && intervals[point].getRight()(0) != 256)
    {
      point++;
    }
    switch (algorithm)
    {
    case 0:
    {
      _lookupTable[i] = intervals[point].linearInterpolation(i);
    }
    break;
    case 1:
    {
      _lookupTable[i] = intervals[point].threshold();
    }
    default:
      break;
    }
  }

  remapPixels();
  updateHistogram();
}

void Image::normalizeHistogram()
{
  uint16 L = pow(2, _bps);
  float imgSize = (float)(_height * _width * _channels);
  for (uint16 i = 0; i < L; i++)
  {
    float sumPr = 0;
    for (uint16 j = 0; j < i; j++)
    {
      sumPr += (float)_histogram[j] / imgSize;
    }
    _lookupTable[i] = (L - 1) * sumPr;
  }

  remapPixels();
  updateHistogram();
}

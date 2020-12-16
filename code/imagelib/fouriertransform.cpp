#include "image.hpp"

using std::cout;
using std::endl;

void Image::DFT()
{
  uint32 imgSize = getImageSize();
  fftw_complex *complexInputData = new fftw_complex[imgSize];
  _complexData = new fftw_complex[imgSize];

  for (uint32 i = 0; i < imgSize; i++)
  {
    complexInputData[i][REAL] = _fData[i];
    complexInputData[i][IMAGINARY] = 0;
  }
  delete (_fData);

  fftw_plan DFTPlan = fftw_plan_dft_2d(_width, _height, complexInputData, _complexData, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(DFTPlan);
  fftw_destroy_plan(DFTPlan);
  delete (complexInputData);
  fftw_cleanup();
  ComplexToData(0.3);
}

void Image::IDFT()
{
  uint16 L = pow(2, _bps);
  uint32 imgSize = getImageSize();
  fftw_complex *out = new fftw_complex[imgSize];

  fftw_plan IDFT = fftw_plan_dft_2d(_width, _height, _complexData, out, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(IDFT);
  fftw_destroy_plan(IDFT);
  delete (_complexData);
  fftw_cleanup();

  _fData = new float[imgSize];
  float max = std::numeric_limits<float>::min();
  float min = std::numeric_limits<float>::max();

  for (uint32 i = 0; i < imgSize; i++)
  {
    _fData[i] = (float)out[i][REAL] / (float)imgSize;
  }

  ShiftInversePeriodicity();

  for (uint32 i = 0; i < imgSize; i++)
  {
    if (_fData[i] > max)
    {
      max = _fData[i];
    }
  }

  if (L - 1 > max)
  {
    max = L - 1;
  }

  min = 0;
  
  for (uint32 i = 0; i < imgSize; i++)
  {
    if (_fData[i] < 0)
    {
      _fData[i] = 0;
    }

    _fData[i] = (float)((((float)L - 1.0f) * (_fData[i] - min)) / (max - min));
    _data[i] = (int)(round(_fData[i]));
  }
}

void Image::PadImage(float xMult, float yMult)
{
  uint32 newWidth = (int)(_width * yMult);
  uint32 newHeight = (int)(_height * xMult);

  unsigned char *newData = new unsigned char[newWidth * newHeight * _channels];
  for (uint32 y = 0; y < newHeight; y++)
    for (uint32 x = 0; x < newWidth; x++)
    {
      if (x < _width && y < _height)
      {
        newData[y * newWidth + x] = _data[y * _width + x];
      }
      else
      {
        newData[y * newWidth + x] = 0;
      }
    }
  delete (_data);
  _data = newData;
  _width = newWidth;
  _height = newHeight;
}

void Image::ShiftPeriodicity(bool hideNegative)
{
  _fData = new float[getImageSize()];
  int shift = 0;
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++)
    {
      int index = y * _width + x;
      shift = ((x + y) % 2 == 0) ? 1 : -1;
      _fData[index] = _data[index] * shift;
      if (hideNegative)
      {
        _data[index] = _fData[index] > 0 ? _fData[index] : 0;
      }
      else
      {
        _data[index] = _fData[index];
      }
    }
}

void Image::ShiftInversePeriodicity()
{
  int shift = 0;
  for (uint32 y = 0; y < _height; y++)
    for (uint32 x = 0; x < _width; x++)
    {
      int index = y * _width + x;
      shift = ((x + y) % 2 == 0) ? 1 : -1;
      _fData[index] = _fData[index] * shift;
    }
}

void Image::ComplexToData(float gamma)
{
  uint32 imgSize = getImageSize();
  uint16 L = pow(2, _bps);
  _fData = new float[imgSize];
  double max = std::numeric_limits<float>::min();
  double min = std::numeric_limits<float>::max();

  for (uint32 i = 0; i < imgSize; i++)
  {
    _fData[i] = sqrt(pow(_complexData[i][REAL], 2) + pow(_complexData[i][IMAGINARY], 2));
    if (_fData[i] < min)
    {
      min = _fData[i];
    }
    if (_fData[i] > max)
    {
      max = _fData[i];
    }
  }
  
  for (uint32 i = 0; i < imgSize; i++)
  {
    _fData[i] = (((float)L - 1.0f) * (_fData[i] - min)) / (max - min);
  }
  intensityPowerLawFloat(gamma);
  for (uint32 i = 0; i < imgSize; i++)
  {
    _data[i] = static_cast<unsigned char>((int)round(_fData[i]));
  }
}

void Image::ApplyFourierTransform(Image::FourierStage stage)
{
  FourierTransform(stage);
  cout << "Width:" << _width << " Height:" << _height << endl;
  updateHistogram();
}

void Image::FourierTransform(Image::FourierStage stage)
{
  PadImage(2, 2);
  if (stage == Padded)
  {
    cout << "done with fourier after paddding" << endl;
    return;
  }

  ShiftPeriodicity(true);
  if (stage == Shifted)
  {
    cout << "done with fourier after shifting" << endl;
    return;
  }

  DFT();
  if (stage == dft)
  {
    cout << "done with fourier after dft" << endl;
    return;
  }

  IDFT();
  if (stage == idft)
  {
    cout << "done with fourier after idft" << endl;
    return;
  }

  PadImage(0.5, 0.5);
  cout << "done with fourier after returning to original" << endl;
}

void Image::generateLineImage(float alphaXMultiplier, float alphaYMultiplier)
{
  uint16 L = pow(2, _bps);
  float alphaX = 2.0f * M_PI / static_cast<float>(_width);
  float alphaY = 2.0f * M_PI / static_cast<float>(_height);

  for (uint32 y = 0; y < _height; y++)
  {
    for (uint32 x = 0; x < _width; x++)
    {
      float i = (L - 1) * fabs(cos(alphaXMultiplier * alphaX * static_cast<float>(x) + alphaYMultiplier * alphaY * static_cast<float>(y)));
      _data[y * _width + x] = static_cast<unsigned char>((int)i);
    }
  }

  updateHistogram();
}

void Image::generateCircleImage(float alphaMultiplier)
{
  uint16 L = pow(2, _bps);
  float alphaX = 1.0f * M_PI / static_cast<float>(_width);
  float alphaY = 1.0f * M_PI / static_cast<float>(_height);

  for (uint32 y = 0; y < _height; y++)
  {
    for (uint32 x = 0; x < _width; x++)
    {
      float i = (L - 1) * fabs(cos(alphaMultiplier * alphaX * static_cast<float>(x)));
      i += (L - 1) * fabs(cos(alphaMultiplier * alphaY * static_cast<float>(y)));

      _data[y * _width + x] = static_cast<unsigned char>((int)i);
    }
  }

  updateHistogram();
}

#include "image.hpp"

using std::cout;
using std::endl;

Image::Image(){};

Image::Image(std::string filename)
{
  openFile(filename);
  _region.max_x = (float)(_width - 1);
  _region.max_y = (float)(_height - 1);
  updateHistogram();
}

Image::Image(const Image &image, bool rgb)
{
  _channels = image._channels;
  _bps = image._bps;
  _pixelUnit = image._pixelUnit;
  _width = image._width;
  _height = image._height;

  unsigned long size = getImageSize();
  _data = new unsigned char[size];
  if (rgb)
  {
    _redData = new unsigned char[size];
    _greenData = new unsigned char[size];
    _blueData = new unsigned char[size];
  }

  for (long i = 0; i < size; i++)
  {
    _data[i] = image._data[i];
    if (rgb)
    {
      _redData[i] = image._redData[i];
      _greenData[i] = image._greenData[i];
      _blueData[i] = image._blueData[i];
    }
  }

  updateHistogram();
}

Image::Image(BBox box)
{
  _region = box;
  _data = new unsigned char[_width * _height];
  _channels = 1;
  updateHistogram();
}

// Constructor for transformation only 1channel/8bps allowed
Image::Image(unsigned int width, unsigned int height, float pixelUnit)
{
  _width = width;
  _height = height;
  _pixelUnit = pixelUnit;
  _bps = 8;
  _channels = 1;
  _data = new unsigned char[_width * _height];
  updateHistogram();
}

Image::Image(unsigned int width, unsigned int height, float alphaX, float alphaY)
{
  _width = width;
  _height = height;
  _channels = 1;
  _bps = 8;
  _data = new unsigned char[getImageSize()];

  generateLineImage(alphaX, alphaY);
}

Image::Image(float alphaX, unsigned int width, unsigned int height)
{
  _width = width;
  _height = height;
  _channels = 1;
  _bps = 8;
  _data = new unsigned char[getImageSize()];

  generateCircleImage(alphaX);
}

Image::~Image()
{
  delete[] _data;
};

void Image::OutputMetadata()
{
  cout << "Width: " << _width << endl;
  cout << "Height: " << _height << endl;
  cout << "Channels: " << _channels << endl;
  cout << "BPS: " << _bps << endl;
  cout << "Size: " << getImageSize() << endl;
}

void Image::CopyFromImage(const Image &image)
{
  _channels = image._channels;
  _bps = image._bps;
  _pixelUnit = image._pixelUnit;
  _width = image._width;
  _height = image._height;

  unsigned long size = getImageSize();
  _data = new unsigned char[size];

  for (long i = 0; i < size; i++)
  {
    _data[i] = image._data[i];
  }

  updateHistogram();
}

void Image::CopyData(unsigned char *fromData, unsigned char *toData, uint32 size)
{
  for (uint32 i = 0; i < size; i++)
  {
    toData[i] = fromData[i];
  }
}

bool Image::openFile(std::string filename)
{
  // Check file type by filename extensions
  if (filename.substr(filename.find_last_of(".") + 1) == "tif")
  {
    return loadTiff(filename);
  }
  else if (filename.substr(filename.find_last_of(".") + 1) == "jpg")
  {
    return loadJpeg(filename);
  }
  return false;
};

void Image::SetViewToSingleColor(Color color)
{
  unsigned char *data;

  switch (color)
  {
  case Red:
    data = _redData;
    break;
  case Blue:
    data = _blueData;
    break;
  case Green:
    data = _greenData;
    break;
  }

  SetDataToView(data, 1);
}

void Image::SetDataToView(unsigned char *data, int channels)
{
  _channels = channels;
  uint32 imageSize = getImageSize();
  delete (_data);
  _data = new unsigned char[imageSize];
  for (int i = 0; i < imageSize; i++)
  {
    _data[i] = data[i];
  }
}

unsigned char *Image::getImageData() { return _data; };
unsigned long Image::getWidth() { return _width; };
unsigned long Image::getHeight() { return _height; };
unsigned long Image::getDepth() { return _depth; };
unsigned long Image::getChannels() { return _channels; };
unsigned long Image::getBitsPerSample() { return _bps; };
unsigned long Image::getSamplesPerPixel() { return _channels; };
unsigned long Image::getPixelUnit() { return _pixelUnit; };
unsigned long Image::getImageSize() { return _width * _height * _channels; }
BBox Image::getRegion() { return _region; };
std::vector<unsigned int> Image::getHistogram() { return _histogram; };

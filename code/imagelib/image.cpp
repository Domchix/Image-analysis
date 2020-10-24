#include "image.hpp"

#include <iostream>

Image::Image(){};

Image::Image(std::string filename)
{
  openFile(filename);
  _region.max_x = (float)(_width - 1);
  _region.max_y = (float)(_height - 1);
  calculateHistogram();
}

Image::Image(const Image &image)
{
  _channels = image._channels;
  _bps = image._bps;
  _pixelUnit = image._pixelUnit;
  _width = image._width;
  _height = image._height;

  unsigned long size = _width * _height * _channels;
  _data = new unsigned char[size];
  for (long i = 0; i < size; i++)
  {
    _data[i] = image._data[i];
  }
  calculateHistogram();
}

Image::Image(BBox box)
{
  _region = box;
  _data = new unsigned char[_width * _height];
  _channels = 1;
  calculateHistogram();
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
  calculateHistogram();
}

Image::~Image()
{
  delete[] _data;
};

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

unsigned char *Image::getImageData() { return _data; };
unsigned long Image::getWidth() { return _width; };
unsigned long Image::getHeight() { return _height; };
unsigned long Image::getDepth() { return _depth; };
unsigned long Image::getChannels() { return _channels; };
unsigned long Image::getBitsPerSample() { return _bps; };
unsigned long Image::getSamplesPerPixel() { return _channels; };
unsigned long Image::getPixelUnit() { return _pixelUnit; };
Image::BBox Image::getRegion() { return _region; };
std::vector<unsigned int> Image::getHistogram() { return _histogram; };

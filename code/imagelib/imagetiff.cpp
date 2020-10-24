#include "image.hpp"

#include <iostream>
#include <tiffio.h> // Note use of libtiff
#include <cstring>

// See http://www.libtiff.org/ for use
// and https://www.awaresystems.be/imaging/tiff.html for tags

bool Image::readTiffMetaData(TIFF *tiff)
{
  std::cout<<"Test";
  TIFFSetDirectory(tiff, 0); // NB!
  // Read using TIFFGetField
  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &_width);
  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &_height);
  TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &_bps);
  TIFFGetField(tiff, TIFFTAG_IMAGEDEPTH, &_depth);
  TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &_channels);
  return true;
}

bool Image::loadTiffTiled(TIFF *tiff)
{
  std::cout << "Tiff is tiled" << std::endl;
  // Get tile info
  unsigned long tileWidth{0};
  unsigned long tileHeight{0};
  TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tileWidth);
  TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tileHeight);

  std::cout << "Tile Width: " << tileWidth << std::endl;
  std::cout << "Tile Height: " << tileHeight << std::endl;

  // Aloc image
  _data = (unsigned char *)_TIFFmalloc(_width * _height * _channels);

  // Aloc tile
  unsigned char *buffer = (unsigned char *)_TIFFmalloc(tileWidth * tileHeight * _channels);

  // Iterate tiles
  for (unsigned long y = 0; y < _height; y += tileHeight)
  {
    for (unsigned long x = 0; x < _width; x += tileWidth)
    {
      // Read tile
      TIFFReadTile(tiff, buffer, x, y, 0, 0);

      // Iterate
      for (unsigned int ty = 0; ty < tileHeight; ty++)
      {
        for (unsigned long tx = 0; tx < tileWidth; tx++)
        {
          if (((x + tx) < _width) && ((y + ty) < _height))
          {
            unsigned long dest = ((y + ty) * _width + (x + tx)) * _channels;
            unsigned long source = (ty * tileWidth + tx) * _channels;

            _data[dest] = buffer[source];
            _data[dest + 1] = buffer[source + 1];
            _data[dest + 2] = buffer[source + 2];
          }
        }
      }
    }
  }
  _TIFFfree(buffer);
  std::cout << "Finished reading" << std::endl;
  return true;
}

bool Image::loadTiffScanline(TIFF *tiff)
{
  std::cout << "Tiff is striped" << std::endl;
  tmsize_t lineSize = TIFFScanlineSize(tiff);

  // Aloc image
  _data = (unsigned char *)_TIFFmalloc(_width * _height * _channels);

  // Aloc buffer
  unsigned char *buf = (unsigned char *)_TIFFmalloc(lineSize);

  for (unsigned int row = 0; row < _height; row++)
  {
    tmsize_t offset = lineSize * row;
    TIFFReadScanline(tiff, buf, row, 0);
    for (size_t x = 0; x < lineSize; x++)
    {
      _data[offset + x] = buf[x];
    }
  }
  std::cout << "Finished reading" << std::endl;

  _TIFFfree(buf);
  return true;
}

bool Image::loadTiff(std::string filename)
{
  std::cout << "Loading tif: " << filename << std::endl;

  TIFF *tiff = TIFFOpen(filename.c_str(), "r");
  // Read image meta data, height, width etc.

  this->readTiffMetaData(tiff);
  
  if (TIFFIsTiled(tiff))
    return loadTiffTiled(tiff);
  else
    return loadTiffScanline(tiff);
  return true;
};

bool Image::saveTiff()
{
  std::cout << "Saving file newTiff.tif" << std::endl;
  TIFF *tiff = TIFFOpen("../../newTiff.tif", "w");

  TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, _width);
  TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, _height);
  TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, _channels);
  TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, _bps);
  TIFFSetField(tiff, TIFFTAG_IMAGEDEPTH, _depth);

  //uint32 rowsPerStrip = image->getHeight();
  //rowsPerStrip = TIFFDefaultStripSize(tiff, rowsPerStrip);
  //std::cout << "Rows per strip: " << rowsPerStrip << std::endl;
  //TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, 8);
  //TIFFSetupStrips(tiff);

  unsigned long lineSize = _channels * _width;

  unsigned char *buf = (unsigned char *)_TIFFmalloc(lineSize);
  for (unsigned long row = 0; row < _height; row++)
  {
    unsigned long offset = lineSize * row;
    for (unsigned long x = 0; x < lineSize; x++)
    {
      buf[x] = _data[x + offset];
    }

    TIFFWriteScanline(tiff, buf, row, 0);
  }

  TIFFClose(tiff);
  _TIFFfree(buf);

  // uint32 rowsPerStrip;
  // rowsPerStrip = _height;
  // rowsPerStrip = TIFFDefaultStripSize(tiff, rowsPerStrip);
  // TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, rowsPerStrip);
  // TIFFSetupStrips(tiff);

  // // row buffer
  // unsigned long scanlineSize = TIFFScanlineSize(tiff);
  // unsigned char *scanline = (unsigned char *)_TIFFmalloc(scanlineSize);

  // // write image
  // for (int i = 0; i < _height; i++)
  // {
  //   std::memcpy(scanline, _data + iplImage->widthStep * i, scanlineSize);
  //   if (TIFFWriteScanline(tiffImage, scanline, i, 0) != 1)
  //   {
  //     //Error
  //   }
  // }

  // // clean up
  // _TIFFfree(scanline);

  return true;
}

Image::Image(std::string filename1, std::string filename2, std::string filename3)
{
  std::cout << "Loading tiff files: " << std::endl
            << filename1 << std::endl
            << filename2 << std::endl
            << filename3 << std::endl;

  Image *image1 = new Image(filename1);
  Image *image2 = new Image(filename2);
  Image *image3 = new Image(filename3);

  std::cout << "Combining tiff files" << std::endl;

  unsigned long size = image1->_width * image1->_height;

  // Transfer metadata
  _width = image1->_width;
  _height = image1->_height;
  _bps = image1->_bps;
  _depth = image1->_depth;
  _channels = 3;
  _data = new unsigned char[size * 3];

  //Combine data
  for (unsigned long x = 0; x < size; x++)
  {
    _data[x * 3] = image1->_data[x];
    _data[x * 3 + 1] = image2->_data[x];
    _data[x * 3 + 2] = image3->_data[x];
  }

  //Save to file
  this->saveTiff();
}

// combine example -> ./imageviewer ../../week1/Region_001_FOV_00041_Acridine_Or_Gray.tif ../../week1/Region_001_FOV_00041_DAPI_Gray.tif ../../week1/Region_001_FOV_00041_FITC_Gray.tif
// ./imageviewer ../../week1/Region_001_FOV_00041_Acridine_Or_Gray.tif ../../week1/Region_001_FOV_00041_FITC_Gray.tif ../../week1/Region_001_FOV_00041_DAPI_Gray.tif
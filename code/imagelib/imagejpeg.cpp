#include "image.hpp"

#include <iostream>

#include <stdlib.h>
#include <jpeglib.h> // Note use of jpeg library
// See https://libjpeg-turbo.org/Documentation/Documentation

bool Image::loadJpegImageFile(char *lpFilename)
{
  struct jpeg_decompress_struct info;
  struct jpeg_error_mgr err;

  struct imgRawImage *lpNewImage;

  unsigned long int dwBufferBytes;
  unsigned char *lpData;

  unsigned char *lpRowBuffer[1];

  FILE *fHandle;

  fHandle = fopen(lpFilename, "rb");
  if (fHandle == NULL)
  {
    return NULL;
  }

  info.err = jpeg_std_error(&err);
  jpeg_create_decompress(&info);

  jpeg_stdio_src(&info, fHandle);
  jpeg_read_header(&info, TRUE);

  jpeg_start_decompress(&info);

  _width = info.output_width;
  _height = info.output_height;
  _channels = info.num_components;

  std::cout << "Width: " << _width << "Height: " << _height << " Channels:" << _channels << std::endl;

  dwBufferBytes = _width * _height * _channels;
  std::cout << "Size: " << dwBufferBytes << std::endl;
  lpData = (unsigned char *)malloc(sizeof(unsigned char) * dwBufferBytes);

  _data = new unsigned char[dwBufferBytes];

  while (info.output_scanline < info.output_height)
  {
    unsigned long lineSize = _channels * info.output_width;
    unsigned long offset = info.output_scanline * lineSize;
    lpRowBuffer[0] = (unsigned char *)(&lpData[lineSize * info.output_scanline]);
    jpeg_read_scanlines(&info, lpRowBuffer, 1);

    for (unsigned long x = 0; x < lineSize; x++)
    {
      _data[offset + x] = lpRowBuffer[0][x];
    }
  }

  jpeg_finish_decompress(&info);
  jpeg_destroy_decompress(&info);
  fclose(fHandle);

  return true;
}

bool Image::loadJpeg(std::string filename)
{

  std::cout << "Loading jpeg: " << filename << std::endl;
  char *c_filename = new char[filename.size() + 1];
  std::copy(filename.begin(), filename.end(), c_filename);
  c_filename[filename.size()] = '\0'; // don't forget the terminating 0

  return loadJpegImageFile(c_filename);
};

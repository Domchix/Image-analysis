#include "image.hpp"

using std::cout;
using std::endl;

void Image::Erosion(unsigned char *data, int XWidth, int YWidth)
{
    unsigned char *tempData = new unsigned char[getImageSize()];
    int halfY = (YWidth - 1) / 2;
    int halfX = (XWidth - 1) / 2;
    uint16 rectangleSize = XWidth * YWidth;
    uint16 pixelCount = 0;
    if (XWidth % 2 == 0)
    {
        halfX = XWidth / 2;
    }
    if (YWidth % 2 == 0)
    {
        halfY = YWidth / 2;
    }
    std::cout << (XWidth - 1) << " " << halfY << " " << rectangleSize << std::endl;

    for (int y = 0; y < _height; y++)
    {
        for (int x = 0; x < _width; x++)
        {
            for (int y2 = -halfY; y2 <= halfY; y2++)
            {
                for (int x2 = -halfX; x2 <= halfX; x2++)
                {
                    // Check if selected point is contained by the image
                    int selectedX = x + x2;
                    int selectedY = y + y2;
                    if (selectedX >= 0 && selectedY >= 0 && selectedX < _width && selectedY < _height)
                    {
                        if (data[selectedY * _width + selectedX] == MAX_INTENSITY)
                        {
                            pixelCount++;
                        }
                    }
                }
            }
            tempData[y * _width + x] = rectangleSize <= pixelCount ? MAX_INTENSITY : MIN_INTENSITY;
            pixelCount = 0;
        }
    }

    for (uint32 i = 0; i < getImageSize(); i++)
    {
        data[i] = tempData[i];
    }
    delete (tempData);
}

void Image::Dilation(unsigned char *data, int width)
{
    unsigned char *tempData = new unsigned char[getImageSize()];
    uint16 L = pow(2, _bps);
    int halfSquare = (width - 1) / 2;

    for (int y = 0; y < _height; y++)
        for (int x = 0; x < _width; x++)
        {
            tempData[y * _width + x] = MIN_INTENSITY;
            for (int y2 = -halfSquare; y2 <= halfSquare; y2++)
                for (int x2 = -halfSquare; x2 <= halfSquare; x2++)
                {
                    int selectedX = x + x2;
                    int selectedY = y + y2;
                    if (selectedX >= 0 && selectedY >= 0 && selectedX < _width && selectedY < _height)
                    {
                        if (data[y * _width + x] == MAX_INTENSITY)
                        {
                            tempData[selectedY * _width + selectedX] = MAX_INTENSITY;
                        }
                    }
                }
        }

    for (uint32 i = 0; i < getImageSize(); i++)
    {
        data[i] = tempData[i];
    }
    delete (tempData);
}

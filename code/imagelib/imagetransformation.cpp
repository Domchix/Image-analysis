#include <iostream>
#include "image.hpp"

#include <Eigen/Core>
#include <Eigen/LU>

using Eigen::Matrix3f;
using Eigen::Vector3f;
using Eigen::Vector3i;
using std::cout;
using std::endl;
using std::max;
using std::min;

void printRegion(Image::BBox region)
{
    cout << "Region: " << endl
         << "minX:" << region.min_x << " maxX:" << region.max_x << endl
         << "minY:" << region.min_y << " maxY:" << region.max_y << endl;
}

Matrix3f getIndexToWorldMatrix(float height, Image::BBox region, float pixelUnit)
{
    Matrix3f offset;
    offset.setIdentity();
    offset(1, 2) = -(height - 1);

    Matrix3f flip_axis;
    flip_axis.setIdentity();
    flip_axis(1, 1) = -1;

    Matrix3f scale_matrix;
    scale_matrix.setIdentity();
    scale_matrix(0, 0) = pixelUnit;
    scale_matrix(1, 1) = pixelUnit;

    Matrix3f offset_matrix;
    offset_matrix.setIdentity();
    offset_matrix(0, 2) = region.min_x;
    offset_matrix(1, 2) = region.min_y;

    return offset_matrix * scale_matrix * flip_axis * offset;
}

Matrix3f getWorldToIndexMatrix(Matrix3f Iw)
{
    return Iw.inverse();
}

void Image::transformRegion(Matrix3f Iw, Matrix3f T, BBox &newRegion)
{
    Vector3f p00{(float)_region.min_x, (float)_region.min_y, 1};
    Vector3f p11{(float)_region.max_x, (float)_region.max_y, 1};
    Vector3f p01{(float)_region.min_x, (float)_region.max_y, 1};
    Vector3f p10{(float)_region.max_x, (float)_region.min_y, 1};

    Vector3f p00New = Iw * T * p00;
    Vector3f p11New = Iw * T * p11;
    Vector3f p01New = Iw * T * p01;
    Vector3f p10New = Iw * T * p10;

    newRegion.min_x = min(min(p00New[0], p01New[0]), min(p10New[0], p11New[0]));
    newRegion.min_y = min(min(p00New[1], p01New[1]), min(p10New[1], p11New[1]));
    newRegion.max_x = max(max(p00New[0], p01New[0]), max(p10New[0], p11New[0]));
    newRegion.max_y = max(max(p00New[1], p01New[1]), max(p10New[1], p11New[1]));
    printRegion(newRegion);
}

Image *Image::transformImage()
{
    Matrix3f scale;
    scale.setIdentity();
    scale(0, 0) = 2.0;
    scale(1, 1) = 2.0;
    return transformImage(scale, false);
}

Image *Image::transformImage(Matrix3f T, bool useBiLinear)
{
    printRegion(_region);
    BBox newRegion = _region;
    _pixelUnit = 1;

    Matrix3f Iw = getIndexToWorldMatrix(_height, _region, _pixelUnit);

    transformRegion(Iw, T, newRegion);

    unsigned long newWidth = (newRegion.max_x - newRegion.min_x + 1);
    unsigned long newHeight = (newRegion.max_y - newRegion.min_y + 1);
    cout << "New width: " << newWidth << endl;
    cout << "New height: " << newHeight << endl;

    Image *newImage = new Image(newWidth, newHeight, 1);
    newImage->_region = newRegion;

    unsigned long size = newWidth * newHeight;
    unsigned char *newData = new unsigned char[size];

    Matrix3f IwNew = getIndexToWorldMatrix(newHeight, newRegion, _pixelUnit);
    Matrix3f Wi = getWorldToIndexMatrix(IwNew);

    Matrix3f inversedTransformation = (Iw * T * Wi).inverse();

    for (unsigned long y = 0; y < newHeight; y++)
    {
        for (unsigned long x = 0; x < newWidth; x++)
        {
            Vector3f newIndex{(float)x, (float)y, 1.0};
            Vector3f oldIndex = inversedTransformation * newIndex;

            unsigned char newIntensity;
            if (useBiLinear)
            {
                newIntensity = getIntensityNearestBiLinear(oldIndex);
            }
            else
            {
                newIntensity = getIntensityNearestNeighbor(oldIndex);
            }

            unsigned long dataIndex = (y * newWidth * _channels) + (x * _channels);
            newData[dataIndex] = newIntensity;
        }
    }

    newImage->_data = newData;
    return newImage;
}

unsigned char Image::getIntensityNearestBiLinear(Eigen::Vector3f &idx)
{
    unsigned long XLess = floor(idx[0]);
    return 0;
}

unsigned char Image::getIntensityNearestNeighbor(Eigen::Vector3f &idx)
{
    Vector3i roundedVector{round(idx[0]), round(idx[1]), 1};
    return getIntensity(roundedVector);
}

unsigned char Image::getIntensity(Eigen::Vector3i &idx)
{
    unsigned long index = idx[1] * _width + idx[0];
    if (index > _width * _height * _channels)
    {
        return 0;
    }
    return _data[index];
}

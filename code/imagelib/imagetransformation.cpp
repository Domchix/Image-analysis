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
    scale_matrix(0, 0) = (float)pixelUnit;
    scale_matrix(1, 1) = (float)pixelUnit;

    Matrix3f offset_matrix;
    offset_matrix.setIdentity();
    offset_matrix(0, 2) = (float)region.min_x;
    offset_matrix(1, 2) = (float)region.min_y;

    return offset_matrix * scale_matrix * flip_axis * offset;
}

Matrix3f getWorldToIndexMatrix(Matrix3f Iw)
{
    return Iw.inverse();
}

void Image::transformRegion(Matrix3f Iw, Eigen::Matrix3f Wi, Matrix3f T, BBox &newRegion)
{
    Vector3f p00{(float)_region.min_x, (float)_region.min_y, 1};
    Vector3f p11{(float)_region.max_x, (float)_region.max_y, 1};
    Vector3f p01{(float)_region.min_x, (float)_region.max_y, 1};
    Vector3f p10{(float)_region.max_x, (float)_region.min_y, 1};

    Vector3f p00New = Iw * T * Wi * p00;
    Vector3f p11New = Iw * T * Wi * p11;
    Vector3f p01New = Iw * T * Wi * p01;
    Vector3f p10New = Iw * T * Wi * p10;

    newRegion.min_x = min(min(p00New[0], p01New[0]), min(p10New[0], p11New[0]));
    newRegion.min_y = min(min(p00New[1], p01New[1]), min(p10New[1], p11New[1]));
    newRegion.max_x = max(max(p00New[0], p01New[0]), max(p10New[0], p11New[0]));
    newRegion.max_y = max(max(p00New[1], p01New[1]), max(p10New[1], p11New[1]));
}

Image *Image::ScaleImage(float scale, bool useBiLinear)
{
    Matrix3f T;
    T.setIdentity();
    T(0, 0) = scale;
    T(1, 1) = scale;

    return transformImage(T, useBiLinear);
}

Image *Image::NonuniformScaleImage(float scaleX, float scaleY, bool useBiLinear)
{
    Matrix3f T;
    T.setIdentity();
    T(0, 0) = scaleX;
    T(1, 1) = scaleY;

    return transformImage(T, useBiLinear);
}

Image *Image::RotateImage(float degrees, bool useBiLinear)
{
    float angleRad = degrees * 3.14159 / 100;
    Matrix3f T;
    T.setIdentity();
    T(0, 0) = cos(angleRad);
    T(0, 1) = -sin(angleRad);
    T(1, 0) = sin(angleRad);
    T(1, 1) = cos(angleRad);

    return transformImage(T, useBiLinear);
}

Image *Image::transformImage(Matrix3f T, bool useBiLinear)
{
    BBox newRegion = _region;
    _pixelUnit = 1;

    Matrix3f Iw = getIndexToWorldMatrix(_height, _region, _pixelUnit);
    Matrix3f Wiold = getWorldToIndexMatrix(Iw);

    transformRegion(Iw, T, Wiold, newRegion);

    unsigned long newWidth = (newRegion.max_x - newRegion.min_x + 1);
    unsigned long newHeight = (newRegion.max_y - newRegion.min_y + 1);

    Image *newImage = new Image(newWidth, newHeight, 1);
    newImage->_region = newRegion;

    Matrix3f IwNew = getIndexToWorldMatrix(newHeight, newRegion, _pixelUnit);
    Matrix3f Wi = getWorldToIndexMatrix(IwNew);
    Matrix3f inversedTransformation = (Wi * T * Iw).inverse();

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

            newImage->_data[y * newWidth + x] = newIntensity;
        }
    }

    return newImage;
}

unsigned char Image::getIntensityNearestBiLinear(Eigen::Vector3f &idx)
{
    int xLess = (int)floor(idx[0]);
    int xMore = (int)ceil(idx[0]);
    int yLess = (int)floor(idx[1]);
    int yMore = (int)ceil(idx[1]);

    Vector3i p0{xLess, yLess, 1};
    Vector3i p1{xMore, yLess, 1};
    Vector3i p2{xLess, yMore, 1};
    Vector3i p3{xMore, yMore, 1};

    unsigned short v0 = static_cast<unsigned short>(getIntensity(p0));
    unsigned short v1 = static_cast<unsigned short>(getIntensity(p1));
    unsigned short v2 = static_cast<unsigned short>(getIntensity(p2));
    unsigned short v3 = static_cast<unsigned short>(getIntensity(p3));

    float x = idx[0] - xLess;
    float y = idx[1] - yLess;

    float w0 = (1 - x) * (1 - y);
    float w1 = x * (1 - y);
    float w2 = (1 - x) * y;
    float w3 = x * y;

    float vs = w0 * v0 + w1 * v1 + w2 * v2 + w3 * v3;
    return static_cast<unsigned char>((int)round(vs));
}

unsigned char Image::getIntensityNearestNeighbor(Eigen::Vector3f &idx)
{
    Vector3i roundedVector{round(idx(0)), round(idx(1)), 1};
    return getIntensity(roundedVector);
}

unsigned char Image::getIntensity(Eigen::Vector3i &idx)
{
    unsigned long index = idx[1] * _width + idx[0];
    if ((idx[0] < 0) || (idx[0] > _width - 1) || (idx[1] < 0) || (idx[1] > _height - 1))
    {
        return 0;
    }
    return _data[index];
}

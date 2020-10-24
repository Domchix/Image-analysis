#ifndef SIMPLE_IMAGE_LIBRARY_HPP
#define SIMPLE_IMAGE_LIBRARY_HPP

#include <string>
#include <tiffio.h>
#include <Eigen/Core>
#include <vector>
#include <iostream>

class Image
{
public:
	struct BBox
	{
		float min_x{0};
		float min_y{0};
		float max_x;
		float max_y;
	};
	Image();
	Image(const Image &);
	Image(std::string filename);
	Image(std::string filename1, std::string filename2, std::string filename3);
	Image(BBox box);
	Image(unsigned int width, unsigned int height, float pixelUnit);
	Image(unsigned int width, unsigned int height, BBox region);

	// Transformation
	Image *ScaleImage(float scale, bool useBiLinear);
	Image *NonuniformScaleImage(float scaleX, float scaleY, bool useBiLinear);
	Image *RotateImage(float degrees, bool useBiLinear);

	// Intensity transformations
	void intensityNegate();
	void intensityPowerLaw(float a, float gamma);
	void contrastStretching(uint16 numberOfSlopeChangePoints, float *slopeChangeFractionPoints,
							float *desiredValueFractionsAtPoints, uint8 algorithm = 0);
	void histogramNormalization();
	// Spacial filtering
	void imageBlurring(uint32 filterWidth);
	void sharpeningUnsharpMask(uint16 blurringFilterWidth, uint8 k = 1);
	void sharpeningLaplacian(bool useN8 = false, bool getOnlyLaplacian = false);
	void sobelOperator();
	void Fig3_43(char imgLetter);

	virtual ~Image();
	// File related
	bool openFile(std::string filename);
	// Image related
	unsigned char *getImageData();
	// Get attributes
	unsigned long getWidth();
	unsigned long getHeight();
	unsigned long getDepth();
	unsigned long getChannels();
	unsigned long getBitsPerSample();
	unsigned long getPixelUnit();
	unsigned long getSamplesPerPixel();
	std::vector<unsigned int> getHistogram();
	BBox getRegion();

private:
	unsigned long _width{0};
	unsigned long _height{0};
	unsigned long _depth{0};
	unsigned long _channels{0};
	unsigned long _bps{0};
	unsigned long _pixelUnit{0};
	float *_lookupTable = new float[256];
	std::vector<unsigned int> _histogram = std::vector<unsigned int>(256, 0);

	unsigned char *_data{nullptr};

	BBox _region;
	// Tiff related stuff
	bool loadTiff(std::string filename);
	bool saveTiff();
	bool readTiffMetaData(TIFF *tiff);
	bool loadTiffTiled(TIFF *tiff);
	bool loadTiffStrip(TIFF *tiff);
	bool loadTiffScanline(TIFF *tiff);
	// Jpeg related stuff
	bool loadJpeg(std::string filename);
	bool loadJpegImageFile(char *lpFilename);
	// Transformation stuff
	Image *transformImage(Eigen::Matrix3f T, bool useBiLinear = false);
	void transformRegion(Eigen::Matrix3f Iw, Eigen::Matrix3f Wi, Eigen::Matrix3f T, BBox &Region);
	unsigned char getIntensityNearestNeighbor(Eigen::Vector3f &idx);
	unsigned char getIntensityNearestBiLinear(Eigen::Vector3f &idx);
	unsigned char getIntensity(Eigen::Vector3i &idx);
	void setIntensity(Eigen::Vector3i &idx, unsigned char intensity);
	// Intensity stuff
	void transformPixels();
	void calculateHistogram();
	// Spacial filtering stuff
};

class Interval
{
public:
	Interval() {}
	Interval(Eigen::Vector2f singleVec, std::string side, uint16 L);
	Interval(Eigen::Vector2f left, Eigen::Vector2f right);

	float linearInterpolation(float x);
	unsigned int threshold(uint32 x);

	Eigen::Vector2f getLeft();
	Eigen::Vector2f getRight();

private:
	Eigen::Vector2f _left, _right;
};
#endif

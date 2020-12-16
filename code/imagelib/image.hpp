#pragma once

#define SIMPLE_IMAGE_LIBRARY_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <tiffio.h> // Note use of libtiff
#include <Eigen/LU>
#include <Eigen/Core>
#include <fftw3.h>

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
	Image(unsigned int width, unsigned int height, float alphaX, float alphaY);
	Image(float alphaX, unsigned int width, unsigned int height);

	// Transformation
	Image *ScaleImage(float scale, bool useBiLinear);
	Image *NonuniformScaleImage(float scaleX, float scaleY, bool useBiLinear);
	Image *RotateImage(float degrees, bool useBiLinear);

	// Intensity transformations
	void intensityNegate();
	void intensityPowerLawInt(float gamma);
	void intensityPowerLawFloat(float gamma);
	void contrastStretching(int nrOfValues, float *values, uint8 algorithm);
	void normalizeHistogram();

	// Fourier transform
	enum FourierStage
	{
		Padded = 0,
		Shifted = 1,
		dft = 2,
		idft = 3,
		Final = 4
	};
	void ApplyFourierTransform(Image::FourierStage stage);
	// Filtering in frequency space
	enum Filter
	{
		Ideal = 0,
		Butterworth = 1,
		Gaussian = 2
	};
	enum FilterType
	{
		Low = 0,
		High = 1
	};
	enum FilterStage
	{
		FilterStandalone = 0,
		FilterApplied = 1,
		FilterAppliedFinal = 2
	};
	void FilterInFrequency(Filter Filter, FilterType type, FilterStage stage, double radius, uint16 n = 0);

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
	unsigned long getImageSize();
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
	float *_fData{nullptr};
	fftw_complex *_complexData{nullptr};

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
	void UpdateIntensityMetadata();
	void remapPixels();
	void updateHistogram();
	// Spacial filtering stuff

	// Fourier transform stuff
	const int REAL = 0;
	const int IMAGINARY = 1;
	void FourierTransform(Image::FourierStage stage);
	void DFT();
	void IDFT();
	void PadImage(float xMult, float yMult);
	void ShiftPeriodicity(bool hideNegative = false);
	void ShiftInversePeriodicity();
	void ComplexToData(float gamma);
	// Image generation
	void generateLineImage(float alphaXMultiplier, float alphaYMultiplier);
	void generateCircleImage(float alphaXMultiplier);
	// Filter in frequency
	float IdealFilter(FilterType type, float D0, float D);
	float ButterworthFilter(FilterType type, float D0, float D, int n);
	float GaussianFilter(FilterType type, float D0, float D);
};

class Interval
{
public:
	Interval() {}
	Interval(Eigen::Vector2f singleVec, std::string side, uint16 L);
	Interval(Eigen::Vector2f left, Eigen::Vector2f right);

	float linearInterpolation(float x);
	unsigned int threshold();

	Eigen::Vector2f getLeft();
	Eigen::Vector2f getRight();

private:
	Eigen::Vector2f _left, _right;
};

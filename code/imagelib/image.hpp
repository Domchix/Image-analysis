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

struct BBox
{
	float min_x{0};
	float min_y{0};
	float max_x;
	float max_y;
};

class Component
{
public:
	Component(){};
	int Label;
	unsigned char Intensity;
	std::vector<uint32> Pixels;
};

class Image
{
public:
	Image();
	Image(const Image &, bool rgb = false);
	Image(std::string filename);
	Image(std::string filename1, std::string filename2, std::string filename3);
	Image(BBox box);
	Image(unsigned int width, unsigned int height, float pixelUnit);
	Image(unsigned int width, unsigned int height, BBox region);
	Image(unsigned int width, unsigned int height, float alphaX, float alphaY);
	Image(float alphaX, unsigned int width, unsigned int height);
	void OutputMetadata();
	void CopyFromImage(const Image &);
	void CopyData(unsigned char *fromData, unsigned char *toData, uint32 size);

	enum Color
	{
		Red = 0,
		Green = 1,
		Blue = 2
	};
	void SetViewToSingleColor(Color color);

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

	//Image Processing
	enum FISHStage
	{
	};
	enum CircuitBoardStage
	{
	};
	enum BottlesStage
	{
		LiquidSegmentation = 0
	};
	const int HIGHLIGHT_INTENSITY = 100;
	void FISHSignalCounts(FISHStage stage);
	const int CIRCUIT_BACKGROUND_INTENSITY = 128;
	const int WIRE_INTENSITY = 64;
	const int MAIN_CENTER = 176;
	void CircuitBoard(CircuitBoardStage stage);
	const int BOTTLENECK_START = 64;
	const int BOTTLENECK_END = 89;
	const int LIQUID_ERROR_BOTTOM = 5;
	const int LIQUID_ERROR_TOP = 20;
	const int SOLDERING_ISLAND_DELTA = 100;
	void Bottles(BottlesStage stage);

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
	unsigned char *_redData{nullptr};
	unsigned char *_greenData{nullptr};
	unsigned char *_blueData{nullptr};
	int *_components{nullptr};
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
	void MedianFilter(uint32 x, uint32 y, int filterWidth);
	void RemoveSaltandPepper();

	//Mathematical Morphology
	const int MAX_INTENSITY = 255;
	const int MIN_INTENSITY = 0;
	void Erosion(unsigned char *data, int XWidth, int YWidth);
	void Dilation(unsigned char *data, int width);
	//Segmentation
	void Treshold(unsigned char *data, int treshold);
	void TresholdReverse(unsigned char *data, int treshold);
	void CCL(unsigned char *data, int *labels, std::vector<Component> &components);
	//void LabelComponent(unsigned char *data, int labelNo, uint32 x, uint32 y);
	void LabelComponent(unsigned char *data, int *labels, Component &component, uint32 x, uint32 y);
	void FillHoles(unsigned char *data, int *labels, int componentToSkip);
	bool ComponentInsideComponent(Component &outsideComponent, Component &insideComponent);
	bool ComponentsIntersect(Component &mainComponent, Component &sideComponent);
	bool ComponentCenteredInsideComponent(Component &mainComponent, Component &insideComponent, int deltaError);
	void GetComponentBoundaries(Component &component, int &minX, int &maxX, int &minY, int &maxY);
	void RemoveSmallComponents(std::vector<Component> &components, unsigned char *data, int size);

	void RemoveWires(unsigned char *data);
	void FillHoles(unsigned char *data, std::vector<Component> &components, std::vector<Component> &holes);
	void FilterComponents(unsigned char *data, std::vector<Component> &components, int fitlerIntensity);
	bool SolderingIslandCorrect(Component &component);
	bool LiquidFilled();

	void SetDataToView(unsigned char *data, int channels);
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

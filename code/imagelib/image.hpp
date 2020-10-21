#ifndef SIMPLE_IMAGE_LIBRARY_HPP
#define SIMPLE_IMAGE_LIBRARY_HPP

#include <string>
#include <tiffio.h>
#include <Eigen/Core>

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
	Image(std::string filename);
	Image(std::string filename1, std::string filename2, std::string filename3);
	Image(BBox box);
	Image(unsigned int width, unsigned int height, float pixelUnit);
	Image(unsigned int width, unsigned int height, BBox region);

	unsigned char getIntensity(Eigen::Vector3i &idx);
	void setIntensity(Eigen::Vector3i &idx, unsigned char intensity);

	Image *transformImage();
	Image *transformImage(Eigen::Matrix3f T, bool useBiLinear = false);

	unsigned char getIntensityNearestNeighbor(Eigen::Vector3f &idx);
	unsigned char getIntensityNearestBiLinear(Eigen::Vector3f &idx);
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
	unsigned long getBPP();
	unsigned long getPixelUnit();
	BBox getRegion();

private:
	unsigned long _width{0};
	unsigned long _height{0};
	unsigned long _depth{0};
	unsigned long _channels{0};
	unsigned long _bpp{0};
	unsigned long _pixelUnit{0};

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
	//Eigen::Matrix3f getWorldToIndexMatrix();
	//Eigen::Matrix3f getWorldToIndexMatrix(Eigen::Matrix3f Iw);
	//Eigen::Matrix3f getIndexToWorldMatrix();
	void transformRegion(Eigen::Matrix3f Iw, Eigen::Matrix3f T, BBox &Region);
};
#endif

#include "image.hpp"

#include <libpng16/png.h>
#include <stdio.h>

bool Image::loadPng(std::string filename){

      std::cout<<"opening png image"<<std::endl;

      //Using C file access
      FILE *fp = fopen(filename.toStdString().c_str(), "rb");
      if (!fp) {
	std::cout<<"Error: couldn't open file: "<<filename.toStdString()<<std::endl;
	return;
      }

      unsigned int headerElm{8};
      unsigned char header[headerElm];

      if (fread(&header, 1, headerElm, fp)!= headerElm) {
	std::cout<<"Error: couldn't read file header: "<<filename.toStdString()<<std::endl;
	fclose(fp);
	return;
      }
      
      unsigned char isPng = !png_sig_cmp(header,0,headerElm);
      if (!isPng) {
	std::cout<<"Error: "<<filename.toStdString()<<" is not png file."<<std::endl;
	fclose(fp);
	return;
      }

      png_structp pngStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING,
						     (png_voidp)nullptr,
						     nullptr,nullptr);

      if (!pngStruct) {
	std::cout<<"Error allocationg png struct."<<std::endl;
	fclose(fp);
	return;
      }

      png_infop pngInfo = png_create_info_struct(pngStruct);						    

      if (!pngInfo) {
	std::cout<<"Error allocationg png info."<<std::endl;
	png_destroy_read_struct(&pngStruct, nullptr, nullptr);
	fclose(fp);
	return;
      }


      fclose(fp);

      return false;
};

#include "qtimageviewer.hpp"

#include <image.hpp>
#include <Eigen/Geometry>

#include <iostream>
#include <string>
#include <cstdlib>
#include <QApplication>
#include <Eigen/Core>
#include <math.h>


int main(int argc, char** argv){
  QApplication app( argc, argv );
  QtImageViewer* imv = new QtImageViewer();

  if(argc >= 2){
    if (argc == 2){
      QString filename(argv[1]);
      std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
      imv->showFile(filename);
    }
    else if (std::string(argv[1]) == "negate"){
      if (argc == 3){
        QString filename(argv[2]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and negating: "<<filename.toStdString()<<std::endl;
        imv->showImage(myImage, std::string(argv[1]));
      }
      else{
        std::cout << "Invalid number of arguments for operation 'negate'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "powerlaw"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and applying power law: "<<filename.toStdString()<<std::endl;
        float* values = new float[1]{atof(argv[2])};
        imv->showImage(myImage, std::string(argv[1]), values);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'powerlaw'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "contrastlinear" || std::string(argv[1]) == "contrastthreshold"
     || std::string(argv[1]) == "contrastslice"){

      if ((argc-3) % 2 != 0){
        std::cout << "number of fraction values must be even (x and y pairs)";
        std::exit(0);
      }
      QString filename(argv[argc-1]);
      Image* myImage = new Image(filename.toStdString());
      std::cout<<"Loading and streaching contrast: "<<filename.toStdString()<<std::endl;
      
      float* values = new float[argc - 3];
      int nrOfValues = 0;
      for (unsigned short i = 2; i < argc-1; i++){
        if (atof(argv[i]) > 1 || atof(argv[i]) < 0){
          std::cout << "values must be fractions between 0 and 1 inclusively" << std::endl;
          std::exit(0);
        }
        values[i-2] = atof(argv[i]);
        nrOfValues++;
      }
      imv->showImage(myImage, std::string(argv[1]), values, nrOfValues);
    }
    else if (std::string(argv[1]) == "normalize"){
      if (argc == 3){
        QString filename(argv[2]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and normalizing histogram: "<<filename.toStdString()<<std::endl;
        imv->showImage(myImage, std::string(argv[1]));
      }
      else{
        std::cout << "Invalid number of arguments for operation 'normalize'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "blur"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and blurring: "<<filename.toStdString()<<std::endl;
        float* values = new float[1]{atoi(argv[2])};
        imv->showImage(myImage, std::string(argv[1]), values);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'blur'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "unsharpmask"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and sharpening with unsharp mask: "<<filename.toStdString()<<std::endl;
        float* values = new float[1]{atoi(argv[2])};
        imv->showImage(myImage, std::string(argv[1]), values);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'uncharpmask'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "laplacian"){
      if (argc == 4){
        QString filename(argv[3]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and sharpening with laplacian: "<<filename.toStdString()<<std::endl;
        float* values = new float[1];
        if (std::string(argv[2]) == "n4") values[0] = 0;
        else if (std::string(argv[2]) == "n8") values[0] = 1;
        else{
          std::cout << "third argument must be 'n4' or 'n8' for neighbourhood scheme" << std::endl;
          std::exit(0);
        }
        imv->showImage(myImage, std::string(argv[1]), values);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'laplacian'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "sobel"){
      if (argc == 3){
        QString filename(argv[2]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and visualizing sobel gradient magnitude: "<<filename.toStdString()<<std::endl;
        imv->showImage(myImage, std::string(argv[1]));
      }
      else{
        std::cout << "Invalid number of arguments for operation sobel'" << std::endl;
        std::exit(0);
      }
    }
    else if (std::string(argv[1]) == "fig3-43"){
      if (argc == 5){
        QString filename(argv[4]);
        Image* myImage = new Image(filename.toStdString());
        std::cout<<"Loading and visualizing figure 3-43: "<<filename.toStdString()<<std::endl;
        imv->showImage(myImage, argv[2][0], argv[3][0]);
      }
      else{
        std::cout << "Invalid number of arguments for operation 'fig3-43'" << std::endl;
        std::exit(0);
      }
    }
  }
  imv->show();
  imv->resize(1000,600);

  int ret = app.exec();
  
  delete(imv);
  //delete(tImg);

  return ret;
  
}

#include "qtimageviewer.hpp"

#include <image.hpp>
#include <Eigen/Geometry>

#include <iostream>
#include <QApplication>

int main(int argc, char** argv){
  QApplication app( argc, argv );
  QtImageViewer* imv = new QtImageViewer();

  if(argc >= 2){
    QString filename(argv[1]);
    std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
    imv->showFile(filename);
  }

  // if (argc != 2) {
  //   std::cout<<"add filename of image to rotate"<<std::endl;
  // }

  // QString filename(argv[1]);

  // imv->showImage(tImg);

  // BBox bbox(2500.0f-1,2500.0f-1);
  // Image* img = new Image(505,500,bbox);
  // for(unsigned int yIter = 0; yIter < img->getHeight(); yIter++){
  //   for (unsigned int xIter = 0; xIter < img->getWidth(); xIter++) {

  //     float x = static_cast<float>(xIter) - img->getWidth()/2;
  //     float y = static_cast<float>(yIter) - img->getHeight()/2;

  //     img->setPixel(xIter, yIter, 0.005*x*x+0.005*y*y);
  //   }
  // }
  // imv->showImage(img);

  imv->show();
  imv->resize(1000,600);

  int ret = app.exec();
  
  delete(imv);
  //delete(tImg);

  return ret;
  
}

#include "qtimageviewer.hpp"

#include <iostream>
#include <QApplication>

int main(int argc, char** argv){

  // Start Qt framework
  QApplication app( argc, argv );

  QtImageViewer* imv = new QtImageViewer();
  // Optionally give image on command line
  if(argc == 2){
    QString filename(argv[1]);
    std::cout<<"Load directly: "<<filename.toStdString()<<std::endl;
    //imv->showFile(filename);
    imv->showFile(filename);
    imv->transformFile(filename);
  } else if(argc == 4){
    //Image combination
    std::cout<<"Load 3 files"<<std::endl;
    QString filename1(argv[1]);
    QString filename2(argv[2]);
    QString filename3(argv[3]);
    imv->combineFiles(filename1, filename2, filename3);
  }
  // Draw our GUI
  imv->show();
  imv->resize(1000,600);

  // Run Qt application until someone presses quit
  int ret = app.exec();

  delete(imv);
  return ret;
}

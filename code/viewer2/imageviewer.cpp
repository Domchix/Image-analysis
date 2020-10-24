#include "qtimageviewer.hpp"

#include <image.hpp>
#include <Eigen/Geometry>

#include <iostream>
#include <string>
#include <cstdlib>
#include <QApplication>
#include <Eigen/Core>

using std::cout;
using std::endl;

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QtImageViewer *imv = new QtImageViewer();

  if (argc >= 2)
  {
    if (argc == 2)
    {
      QString filename(argv[1]);
      cout << "Load directly: " << filename.toStdString() << endl;
      imv->showFile(filename);
    }
    else if (std::string(argv[1]) == "power")
    {
      if (argc != 4)
      {
        cout << "Invalid number of arguments" << endl;
      }
      else
      {
        QString filename(argv[2]);
        Image *myImage = new Image(filename.toStdString());
        std::cout << "Loading and applying power law: " << filename.toStdString() << std::endl;
        float *gamm = new float[1]{atof(argv[3])};
        imv->showImage(myImage, 0, gamm);
      }
    }
    else if (std::string(argv[1]) == "linear" || std::string(argv[1]) == "threshold")
    {
      if ((argc - 3) % 2 != 0)
      {
        std::cout << "lines must be even (x and y pairs)";
        return 0;
      }
      QString filename(argv[2]);
      Image *myImage = new Image(filename.toStdString());
      std::cout << "Loading and streaching contrast: " << filename.toStdString() << std::endl;

      float *values = new float[argc - 3];
      int nrOfValues = 0;
      for (unsigned short i = 3; i < argc; i++)
      {
        float value = atof(argv[i]);
        if (value > 1 || value < 0)
        {
          std::cout << "values must be fractions between 0 and 1 inclusively" << std::endl;
          return 0;
        }
        values[i - 3] = value;
        nrOfValues++;
      }
      int method;
      if (argv[1] == "contrastlinear")
      {
        method = 1;
      }
      else
      {
        method = 2;
      }
      imv->showImage(myImage, method, values, nrOfValues);
    }
    else if (std::string(argv[1]) == "normalize")
    {
      if (argc == 3)
      {
        QString filename(argv[2]);
        Image *myImage = new Image(filename.toStdString());
        std::cout << "Loading and normalizing histogram: " << filename.toStdString() << std::endl;
        imv->showImage(myImage, 3);
      }
      else
      {
        std::cout << "Invalid number of arguments for operation 'normalize'" << std::endl;
        std::exit(0);
      }
    }
  }
  imv->show();
  imv->resize(1000, 600);

  int ret = app.exec();

  delete (imv);

  return ret;
}

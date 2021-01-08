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

void InvalidArguments();
bool HandleInput(int argc, char **argv, QApplication *app, QtImageViewer *imv);

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  QtImageViewer *imv = new QtImageViewer();

  if (HandleInput(argc, argv, &app, imv) == false)
  {
    return 0;
  }

  imv->show();
  imv->resize(1000, 600);

  int ret = app.exec();

  delete (imv);

  return ret;
}

void InvalidArguments()
{
  std::cout << "Invalid number of arguments" << std::endl;
  std::exit(0);
}

bool HandleInput(int argc, char **argv, QApplication *app, QtImageViewer *imv)
{
  if (argc >= 2)
  {
    if (argc == 2)
    {
      QString filename(argv[1]);
      cout << "Load directly: " << filename.toStdString() << endl;
      imv->showFile(filename);
      return true;
    }

    std::string command = std::string(argv[1]);

    if (command == "fgenerate")
    {
      if (argc != 6 && argc != 5)
      {
        InvalidArguments();
        return false;
      }
      if (argc == 5)
      {
        imv->showImage(atoi(argv[2]), atoi(argv[3]), atof(argv[4]));
        return true;
      }
      imv->showImage(atoi(argv[2]), atoi(argv[3]), atof(argv[4]), atof(argv[5]));
      return true;
    }
    else if (command == "fish")
    {
      if (argc != 5 && argc != 6)
      {
        InvalidArguments();
        return false;
      }

      QString file1(argv[2]);
      QString file2(argv[3]);
      QString file3(argv[4]);
      Image *myImage = new Image(file1.toStdString(), file2.toStdString(), file3.toStdString());
      int stage = argc == 6 ? atoi(argv[5]) : 0;

      imv->showImage(myImage, (Image::FISHStage)stage);
      return true;
    }

    QString filename(argv[2]);
    Image *myImage = new Image(filename.toStdString());

    if (command == "power")
    {
      if (argc != 4)
      {
        InvalidArguments();
        return false;
      }
      std::cout << "Loading and applying power law: " << filename.toStdString() << std::endl;

      float *gamm = new float[1]{atof(argv[3])};
      imv->showImage(myImage, 0, gamm);
    }
    else if (command == "linear" || command == "threshold")
    {
      if ((argc - 3) % 2 != 0)
      {
        std::cout << "lines must be even (x and y pairs)";
        InvalidArguments();
        return false;
      }
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
    else if (command == "normalize")
    {
      if (argc != 3)
      {
        InvalidArguments();
        return false;
      }
      std::cout << "Loading and normalizing histogram: " << filename.toStdString() << std::endl;

      imv->showImage(myImage, 3);
    }
    else if (command == "ftransform")
    {
      if (argc != 4)
      {
        InvalidArguments();
        return false;
      }

      imv->showImage(myImage, (Image::FourierStage)atoi(argv[3]));
    }
    else if (command == "ffilter")
    {
      if (argc != 7 && argc != 8)
      {
        InvalidArguments();
        return false;
      }

      int n = argc == 8 ? atoi(argv[7]) : 0;

      imv->showImage(myImage, (Image::Filter)atoi(argv[3]), (Image::FilterType)atoi(argv[4]), (Image::FilterStage)atoi(argv[5]), atof(argv[6]), n);
    }
    else if (command == "circuit")
    {
      if (argc != 2 && argc != 3)
      {
        InvalidArguments();
        return false;
      }

      int stage = argc == 3 ? atoi(argv[2]) : 0;

      imv->showImage(myImage, (Image::CircuitBoardStage)stage);
    }
    else if (command == "bottles")
    {
      if (argc != 3 && argc != 4)
      {
        InvalidArguments();
        return false;
      }

      int stage = argc == 4 ? atoi(argv[3]) : 0;

      imv->showImage(myImage, (Image::BottlesStage)stage);
    }
  }

  return true;
}

#include "qtimageviewer.hpp"

#include <QFileDialog>
#include <QPalette>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QInputDialog>

#include <iostream>

#include <image.hpp> /// include your imagelibrary

QtImageViewer::QtImageViewer(QWidget *parent) : QMainWindow(parent)
{
  init();
};

void QtImageViewer::init()
{
  _useBiLinear = false;

  _qImageLabel = new QLabel;

  _qImageLabel->setBackgroundRole(QPalette::Base);
  _qImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  _qImageLabel->setScaledContents(false);

  _qScrollArea = new QScrollArea;
  _qScrollArea->setBackgroundRole(QPalette::Dark);
  _qScrollArea->setWidget(_qImageLabel);
  _qScrollArea->setVisible(true);

  setCentralWidget(_qScrollArea);
  createActions();
}

QtImageViewer::~QtImageViewer()
{
  delete (_qImageLabel);
  delete (_qScrollArea);
  delete (_fileMenu);
  delete (_fileOpenAction);
  delete (_quitAction);
};

void QtImageViewer::createActions()
{
  _fileOpenAction = new QAction(tr("&Open..."), this);
  _fileOpenAction->setShortcut(QKeySequence::Open);
  connect(_fileOpenAction, SIGNAL(triggered()), this, SLOT(openFile()));

  _quitAction = new QAction(tr("&Quit..."), this);
  _quitAction->setShortcut(QKeySequence::Quit);
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(quit()));

  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_fileOpenAction);
  _fileMenu->addAction(_quitAction);

  _scaleAction = new QAction(tr("&Scale"), this);
  connect(_scaleAction, SIGNAL(triggered()), this, SLOT(scaleImage()));
  _nonuniformScaleAction = new QAction(tr("&Nonuniform Scale"), this);
  connect(_nonuniformScaleAction, SIGNAL(triggered()), this, SLOT(nonuniformScaleImage()));
  _rotateAction = new QAction(tr("&Rotate"), this);
  connect(_rotateAction, SIGNAL(triggered()), this, SLOT(rotateImage()));
  _interpolationAction = new QAction(tr("&Set Interpolation"), this);
  connect(_interpolationAction, SIGNAL(triggered()), this, SLOT(setInterpolation()));

  _editMenu = menuBar()->addMenu(tr("&Edit"));
  _editMenu->addAction(_scaleAction);
  _editMenu->addAction(_nonuniformScaleAction);
  _editMenu->addAction(_rotateAction);
  _editMenu->addAction(_interpolationAction);
}

void QtImageViewer::quit()
{
  close();
};

void QtImageViewer::openFile()
{
  QFileDialog dialog(this, tr("Open File"));
  QString filename = dialog.getOpenFileName(this, "Select image to open");
  std::cout << "Opening: " << filename.toStdString() << std::endl;
  showFile(filename);
}

void QtImageViewer::showFile(const QString filename)
{
  _currentImage = new Image(filename.toStdString()); // Load using your library!
  showImage();
}

void QtImageViewer::showImage()
{
  std::cout << "---" << std::endl;
  std::cout << "Width: " << _currentImage->getWidth() << std::endl;
  std::cout << "Height: " << _currentImage->getHeight() << std::endl;
  std::cout << "Channels: " << _currentImage->getChannels() << std::endl;
  std::cout << "BPS: " << _currentImage->getBitsPerSample() << std::endl;
  std::cout << "Depth: " << _currentImage->getDepth() << std::endl;
  std::cout << "---" << std::endl;

  QImage::Format format = QImage::Format_Invalid;
  // We deal only with 8 bits per channel for now.
  // Check for RGB vs Grayscale
  if (_currentImage->getChannels() == 3)
    format = QImage::Format_RGB888;
  else if (_currentImage->getChannels() == 1)
    format = QImage::Format_Grayscale8;

  QImage qImg(_currentImage->getImageData(),
              _currentImage->getWidth(),
              _currentImage->getHeight(),
              _currentImage->getWidth() * _currentImage->getChannels() * _currentImage->getBitsPerSample() / 8,
              format);

  _qImageLabel->setPixmap(QPixmap::fromImage(qImg));
  _qImageLabel->resize(_qImageLabel->pixmap()->size());
  _qScrollArea->setVisible(true);

  update(); /// Force Qt to redraw
}

void QtImageViewer::scaleImage()
{
  bool result;
  double scale = QInputDialog::getDouble(this, tr("Scale image"), tr("Scale"), 0, -10000, 10000, 2, &result);

  if (!result)
  {
    return;
  }

  _currentImage = _currentImage->ScaleImage(scale, _useBiLinear);
  showImage();
}

void QtImageViewer::nonuniformScaleImage()
{
  bool result;
  double scaleX = QInputDialog::getDouble(this, tr("Nonuniform scale image"), tr("Scale x"), 0, -10000, 10000, 2, &result);
  double scaleY = QInputDialog::getDouble(this, tr("Nonuniform scale image"), tr("Scale y"), 0, -10000, 10000, 2, &result);

  if (!result)
  {
    return;
  }

  _currentImage = _currentImage->NonuniformScaleImage(scaleX, scaleY, _useBiLinear);
  showImage();
}

void QtImageViewer::rotateImage()
{
  bool result;
  double degrees = QInputDialog::getDouble(this, tr("Rotate image"), tr("Rotate degrees"), 0, -10000, 10000, 2, &result);

  if (!result)
  {
    return;
  }

  _currentImage = _currentImage->RotateImage(degrees, _useBiLinear);
  showImage();
}

void QtImageViewer::setInterpolation()
{
  bool result;
  int interpolation = QInputDialog::getInt(this, tr("Set Interpolation"), tr("0-NN 1-BiLinear"), _useBiLinear, 0, 1, 1, &result);
  
  if (!result)
  {
    return;
  }

  _useBiLinear = interpolation;
}

void QtImageViewer::combineFiles(const QString filename1, const QString filename2, const QString filename3)
{
  _currentImage = new Image(filename1.toStdString(), filename2.toStdString(), filename3.toStdString());
  showImage();
}
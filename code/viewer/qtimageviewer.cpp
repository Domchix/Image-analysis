#include "qtimageviewer.hpp"

#include <QFileDialog>
#include <QPalette>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>

#include <iostream>

#include <image.hpp> /// include your imagelibrary

QtImageViewer::QtImageViewer(QWidget *parent) : QMainWindow(parent)
{
  init();
};

void QtImageViewer::init()
{
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
  connect(_quitAction, SIGNAL(triggetu gred()), this, SLOT(quit()));

  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_fileOpenAction);
  _fileMenu->addAction(_quitAction);
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
  std::cout << "Width: " << _currentImage->getWidth() << std::endl;
  std::cout << "Height: " << _currentImage->getHeight() << std::endl;
  std::cout << "Channels: " << _currentImage->getChannels() << std::endl;
  std::cout << "BPP: " << _currentImage->getBPP() << std::endl;
  std::cout << "Depth: " << _currentImage->getDepth() << std::endl;

  QImage::Format format = QImage::Format_Invalid;
  // We deal only with 8 bits per channel for now.
  // Check for RGB vs Grayscale
  if (_currentImage->getChannels() == 3)
    format = QImage::Format_RGB888;
  else if (_currentImage->getChannels() == 1)
    format = QImage::Format_Grayscale8;

  // QImage qImg(_currentImage->getImageData(),
  //             _currentImage->getWidth(),
  //             _currentImage->getHeight(),
  //             format);
  QImage qImg(_currentImage->getImageData(),
              _currentImage->getWidth(),
              _currentImage->getHeight(),
              _currentImage->getWidth() * _currentImage->getChannels() * _currentImage->getBPP() / 8,
              format);

  _qImageLabel->setPixmap(QPixmap::fromImage(qImg));
  _qImageLabel->resize(_qImageLabel->pixmap()->size());
  _qScrollArea->setVisible(true);

  update(); /// Force Qt to redraw
  //delete (myImage); /// Data is copied to Qt, we can delete our image.
}

void QtImageViewer::transformFile(const QString filename)
{
  //_currentImage = new Image(filename.toStdString());
  _currentImage = _currentImage->transformImage();
  showImage();
}

void QtImageViewer::combineFiles(const QString filename1, const QString filename2, const QString filename3)
{
  _currentImage = new Image(filename1.toStdString(), filename2.toStdString(), filename3.toStdString());
  showImage();
}
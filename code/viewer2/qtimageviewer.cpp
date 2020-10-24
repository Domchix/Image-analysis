#include "qtimageviewer.hpp"

#include <QFileDialog>
#include <QMainWindow>
#include <QPalette>
#include <QMenu>
#include <QMenuBar>
#include <QImageReader>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>

#include <iostream>

QtImageViewer::QtImageViewer(QWidget *parent) :
  QMainWindow(parent){
  init();
};

void  QtImageViewer::init(){

  _lImageLabel = new QLabel;
  _lImageLabel->setBackgroundRole(QPalette::Base);
  _lImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  _lImageLabel->setScaledContents(false);
  
  _lScrollArea = new QScrollArea;
  _lScrollArea->setBackgroundRole(QPalette::Dark);
  _lScrollArea->setWidget(_lImageLabel);
  _lScrollArea->setVisible(true);

  _rImageLabel = new QLabel;
  _rImageLabel->setBackgroundRole(QPalette::Base);
  _rImageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  _rImageLabel->setScaledContents(false);
  
  _rScrollArea = new QScrollArea;
  _rScrollArea->setBackgroundRole(QPalette::Dark);
  _rScrollArea->setWidget(_rImageLabel);
  _rScrollArea->setVisible(true);
  

  _mainSplitter = new QSplitter();
  _leftSplitter = new QSplitter(Qt::Vertical);
  _rightSplitter = new QSplitter(Qt::Vertical);

  _mainSplitter->addWidget(_leftSplitter);
  _mainSplitter->addWidget(_rightSplitter);

  _leftSplitter->addWidget(_lScrollArea);
  _rightSplitter->addWidget(_rScrollArea);

  setCentralWidget(_mainSplitter);
  createActions();
}

QtImageViewer::~QtImageViewer(){
  delete(_lImageLabel);
  delete(_lScrollArea);
  delete(_rImageLabel);
  delete(_rScrollArea);
  delete(_fileMenu);
  delete(_fileOpenAction);
  delete(_quitAction);
};

void QtImageViewer::createActions(){

  _fileOpenAction = new QAction(tr("&Open..."), this);
  _fileOpenAction->setShortcut(QKeySequence::Open);
  connect(_fileOpenAction, SIGNAL(triggered()), this, SLOT(openFile()));

  _quitAction = new QAction(tr("&Quit..."), this);
  _quitAction->setShortcut(QKeySequence::Quit);
  connect(_quitAction, SIGNAL(triggered()), this, SLOT(quit()));

  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(_fileOpenAction);
  _fileMenu->addAction(_quitAction);
}

void QtImageViewer::openFile(){
  QFileDialog dialog(this, tr("Open File"));
  QString filename = dialog.getOpenFileName(this, "Select image to open");
  std::cout<<"Opening: "<<filename.toStdString()<<std::endl;
  showFile(filename);
}

void QtImageViewer::showFile(const QString filename){  

  Image* myImage = new Image(filename.toStdString());
  showImage(myImage);

  update(); // For Qt to redraw with new image
  delete(myImage);
}

void QtImageViewer::showImage(Image *img){
  std::cout<<"Transform and show Image! "<<std::endl;
  //create a copy
  
  // img->Fig3_43('f');
  showImageLeft(img);
  Image *copy = new Image(*(img));
  // transform copy
  // copy->intensityNegate();
  // copy->intensityPowerLaw(1, 0.3);

  // uint16 n = 2;
  // float xpoints[n] = {0.375f, 0.625f};
  // float ypoints[n] = {0.125f, 0.875f};

  // uint16 n = 2;
  // float xpoints[n] = {0.9f, 1.0f};
  // float ypoints[n] = {0.3f, 0.0f};

  // copy->contrastStretching(n, xpoints, ypoints, 2);

  // copy->histogramNormalization();

  // copy->imageBlurring(5);

  // copy->sharpeningUnsharpMask(5, 1);

  // copy->sharpeningLaplacian(true);

  // copy->sobelOperator();

  //copy->Fig3_43('g');

  // show copy
  showImageRight(copy); 
  delete(copy); // copy not needed anymore
}

void QtImageViewer::showImage(Image *img, std::string transformationType, float* values, int nrOfValues){
  std::cout<<"Transform and show Image! "<<std::endl;
  showImageLeft(img);
  Image *copy = new Image(*(img));
  if (transformationType == "negate"){
    copy->intensityNegate();
  }
  else if (transformationType == "powerlaw"){
    copy->intensityPowerLaw(values[0]);
  }
  else if (transformationType == "contrastlinear" || transformationType == "contrastthreshold"
   || transformationType == "contrastslice"){
    float* xpoints = new float[nrOfValues / 2];
    float* ypoints = new float[nrOfValues / 2];
    int j = 0;
    for (int i = 0; i < nrOfValues; i++){
      if (i % 2 == 0) xpoints[j] = values[i];
      else{
        ypoints[j] = values[i];
        j++;
      } 
    }
    if (transformationType == "contrastlinear") copy->contrastStretching(nrOfValues / 2, xpoints, ypoints, 0);
    else if (transformationType == "contrastthreshold") copy->contrastStretching(nrOfValues / 2, xpoints, ypoints, 1);
    else copy->contrastStretching(nrOfValues / 2, xpoints, ypoints, 2);
  }
  else if (transformationType == "normalize"){
    copy->normalizeHistogram();
  }
  else if (transformationType == "blur"){
    copy->imageBlurring((int)values[0]);
  }
  else if (transformationType == "unsharpmask"){
    copy->sharpeningUnsharpMask((int)values[0], 1);
  }
  else if (transformationType == "laplacian"){
    if (values[0] == 0) copy->sharpeningLaplacian(false);
    else copy->sharpeningLaplacian(true);
  }
  else if (transformationType == "sobel"){
    copy->sobelOperator();
  }

  showImageRight(copy);
  delete(copy);

  update(); // For Qt to redraw with new image
  delete(img);
}

void QtImageViewer::showImage(Image *img, char left, char right){
  std::cout<<"Transform and show Image! "<<std::endl;
  //create a copy
  Image *copy = new Image(*(img));
  img->Fig3_43(left);
  showImageLeft(img);

  copy->Fig3_43(right);
  showImageRight(copy);
  delete(copy); // copy not needed anymore
}

void QtImageViewer::showImageLeft(Image *img) {

  QImage::Format format = QImage::Format_Invalid;

  if(img->getSamplesPerPixel() == 3){
    format = QImage::Format_RGB888;
    std::cout<<"Sorry, only dealing with grayscale images for now"<<std::endl;
    close();
  }
  else if (img->getSamplesPerPixel() == 1)
    format = QImage::Format_Grayscale8;

  std::vector<unsigned int> hist = img->getHistogram();

  QtCharts::QBarSet *bset = new QtCharts::QBarSet("Intensities");
  unsigned int maxInt{0};
  for(unsigned int i : hist) { (*bset) << i; maxInt = std::max(maxInt,i);  }
  
  QtCharts::QBarSeries *bseries = new QtCharts::QBarSeries();
  bseries->append(bset);

  QtCharts::QChart *chart = new QtCharts::QChart();
  chart->addSeries(bseries);
  chart->setTitle("Intensity Histogram");
  chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
  

  QtCharts::QValueAxis *xAxis = new  QtCharts::QValueAxis();
  xAxis->setRange(0,255);
  
  QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
  yAxis->setRange(0,maxInt);

  // Seems you must add axis to chart first, then to bseries
  chart->addAxis(xAxis, Qt::AlignBottom);
  chart->addAxis(yAxis, Qt::AlignLeft);

  bseries->attachAxis(xAxis);
  bseries->attachAxis(yAxis);
  bset->setBorderColor(QColor("black"));

  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);
  
  _lChartView = new QtCharts::QChartView(chart);
  _lChartView->setRenderHint(QPainter::Antialiasing);
  _leftSplitter->addWidget(_lChartView);

  // Copy image data to Qt
  QImage qImg(img->getImageData(),
	      img->getWidth(),
	      img->getHeight(),
	      img->getWidth()*img->getSamplesPerPixel()*img->getBitsPerSample()/8,
	      format);
  
  // Tell Qt to show this image data
  _lImageLabel->setPixmap(QPixmap::fromImage(qImg));
  _lImageLabel->resize(_lImageLabel->pixmap()->size());
  _lScrollArea->setVisible(true);
  
  update(); // For Qt to redraw with new image
}

void QtImageViewer::showImageRight(Image *img) {

  QImage::Format format = QImage::Format_Invalid;

  if(img->getSamplesPerPixel() == 3){
    format = QImage::Format_RGB888;
    std::cout<<"Sorry, only dealing with grayscale images for now"<<std::endl;
    close();
  }
  else if (img->getSamplesPerPixel() == 1)
    format = QImage::Format_Grayscale8;

  std::vector<unsigned int> hist = img->getHistogram();

  QtCharts::QBarSet *bset = new QtCharts::QBarSet("Intensities");
  unsigned int maxInt{0};
  for(unsigned int i : hist) { (*bset) << i; maxInt = std::max(maxInt,i);  }
  
  QtCharts::QBarSeries *bseries = new QtCharts::QBarSeries();
  bseries->append(bset);

  QtCharts::QChart *chart = new QtCharts::QChart();
  chart->addSeries(bseries);
  chart->setTitle("Intensity Histogram");
  chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
  
  QtCharts::QValueAxis *xAxis = new  QtCharts::QValueAxis();
  xAxis->setRange(0,255);
  
  QtCharts::QValueAxis *yAxis = new QtCharts::QValueAxis();
  yAxis->setRange(0,maxInt);

  // Seems you must add axis to chart first, then to bseries
  chart->addAxis(xAxis, Qt::AlignBottom);
  chart->addAxis(yAxis, Qt::AlignLeft);

  bseries->attachAxis(xAxis);
  bseries->attachAxis(yAxis);
  bset->setBorderColor(QColor("black"));

  chart->legend()->setVisible(true);
  chart->legend()->setAlignment(Qt::AlignBottom);
  
  _rChartView = new QtCharts::QChartView(chart);
  _rChartView->setRenderHint(QPainter::Antialiasing);
  _rightSplitter->addWidget(_rChartView);

  // Copy image data to Qt
  QImage qImg(img->getImageData(),
	      img->getWidth(),
	      img->getHeight(),
	      img->getWidth()*img->getSamplesPerPixel()*img->getBitsPerSample()/8,
	      format);
  
  // Tell Qt to show this image data
  _rImageLabel->setPixmap(QPixmap::fromImage(qImg));
  _rImageLabel->resize(_rImageLabel->pixmap()->size());
  _rScrollArea->setVisible(true);
  update(); // For Qt to redraw with new image
}

void QtImageViewer::quit(){
  close();
};

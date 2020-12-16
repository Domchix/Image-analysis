#ifndef QT_IMAGE_VIEWER_MAINWINDOW_HPP
#define QT_IMAGE_VIEWER_MAINWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QScrollArea>

#include <QSplitter>
#include <QChartView>

#include <image.hpp>

class QtImageViewer : public QMainWindow
{

  Q_OBJECT

public:
  QtImageViewer(QWidget *parent = nullptr);
  QtImageViewer(const QString filename, QWidget *parent = nullptr);

  virtual ~QtImageViewer();

public slots:
  void showFile(const QString filename);
  void showImage(int width, int height, float alphaX, float alphaY);
  void showImage(int width, int height, float alphaX);
  void showImage(Image *img);
  void showImage(Image *img, Image::FourierStage stage);
  void showImage(Image *img, int transformation, float *values = nullptr, int nrOfValues = 0);
  void showImage(Image *img, Image::Filter filter, Image::FilterType type, Image::FilterStage stage, double radius, int n);
  void showImageLeft(Image *img);
  void showImageRight(Image *img);
  void openFile();
  void quit();

private:
  void init();
  void createActions();
  void createMenus();

  QImage _lImage;
  QLabel *_lImageLabel{nullptr};
  QScrollArea *_lScrollArea{nullptr};

  QImage _rImage;
  QLabel *_rImageLabel{nullptr};
  QScrollArea *_rScrollArea{nullptr};

  QMenu *_fileMenu{nullptr};

  QAction *_fileOpenAction{nullptr};
  QAction *_quitAction{nullptr};

  QSplitter *_mainSplitter{nullptr};
  QSplitter *_leftSplitter{nullptr};
  QSplitter *_rightSplitter{nullptr};
  QtCharts::QChartView *_lChartView{nullptr};
  QtCharts::QChartView *_rChartView{nullptr};
};

#endif

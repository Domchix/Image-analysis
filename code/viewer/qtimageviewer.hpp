#ifndef QT_IMAGE_VIEWER_MAINWINDOW_HPP
#define QT_IMAGE_VIEWER_MAINWINDOW_HPP

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QScrollArea>

#include <image.hpp>

class QtImageViewer : public QMainWindow{
  Q_OBJECT /// Qt Syntax. Must be included.
public:
  QtImageViewer(QWidget *parent=nullptr);
  virtual ~QtImageViewer();

public slots: /// Qt Syntax Signal Slot mechanism.
  void showFile(const QString filename);
  void combineFiles(const QString filename1, const QString filename2, const QString filename3);
  void transformFile(const QString filename);
  void openFile();
  void showImage();
  void quit();

private:
  void init();
  void createActions();
  void createMenus();
  Image* _currentImage;
  // Qt Image related
  QImage _qImage;
  QLabel *_qImageLabel{nullptr};
  QScrollArea *_qScrollArea{nullptr};
  // Qt interface related
  QMenu *_fileMenu{nullptr};
  QAction *_fileOpenAction{nullptr};
  QAction *_quitAction{nullptr};
  
};
#endif

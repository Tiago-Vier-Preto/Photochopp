#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QImage>
#include <QInputDialog>
#if defined(QT_PRINTSUPPORT_LIB)
#  include <QtPrintSupport/qtprintsupportglobal.h>

#  if QT_CONFIG(printer)
#    include <QPrinter>
#  endif
#endif

QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

//! [0]
class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = nullptr);
    bool loadFile(const QString &);

public slots:
    void convolution(const  std::vector<std::vector<float>> &kernel);

private slots:
    void open();
    void saveAs();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void about();

private:
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scale();
    void flipHorizontally();
    void flipVertically();
    void convertToGrayScale();
    void grayScaleQuantization();
    void grayScaleHistogram();
    void resetImage();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void brightness();
    void contrast();
    void negative();
    void rotateLeft();
    void rotateRight();
    void histogramEqualization();
    void grayScaleHistogramMatching();
    void showConvWindow();

    QImage image;
    QImage resultImage;
    QLabel *imageLabel;
    QLabel *resultLabel;
    QScrollArea *scrollArea;
    QScrollArea *scrollAreaResult;
    double scaleFactor = 1;

#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    QPrinter printer;
#endif

    QAction *saveAsAct;
    QAction *copyAct;
    QAction *flipHorizontallyAct;
    QAction *flipVerticallyAct;
    QAction *convertToGrayScaleAct;
    QAction *grayScaleQuantizationAct;
    QAction *resetImageAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *histogramAct;
    QAction *brightnessAct;
    QAction *contrastAct;
    QAction *negativeAct;
    QAction *rotateLeftAct;
    QAction *rotateRightAct;
    QAction *histogramEqualizationAct;
    QAction *grayScaleHistogramMatchingAct;
    QAction *conv2dAct;
    QAction *showConvWindowAct;

};

#endif

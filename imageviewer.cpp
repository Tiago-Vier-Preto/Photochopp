#include "imageviewer.h"

#include <QApplication>
#include <QClipboard>
#include <QColorSpace>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QGroupBox>
#include <cstring>

#if defined(QT_PRINTSUPPORT_LIB)
#  include <QtPrintSupport/qtprintsupportglobal.h>

#  if QT_CONFIG(printdialog)
#    include <QPrintDialog>
#  endif
#endif

ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent), imageLabel(new QLabel), resultLabel(new QLabel)
    , scrollArea(new QScrollArea), scrollAreaResult(new QScrollArea)
{

    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);
    imageLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setBackgroundRole(QPalette::Base);
    resultLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    resultLabel->setScaledContents(true);

    QGroupBox *originalBox = new QGroupBox(tr("Original Image"));
    QGroupBox *processedBox = new QGroupBox(tr("Processed Image"));

    QScrollArea *scrollAreaOriginal = new QScrollArea;
    QScrollArea *scrollAreaProcessed = new QScrollArea;

    scrollAreaOriginal->setWidget(imageLabel);
    scrollAreaOriginal->setAlignment(Qt::AlignCenter);
    scrollAreaOriginal->setWidgetResizable(false);

    scrollAreaProcessed->setWidget(resultLabel);
    scrollAreaProcessed->setAlignment(Qt::AlignCenter);
    scrollAreaProcessed->setWidgetResizable(false);

    // Defines the layout for the group boxes
    QVBoxLayout *originalLayout = new QVBoxLayout;
    originalLayout->addWidget(scrollAreaOriginal); //
    originalBox->setLayout(originalLayout);

    QVBoxLayout *processedLayout = new QVBoxLayout;
    processedLayout->addWidget(scrollAreaProcessed); 
    processedBox->setLayout(processedLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(originalBox); // Original image box
    mainLayout->addWidget(processedBox); // Processed image box

    // Creates the central widget
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    createActions();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool ImageViewer::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                     .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }


    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
                                .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::setImage(const QImage &newImage)
{
    image = newImage;
    resultImage = newImage;
    if (image.colorSpace().isValid())
        image.convertToColorSpace(QColorSpace::SRgb);

    // Defines the maximum size for the images
    const QSize maxSize = QGuiApplication::primaryScreen()->availableSize() * 3 / 7 + QSize(40, 40);
    const QSize imageSize = image.size();

    QImage scaledImage = image;
    if (imageSize.width() > maxSize.width() || imageSize.height() > maxSize.height()) {
        scaledImage = image.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    imageLabel->setPixmap(QPixmap::fromImage(scaledImage));
    imageLabel->adjustSize();

    resultLabel->setPixmap(QPixmap::fromImage(scaledImage));
    resultLabel->adjustSize();

    //scrollArea->setWidgetResizable(true);
    //scrollAreaResult->setWidgetResizable(true);

    QSize newWindowSize = QSize(scaledImage.width() * 2 + 150, scaledImage.height() + 150); // Largura das duas imagens + espaço extra
    this->resize(newWindowSize);

    // Center the window on the screen
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y); 

    flipHorizontallyAct->setEnabled(true);
    flipVerticallyAct->setEnabled(true);
    convertToGreyScaleAct->setEnabled(true);
    greyScaleQuantizationAct->setEnabled(true);
    resetImageAct->setEnabled(true);
    
    scaleFactor = 1.0;

    updateActions();
}


bool ImageViewer::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(resultImage)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                     .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
                                                  ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    for (const QByteArray &mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::open()
{
    QFileDialog dialog(this, tr("Open Image"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    if (dialog.exec() == QDialog::Accepted) {
        QString fileName = dialog.selectedFiles().constFirst();
        if (loadFile(fileName)) {
            setImage(image); 
        } else {
            QMessageBox::information(this, tr("Error"), tr("Failed to load image"));
        }
    }
}



void ImageViewer::saveAs()
{
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().constFirst())) {}
}

void ImageViewer::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(resultImage);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageViewer::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
                                    .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    resultLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About Photochopp"),
                       tr("<p> The <b>Photochopp Editor</b> is a simple image editor that allows you to perform basic image processing operations.</p>"
                       "<p>Load an image by clicking on the 'File' menu and selecting 'Open'.</p>"
                       "<p>The editor allows you to flip the image horizontally or vertically, convert it to grayscale, quantize the grayscale, and reset the image to its original state.</p>"
                       "<p>Developed by: <b>Augusto Mattei Grohmann</b> and <b>Tiago Vier Preto<b> </p>"));
}

void ImageViewer::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAsAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    editMenu->addSeparator();

    flipHorizontallyAct = editMenu->addAction(tr("Flip &Horizontally"), this, &ImageViewer::flipHorizontally);
    flipHorizontallyAct->setEnabled(false);

    flipVerticallyAct = editMenu->addAction(tr("Flip &Vertically"), this, &ImageViewer::flipVertically);
    flipVerticallyAct->setEnabled(false);

    convertToGreyScaleAct = editMenu->addAction(tr("Convert to &Grey Scale"), this, &ImageViewer::convertToGreyScale);
    convertToGreyScaleAct->setEnabled(false);

    greyScaleQuantizationAct = editMenu->addAction(tr("Grey Scale &Quantization"), this, &ImageViewer::greyScaleQuantization);
    greyScaleQuantizationAct->setEnabled(false);

    resetImageAct = editMenu->addAction(tr("&Reset Image"), this, &ImageViewer::resetImage);
    resetImageAct->setEnabled(false);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
}

void ImageViewer::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap(Qt::ReturnByValue).size());
    resultLabel->resize(scaleFactor * resultLabel->pixmap(Qt::ReturnByValue).size());

    // Ajustar as barras de rolagem da imagem original
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    // Ajustar as barras de rolagem da imagem processada (caso seja necessário)
    adjustScrollBar(scrollAreaResult->horizontalScrollBar(), factor);
    adjustScrollBar(scrollAreaResult->verticalScrollBar(), factor);

    // Atualizar os estados das ações
    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}


void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

void ImageViewer::scale()
{
    const QSize maxSize = QGuiApplication::primaryScreen()->availableSize() * 3 / 7 + QSize(40, 40);
    const QSize imageSize = resultImage.size();

    QImage scaledImage = resultImage;
    if (imageSize.width() > maxSize.width() || imageSize.height() > maxSize.height()) {
        scaledImage = resultImage.scaled(maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    resultLabel->setPixmap(QPixmap::fromImage(scaledImage));
    resultLabel->adjustSize();
}

void ImageViewer::flipHorizontally()
{
    if (resultImage.isNull()) {
        return;
    }

    QImage tempImage = resultImage.copy();
    int width = resultImage.width();
    int height = resultImage.height();
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            resultImage.setPixelColor(i,j,tempImage.pixelColor(width - i - 1, j));
        }
    }
    scale();

}

void ImageViewer::flipVertically()
{
    if (resultImage.isNull()) {
        return;
    }

    QImage tempImage = resultImage.copy();
    int height = resultImage.height();
    for (int i = 0; i < height; ++i) {
        memcpy(resultImage.scanLine(i), tempImage.scanLine(height - 1 - i), resultImage.bytesPerLine());
    }
    scale();
}

void ImageViewer::convertToGreyScale()
{
    if (resultImage.isNull()) {
        return;
    }

    for (int i = 0; i < resultImage.width(); ++i)
    {
        for (int j = 0; j < resultImage.height(); ++j)
        {
            QColor pixelColor = resultImage.pixelColor(i,j);
            double L = 0.299*pixelColor.red() + 0.587*pixelColor.green() + 0.114*pixelColor.blue();
            QColor grey(L,L,L);
            resultImage.setPixelColor(i,j,grey);
        }
    }
    scale();
}

void ImageViewer::greyScaleQuantization()
{
    if (resultImage.isNull()) {
        return;
    }

    // Ask the user for the number of levels
    QString input = QInputDialog::getText(this, tr("Quantization"), tr("Enter the number of levels:"));
    int n = input.toInt();

    if (n <= 0) {
        QMessageBox::warning(this, tr("Error"), tr("Number of levels must be greater than 0."));
        return;
    }

    convertToGreyScale();

    int t1 = INT_MAX;
    int t2 = INT_MIN;

    // Finds the minimum and maximum shades of grey in the image
    for (int i = 0; i < resultImage.width(); i++) {
        for (int j = 0; j < resultImage.height(); j++) {
            QRgb pixel = resultImage.pixel(i, j);
            int value = qGray(pixel);
            if (value < t1) {
                t1 = value;
            }
            if (value > t2) {
                t2 = value;
            }
        }
    }

    int tam_int = t2 - t1 + 1;

    // if the number of levels is greater than the number of shades of grey, return
    if (n >= tam_int) return;

    // Calculates the size of each bin
    float tb = (float) tam_int / n;

    for (int i = 0; i < resultImage.width(); i++) {
        for (int j = 0; j < resultImage.height(); j++) {
            QRgb pixel = resultImage.pixel(i, j);
            int value = qGray(pixel);

            int bin = (value - t1 + 0.5) / tb;

            int new_value = t1 - 0.5 + (bin + 0.5) * tb;

            resultImage.setPixel(i, j, qRgb(new_value, new_value, new_value));
        }
    }
    scale();
}


void ImageViewer::resetImage()
{
    resultImage = image;
    scale();
}


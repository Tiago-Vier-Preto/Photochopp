#include "imageviewer.h"
#include "convolutionwindow.h"
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
    convertToGrayScaleAct->setEnabled(true);
    grayScaleQuantizationAct->setEnabled(true);
    resetImageAct->setEnabled(true);
    histogramAct->setEnabled(true);
    brightnessAct->setEnabled(true);
    contrastAct->setEnabled(true);
    negativeAct->setEnabled(true);
    rotateLeftAct->setEnabled(true);
    rotateRightAct->setEnabled(true);
    histogramEqualizationAct->setEnabled(true);
    grayScaleHistogramMatchingAct->setEnabled(true);
    conv2dAct->setEnabled(true);
    
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

    convertToGrayScaleAct = editMenu->addAction(tr("Convert to &Gray Scale"), this, &ImageViewer::convertToGrayScale);
    convertToGrayScaleAct->setEnabled(false);

    grayScaleQuantizationAct = editMenu->addAction(tr("Gray Scale &Quantization"), this, &ImageViewer::grayScaleQuantization);
    grayScaleQuantizationAct->setEnabled(false);

    brightnessAct = editMenu->addAction(tr("&Brightness"), this, &ImageViewer::brightness);
    brightnessAct->setEnabled(false);

    contrastAct = editMenu->addAction(tr("&Contrast"), this, &ImageViewer::contrast);
    contrastAct->setEnabled(false);

    negativeAct = editMenu->addAction(tr("&Negative"), this, &ImageViewer::negative);
    negativeAct->setEnabled(false);

    histogramEqualizationAct = editMenu->addAction(tr("&Histogram Equalization"), this, &ImageViewer::histogramEqualization);
    histogramEqualizationAct->setEnabled(false);

    grayScaleHistogramMatchingAct = editMenu->addAction(tr("&Grayscale Histogram Matching"), this, &ImageViewer::grayScaleHistogramMatching);
    grayScaleHistogramMatchingAct->setEnabled(false);

    conv2dAct = editMenu->addAction(tr("2D &Convolution"), this, &ImageViewer::conv2d);
    conv2dAct->setEnabled(false);

    resetImageAct = editMenu->addAction(tr("&Reset Image"), this, &ImageViewer::resetImage);
    resetImageAct->setEnabled(false);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    rotateLeftAct = viewMenu->addAction(tr("&Rotate 90 degrees Left"), this, &ImageViewer::rotateLeft);
    rotateLeftAct->setEnabled(false);

    rotateRightAct = viewMenu->addAction(tr("&Rotate 90 degrees Right"), this, &ImageViewer::rotateRight);
    rotateRightAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    QMenu *analyzeMenu = menuBar()->addMenu(tr("&Analyze"));

    histogramAct = analyzeMenu->addAction(tr("&Grayscale Histogram"), this, &ImageViewer::grayScaleHistogram);
    histogramAct->setEnabled(false);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    helpMenu->addAction(tr("&About"), this, &ImageViewer::about);
}

void ImageViewer::updateActions()
{
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!image.isNull());
    zoomOutAct->setEnabled(!image.isNull());
    normalSizeAct->setEnabled(!image.isNull());
    flipHorizontallyAct->setEnabled(!image.isNull());
    flipVerticallyAct->setEnabled(!image.isNull());
    convertToGrayScaleAct->setEnabled(!image.isNull());
    grayScaleQuantizationAct->setEnabled(!image.isNull());
    resetImageAct->setEnabled(!image.isNull());
    histogramAct->setEnabled(!image.isNull());
    brightnessAct->setEnabled(!image.isNull());
    contrastAct->setEnabled(!image.isNull());
    negativeAct->setEnabled(!image.isNull());
    rotateLeftAct->setEnabled(!image.isNull());
    rotateRightAct->setEnabled(!image.isNull());
    histogramEqualizationAct->setEnabled(!image.isNull());
    grayScaleHistogramMatchingAct->setEnabled(!image.isNull());
    conv2dAct->setEnabled(!image.isNull());
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

void ImageViewer::convertToGrayScale()
{
    resultImage = resultImage.convertToFormat(QImage::Format_Grayscale8);
    if (resultImage.isNull()) {
        return;
    }

    for (int i = 0; i < resultImage.width(); ++i)
    {
        for (int j = 0; j < resultImage.height(); ++j)
        {
            QColor pixelColor = resultImage.pixelColor(i,j);
            double L = 0.299*pixelColor.red() + 0.587*pixelColor.green() + 0.114*pixelColor.blue();
            QColor gray(L,L,L);
            resultImage.setPixelColor(i,j,gray);
        }
    } 
    scale();
}

void ImageViewer::grayScaleQuantization()
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

    convertToGrayScale();

    int t1 = INT_MAX;
    int t2 = INT_MIN;

    // Finds the minimum and maximum shades of gray in the image
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

    // if the number of levels is greater than the number of shades of gray, return
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

void ImageViewer::grayScaleHistogram()
{
    if (resultImage.isNull()) {
        return;
    } 

    int histogram[256] = {0};

    QImage resultImageGray = (resultImage.format() == QImage::Format_Grayscale8) ? resultImage : resultImage.convertToFormat(QImage::Format_Grayscale8);

    for (int i = 0; i < resultImageGray.width(); i++) {
        for (int j = 0; j < resultImageGray.height(); j++) {
            QRgb pixel = resultImageGray.pixel(i, j);
            int value = qGray(pixel);
            histogram[value]++;
        }
    }

    int max = *std::max_element(histogram, histogram + 256);

    int histWidth = 512;
    int histHeight = 400;
    QImage histogramImage(histWidth, histHeight, QImage::Format_RGB32);
    histogramImage.fill(QColor(230, 230, 230));  // Light gray background

    QPainter painter(&histogramImage);
    painter.setRenderHint(QPainter::Antialiasing);

    // Set pen and brush for histogram bars
    QPen pen(Qt::blue);  
    painter.setPen(pen);
    painter.setBrush(QColor(0, 120, 215)); 

    // Draw the histogram bars
    for (int i = 0; i < 256; i++) {
        int binWidth = histWidth / 256;
        int binHeight = (histogram[i] * histHeight) / max;
        painter.drawRect(i * binWidth, histHeight - binHeight, binWidth - 1, binHeight);
    }

    // Add x and y axis labels
    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);

    painter.setPen(Qt::black);
    painter.drawText(histWidth / 2 - 20, histHeight - 10, "Pixel Value");  // X-axis label
    painter.rotate(-90);
    painter.drawText(-histHeight / 2 - 40, 20, "Frequency");  // Y-axis label
    painter.resetTransform();

    // Create a new window to display the histogram
    QLabel *histogramLabel = new QLabel;
    histogramLabel->setPixmap(QPixmap::fromImage(histogramImage));
    histogramLabel->setAlignment(Qt::AlignCenter);

    QScrollArea *scrollAreaHistogram = new QScrollArea;
    scrollAreaHistogram->setWidget(histogramLabel);
    scrollAreaHistogram->setAlignment(Qt::AlignCenter);
    scrollAreaHistogram->setWidgetResizable(true);

    QVBoxLayout *histogramLayout = new QVBoxLayout;
    histogramLayout->addWidget(scrollAreaHistogram);

    QWidget *histogramWindow = new QWidget;
    histogramWindow->setLayout(histogramLayout);
    histogramWindow->setWindowTitle(tr("Result Image Grayscale Histogram"));
    histogramWindow->resize(histWidth + 50, histHeight + 50);  // Increase height to accommodate labels
    histogramWindow->show();
}

void ImageViewer::brightness()
{
    if (resultImage.isNull()) {
        return;
    }

    QString input = QInputDialog::getText(this, tr("Brightness"), tr("Enter the brightness value:\nValue must be between -255 and 255."));
    int brightness = input.toInt();

    if (brightness < -255 || brightness > 255) {
        QMessageBox::warning(this, tr("Error"), tr("Brightness value must be between -255 and 255."));
        return;
    }

    if (resultImage.format() == QImage::Format_Grayscale8) {
        for (int i = 0; i < resultImage.width(); i++) {
            for (int j = 0; j < resultImage.height(); j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int value = qGray(pixel) + brightness;
                value = std::max(0, std::min(value, 255));
                resultImage.setPixel(i, j, qRgb(value, value, value));
            }
        }
    } else {
        for (int i = 0; i < resultImage.width(); i++) {
            for (int j = 0; j < resultImage.height(); j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int r = qRed(pixel) + brightness;
                int g = qGreen(pixel) + brightness;
                int b = qBlue(pixel) + brightness;

                r = std::max(0, std::min(r, 255));
                g = std::max(0, std::min(g, 255));
                b = std::max(0, std::min(b, 255));

                resultImage.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }
    scale();
}

void ImageViewer::contrast()
{
    if (resultImage.isNull()) {
        return;
    }

    QString input = QInputDialog::getText(this, tr("Contrast"), tr("Enter the contrast value:\nValue must be greater than 0 and less than or equal to 255."));
    float contrast = input.toFloat();

    if (contrast <= 0 || contrast > 10) {
        QMessageBox::warning(this, tr("Error"), tr("Contrast value must be greater than 0 and less than or equal to 255."));
        return;
    }

    if (resultImage.format() == QImage::Format_Grayscale8) {
        for (int i = 0; i < resultImage.width(); i++) {
            for (int j = 0; j < resultImage.height(); j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int value = qGray(pixel) * contrast;
                value = std::max(0, std::min(value, 255));
                resultImage.setPixel(i, j, qRgb(value, value, value));
            }
        }
    } else {
        for (int i = 0; i < resultImage.width(); i++) {
            for (int j = 0; j < resultImage.height(); j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int r = qRed(pixel) * contrast;
                int g = qGreen(pixel) * contrast;
                int b = qBlue(pixel) * contrast;

                r = std::max(0, std::min(r, 255));
                g = std::max(0, std::min(g, 255));
                b = std::max(0, std::min(b, 255));

                resultImage.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }
    scale();
}

void ImageViewer::negative() 
{
    if (resultImage.isNull()) {
        return;
    }

    if (resultImage.format() == QImage::Format_Grayscale8) {
        for (int i = 0; i < resultImage.width(); i++) {
            for (int j = 0; j < resultImage.height(); j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int value = 255 - qGray(pixel);
                resultImage.setPixel(i, j, qRgb(value, value, value));
            }
        }
    } else {
        for (int i = 0; i < resultImage.width(); i++) {
            for (int j = 0; j < resultImage.height(); j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int r = 255 - qRed(pixel);
                int g = 255 - qGreen(pixel);
                int b = 255 - qBlue(pixel);

                resultImage.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }
    scale();
}

void ImageViewer::rotateLeft()
{
    if (resultImage.isNull()) {
        return;
    }

    QTransform transform;
    transform.rotate(-90);

    resultImage = resultImage.transformed(transform);
    scale();
}

void ImageViewer::rotateRight()
{
    if (resultImage.isNull()) {
        return;
    }

    QTransform transform;
    transform.rotate(90);

    resultImage = resultImage.transformed(transform);
    scale();
}

void ImageViewer::histogramEqualization() {
    if (resultImage.isNull()) {
        return;
    }

    int width = resultImage.width();
    int height = resultImage.height();
    int numPixels = width * height;

    if (resultImage.format() == QImage::Format_Grayscale8) {
        int histogram[256] = {0};
        int cdf[256] = {0};
        float alpha = 255.0f / numPixels;

        // Cálculo do histograma
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int value = qGray(resultImage.pixel(i, j));
                histogram[value]++;
            }
        }

        // Cálculo do histograma cumulativo (CDF)
        cdf[0] = static_cast<int>(std::round(alpha * histogram[0]));
        for (int i = 1; i < 256; i++) {
            cdf[i] = cdf[i - 1] + static_cast<int>(std::round(alpha * histogram[i]));
            cdf[i] = std::min(255, cdf[i]); // Limitando o valor entre 0 e 255
        }

        // Aplicação do histograma cumulativo na imagem
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                int value = qGray(resultImage.pixel(i, j));
                int equalizedGray = cdf[value];
                resultImage.setPixel(i, j, qRgb(equalizedGray, equalizedGray, equalizedGray));
            }
        }

        int histogramOriginal[256] = {0};

        QImage resultImageGray = image.convertToFormat(QImage::Format_Grayscale8);

        for (int i = 0; i < resultImageGray.width(); i++) {
            for (int j = 0; j < resultImageGray.height(); j++) {
                QRgb pixel = resultImageGray.pixel(i, j);
                int value = qGray(pixel);
                histogramOriginal[value]++;
            }
        }

        int max = *std::max_element(histogramOriginal, histogramOriginal + 256);

        int histWidth = 512;
        int histHeight = 400;
        QImage histogramImage(histWidth, histHeight, QImage::Format_RGB32);
        histogramImage.fill(QColor(230, 230, 230));  // Light gray background

        QPainter painter(&histogramImage);
        painter.setRenderHint(QPainter::Antialiasing);

        // Set pen and brush for histogram bars
        QPen pen(Qt::blue);  
        painter.setPen(pen);
        painter.setBrush(QColor(0, 120, 215)); 

        // Draw the histogram bars
        for (int i = 0; i < 256; i++) {
            int binWidth = histWidth / 256;
            int binHeight = (histogramOriginal[i] * histHeight) / max;
            painter.drawRect(i * binWidth, histHeight - binHeight, binWidth - 1, binHeight);
        }

        // Add x and y axis labels
        QFont font = painter.font();
        font.setPointSize(10);
        painter.setFont(font);

        painter.setPen(Qt::black);
        painter.drawText(histWidth / 2 - 20, histHeight - 10, "Pixel Value");  // X-axis label
        painter.rotate(-90);
        painter.drawText(-histHeight / 2 - 40, 20, "Frequency");  // Y-axis label
        painter.resetTransform();

        // Create a new window to display the histogram
        QLabel *histogramLabel = new QLabel;
        histogramLabel->setPixmap(QPixmap::fromImage(histogramImage));
        histogramLabel->setAlignment(Qt::AlignCenter);

        QScrollArea *scrollAreaHistogram = new QScrollArea;
        scrollAreaHistogram->setWidget(histogramLabel);
        scrollAreaHistogram->setAlignment(Qt::AlignCenter);
        scrollAreaHistogram->setWidgetResizable(true);

        QVBoxLayout *histogramLayout = new QVBoxLayout;
        histogramLayout->addWidget(scrollAreaHistogram);

        QWidget *histogramWindow = new QWidget;
        histogramWindow->setLayout(histogramLayout);
        histogramWindow->setWindowTitle(tr("Original Image Grayscale Histogram"));
        histogramWindow->resize(histWidth + 50, histHeight + 50);  // Increase height to accommodate labels
        histogramWindow->show();

        grayScaleHistogram();

    } else { // Para imagens coloridas
        int histogramR[256] = {0};
        int histogramG[256] = {0};
        int histogramB[256] = {0};
        int cdfR[256] = {0};
        int cdfG[256] = {0};
        int cdfB[256] = {0};
        float alpha = 255.0f / numPixels;

        // Cálculo dos histogramas
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                QRgb pixel = resultImage.pixel(i, j);
                histogramR[qRed(pixel)]++;
                histogramG[qGreen(pixel)]++;
                histogramB[qBlue(pixel)]++;
            }
        }

        // Cálculo dos histogramas cumulativos (CDF)
        cdfR[0] = static_cast<int>(std::round(alpha * histogramR[0]));
        cdfG[0] = static_cast<int>(std::round(alpha * histogramG[0]));
        cdfB[0] = static_cast<int>(std::round(alpha * histogramB[0]));
        for (int i = 1; i < 256; i++) {
            cdfR[i] = std::min(255, cdfR[i - 1] + static_cast<int>(std::round(alpha * histogramR[i])));
            cdfG[i] = std::min(255, cdfG[i - 1] + static_cast<int>(std::round(alpha * histogramG[i])));
            cdfB[i] = std::min(255, cdfB[i - 1] + static_cast<int>(std::round(alpha * histogramB[i])));
        }

        // Aplicação do histograma cumulativo na imagem
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                QRgb pixel = resultImage.pixel(i, j);
                int r = cdfR[qRed(pixel)];
                int g = cdfG[qGreen(pixel)];
                int b = cdfB[qBlue(pixel)];
                resultImage.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }
    scale(); 
}

void ImageViewer::grayScaleHistogramMatching()
{
    if (resultImage.isNull()) {
        return;
    }

    // Ask the user for the image to be used as reference
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::homePath(), tr("Images (*.png *.jpg *.bmp)"));
    if (fileName.isEmpty()) {
        return;
    }

    QImage referenceImage;
    if (!referenceImage.load(fileName)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to load reference image."));
        return;
    }

    if (referenceImage.format() != QImage::Format_Grayscale8) {
        referenceImage = referenceImage.convertToFormat(QImage::Format_Grayscale8);
    }

    int hist_src[256] = {0}, hist_target[256] = {0}, hist_src_cum[256] = {0}, hist_target_cum[256] = {0};
    int HM[256] = {0};

    float alpha_target = 255.0f / (referenceImage.width() * referenceImage.height());
    float alpha_src = 255.0f / (resultImage.width() * resultImage.height());

    // histogram calculation 
    for (int i = 0; i < referenceImage.width(); i++) {
        for (int j = 0; j < referenceImage.height(); j++) {
            int value = qGray(referenceImage.pixel(i, j));
            hist_target[value]++;
        }
    }

    for (int i = 0; i < resultImage.width(); i++) {
        for (int j = 0; j < resultImage.height(); j++) {
            int value = qGray(resultImage.pixel(i, j));
            hist_src[value]++;
        }
    }

    // Cumulative histogram calculation
    hist_src_cum[0] = static_cast<int>(std::round(alpha_src * hist_src[0]));
    hist_target_cum[0] = static_cast<int>(std::round(alpha_target * hist_target[0]));

    for (int i = 1; i < 256; i++) {
        hist_src_cum[i] = hist_src_cum[i - 1] + static_cast<int>(std::round(alpha_src * hist_src[i]));
        hist_target_cum[i] = hist_target_cum[i - 1] + static_cast<int>(std::round(alpha_target * hist_target[i]));
    }

    for (int i = 0; i < 256; i++) {
        int j = 0;
        do {
            HM[i] = j;
            j++;
        } while (hist_src_cum[i] > hist_target_cum[j]);
    }

    for (int i = 0; i < resultImage.width(); i++) {
        for (int j = 0; j < resultImage.height(); j++) {
            int value = qGray(resultImage.pixel(i, j));
            resultImage.setPixel(i, j, qRgb(HM[value], HM[value], HM[value]));
        }
    }
    scale();
}

void ImageViewer::conv2d() 
{
    convolutionwindow *convWindow = new convolutionwindow(this); 
    convWindow->show();
}
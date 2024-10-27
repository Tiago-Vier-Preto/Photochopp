#include "convolutionwindow.h"
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDoubleValidator>

convolutionwindow::convolutionwindow(QWidget *parent)
    : QMainWindow{parent} {

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20); 
    mainLayout->setSpacing(15); 

    QLabel *titleLabel = new QLabel("Digite os valores para o kernel 3x3:");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter); 

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setFixedSize(80, 30);
            lineEdit->setAlignment(Qt::AlignCenter); 
            lineEdit->setStyleSheet("font-size: 16px; padding: 5px; "
                                    "border: 1px solid #ccc; border-radius: 5px;");
            QDoubleValidator *validator = new QDoubleValidator(this);
            validator->setLocale(QLocale::C); 
            lineEdit->setValidator(validator); 
            kernelInputs[i * 3 + j] = lineEdit; 
            gridLayout->addWidget(lineEdit, i, j);
        }
    }
    mainLayout->addLayout(gridLayout);

    QPushButton *applyButton = new QPushButton("Apply Kernel", this);
    applyButton->setStyleSheet("background-color: #4CAF50; color: white; "
                              "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                              "border: none; border-radius: 5px;");
    applyButton->setFixedSize(155, 40);
    applyButton->setCursor(Qt::PointingHandCursor);
    applyButton->setToolTip("Apply Kernel");
    applyButton->setFocusPolicy(Qt::NoFocus);
    mainLayout->addWidget(applyButton, 0, Qt::AlignCenter);

    QHBoxLayout *firstButtonLayout = new QHBoxLayout();

    QPushButton *gaussianButton = new QPushButton("Gaussian Filter", this);
    gaussianButton->setStyleSheet("background-color: #333; color: white; "
                                  "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                  "border: none; border-radius: 5px;");
    gaussianButton->setFixedSize(155, 40);
    gaussianButton->setCursor(Qt::PointingHandCursor);
    gaussianButton->setFocusPolicy(Qt::NoFocus);
    firstButtonLayout->addWidget(gaussianButton);

    QPushButton *laplacianButton = new QPushButton("Laplacian Filter", this);
    laplacianButton->setStyleSheet("background-color: #333; color: white; "
                                   "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                   "border: none; border-radius: 5px;");
    laplacianButton->setFixedSize(155, 40);
    laplacianButton->setCursor(Qt::PointingHandCursor);
    laplacianButton->setFocusPolicy(Qt::NoFocus);
    firstButtonLayout->addWidget(laplacianButton);

    QPushButton *highPassButton = new QPushButton("High-Pass Filter", this);
    highPassButton->setStyleSheet("background-color: #333; color: white; "
                                  "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                  "border: none; border-radius: 5px;");
    highPassButton->setFixedSize(155, 40);
    highPassButton->setCursor(Qt::PointingHandCursor);
    highPassButton->setFocusPolicy(Qt::NoFocus);
    firstButtonLayout->addWidget(highPassButton);

    mainLayout->addLayout(firstButtonLayout);

    QHBoxLayout *secondButtonLayout = new QHBoxLayout();

    QPushButton *prewittHxButton = new QPushButton("Prewitt Hx", this);
    prewittHxButton->setStyleSheet("background-color: #333; color: white; "
                                  "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                  "border: none; border-radius: 5px;");
    prewittHxButton->setFixedSize(155, 40);
    prewittHxButton->setCursor(Qt::PointingHandCursor);
    prewittHxButton->setFocusPolicy(Qt::NoFocus);
    secondButtonLayout->addWidget(prewittHxButton);

    QPushButton *prewittHyButton = new QPushButton("Prewitt Hy", this);
    prewittHyButton->setStyleSheet("background-color: #333; color: white; "
                                  "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                  "border: none; border-radius: 5px;");
    prewittHyButton->setFixedSize(155, 40);
    prewittHyButton->setCursor(Qt::PointingHandCursor);
    prewittHyButton->setFocusPolicy(Qt::NoFocus);
    secondButtonLayout->addWidget(prewittHyButton);

    QPushButton *sobelHxButton = new QPushButton("Sobel Hx", this);
    sobelHxButton->setStyleSheet("background-color: #333; color: white; "
                                  "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                  "border: none; border-radius: 5px;");
    sobelHxButton->setFixedSize(155, 40);
    sobelHxButton->setCursor(Qt::PointingHandCursor);
    sobelHxButton->setFocusPolicy(Qt::NoFocus);
    secondButtonLayout->addWidget(sobelHxButton);

    mainLayout->addLayout(secondButtonLayout);

    QHBoxLayout *thirdButtonLayout = new QHBoxLayout();

    QPushButton *sobelHyButton = new QPushButton("Sobel Hy", this);
    sobelHyButton->setStyleSheet("background-color: #333; color: white; "
                                  "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                                  "border: none; border-radius: 5px;");
    sobelHyButton->setFixedSize(155, 40);
    sobelHyButton->setCursor(Qt::PointingHandCursor);
    sobelHyButton->setFocusPolicy(Qt::NoFocus);
    thirdButtonLayout->addWidget(sobelHyButton);

    mainLayout->addLayout(thirdButtonLayout);

    connect(applyButton, &QPushButton::clicked, this, &convolutionwindow::saveKernelValues);
    connect(gaussianButton, &QPushButton::clicked, this, &convolutionwindow::gaussianFilter);
    connect(laplacianButton, &QPushButton::clicked, this, &convolutionwindow::laplacianFilter);
    connect(highPassButton, &QPushButton::clicked, this, &convolutionwindow::highPassFilter);
    connect(prewittHxButton, &QPushButton::clicked, this, &convolutionwindow::prewittHxFilter);
    connect(prewittHyButton, &QPushButton::clicked, this, &convolutionwindow::prewittHyFilter);
    connect(sobelHxButton, &QPushButton::clicked, this, &convolutionwindow::sobelHxFilter);
    connect(sobelHyButton, &QPushButton::clicked, this, &convolutionwindow::sobelHyFilter);

    setWindowTitle("Entrada de Kernel 3x3");
    resize(500, 300);
}

void convolutionwindow::saveKernelValues() {
    std::vector<std::vector<float>> kernelValues(3, std::vector<float>(3, 0));
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QString text = kernelInputs[i * 3 + j]->text();
            if (text.isEmpty()) {
                QMessageBox::warning(this, "Erro", "Por favor, preencha todos os campos.");
                return;
            }
            bool ok;
            float value = text.toFloat(&ok); 
            if (!ok) {
                QMessageBox::warning(this, "Erro", "Valor inválido no kernel. Por favor, insira um número.");
                return;
            }
            kernelValues[i][j] = value; 
        }
    }
    emit convolution(kernelValues); 
}

void convolutionwindow::gaussianFilter() {
    QVector<double> gaussianValues = {
        0.0625, 0.125, 0.0625,
        0.125, 0.25, 0.125,
        0.0625, 0.125, 0.0625
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(gaussianValues[i]));
    }
}

void convolutionwindow::laplacianFilter() {
    QVector<double> laplacianValues = {
        0, -1, 0,
        -1, 4, -1,
        0, -1, 0
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(laplacianValues[i]));
    }
}

void convolutionwindow::highPassFilter() {
    QVector<double> highPassValues = {
        -1, -1, -1,
        -1, 8, -1,
        -1, -1, -1
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(highPassValues[i]));
    }
}

void convolutionwindow::prewittHxFilter() {
    QVector<double> prewittHxValues = {
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(prewittHxValues[i]));
    }
}

void convolutionwindow::prewittHyFilter() {
    QVector<double> prewittHyValues = {
        -1, -1, -1,
        0, 0, 0,
        1, 1, 1
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(prewittHyValues[i]));
    }
}

void convolutionwindow::sobelHxFilter() {
    QVector<double> sobelHxValues = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(sobelHxValues[i]));
    }
}

void convolutionwindow::sobelHyFilter() {
    QVector<double> sobelHyValues = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };

    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(sobelHyValues[i]));
    }
}

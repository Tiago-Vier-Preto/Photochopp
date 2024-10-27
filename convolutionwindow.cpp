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
    // Define o widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Cria o layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20); // Margens ao redor do layout principal
    mainLayout->setSpacing(15); // Espaçamento entre widgets

    // Título
    QLabel *titleLabel = new QLabel("Digite os valores para o kernel 3x3:");
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    mainLayout->addWidget(titleLabel, 0, Qt::AlignCenter); // Centraliza o título

    // Cria o grid layout para os inputs 3x3
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10); // Espaçamento entre inputs
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            QLineEdit *lineEdit = new QLineEdit(this);
            lineEdit->setFixedSize(80, 30); // Define um tamanho fixo para os inputs
            lineEdit->setAlignment(Qt::AlignCenter); // Centraliza o texto
            lineEdit->setStyleSheet("font-size: 16px; padding: 5px; "
                                    "border: 1px solid #ccc; border-radius: 5px;");
            QDoubleValidator *validator = new QDoubleValidator(this);
            validator->setLocale(QLocale::C); // Define locale para aceitar vírgula e ponto
            lineEdit->setValidator(validator); // Valida apenas números
            kernelInputs[i * 3 + j] = lineEdit; // Armazena o QLineEdit no array
            gridLayout->addWidget(lineEdit, i, j);
        }
    }
    mainLayout->addLayout(gridLayout);

    // Botão para salvar o kernel
    QPushButton *saveButton = new QPushButton("Apply Kernel", this);
    saveButton->setStyleSheet("background-color: #4CAF50; color: white; "
                              "font-size: 14px; font-weight: bold; padding: 10px 24px; "
                              "border: none; border-radius: 5px;");
    saveButton->setFixedSize(155, 40);
    saveButton->setCursor(Qt::PointingHandCursor);
    saveButton->setToolTip("Apply Kernel");
    saveButton->setFocusPolicy(Qt::NoFocus);
    mainLayout->addWidget(saveButton, 0, Qt::AlignCenter);

    // Cria um layout horizontal para os botões adicionais
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


    // Conecta o botão ao slot para capturar os valores
    connect(saveButton, &QPushButton::clicked, this, &convolutionwindow::saveKernelValues);
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
    QVector<double> kernelValues;
    for (int i = 0; i < 9; ++i) {
        bool ok;
        double value = kernelInputs[i]->text().toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Erro", "Por favor, insira valores válidos em todos os campos.");
            return;
        }
        kernelValues.append(value);
    }

    // Exibe o kernel em uma mensagem
    QString kernelText;
    for (int i = 0; i < 9; ++i) {
        kernelText += QString::number(kernelValues[i]) + " ";
        if ((i + 1) % 3 == 0) kernelText += "\n";
    }
    QMessageBox::information(this, "Kernel 3x3", "Valores do kernel:\n" + kernelText);
}

void convolutionwindow::gaussianFilter() {
    // Define os valores do filtro gaussiano
    QVector<double> gaussianValues = {
        0.0625, 0.125, 0.0625,
        0.125, 0.25, 0.125,
        0.0625, 0.125, 0.0625
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(gaussianValues[i]));
    }
}

void convolutionwindow::laplacianFilter() {
    // Define os valores do filtro laplaciano
    QVector<double> laplacianValues = {
        0, -1, 0,
        -1, 4, -1,
        0, -1, 0
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(laplacianValues[i]));
    }
}

void convolutionwindow::highPassFilter() {
    // Define os valores do filtro high-pass
    QVector<double> highPassValues = {
        -1, -1, -1,
        -1, 8, -1,
        -1, -1, -1
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(highPassValues[i]));
    }
}

void convolutionwindow::prewittHxFilter() {
    // Define os valores do filtro Prewitt Hx
    QVector<double> prewittHxValues = {
        -1, 0, 1,
        -1, 0, 1,
        -1, 0, 1
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(prewittHxValues[i]));
    }
}

void convolutionwindow::prewittHyFilter() {
    // Define os valores do filtro Prewitt Hy
    QVector<double> prewittHyValues = {
        -1, -1, -1,
        0, 0, 0,
        1, 1, 1
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(prewittHyValues[i]));
    }
}

void convolutionwindow::sobelHxFilter() {
    // Define os valores do filtro Sobel Hx
    QVector<double> sobelHxValues = {
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(sobelHxValues[i]));
    }
}

void convolutionwindow::sobelHyFilter() {
    // Define os valores do filtro Sobel Hy
    QVector<double> sobelHyValues = {
        -1, -2, -1,
        0, 0, 0,
        1, 2, 1
    };

    // Insere os valores nos QLineEdit correspondentes
    for (int i = 0; i < 9; ++i) {
        kernelInputs[i]->setText(QString::number(sobelHyValues[i]));
    }
}

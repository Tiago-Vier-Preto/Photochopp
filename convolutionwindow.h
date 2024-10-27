#ifndef CONVOLUTIONWINDOW_H
#define CONVOLUTIONWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QVector>

class convolutionwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit convolutionwindow(QWidget *parent = nullptr);

private slots:
    void saveKernelValues(); // Slot para salvar os valores do kernel

private:
    QLineEdit *kernelInputs[9]; // Array para armazenar os campos de entrada do kernel 3x3
    void gaussianFilter();
    void laplacianFilter();
    void highPassFilter();
    void prewittHxFilter();
    void prewittHyFilter();
    void sobelHxFilter();
    void sobelHyFilter();
};

#endif // CONVOLUTIONWINDOW_H

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
    void saveKernelValues(); 

private:
    QLineEdit *kernelInputs[9]; 
    void gaussianFilter();
    void laplacianFilter();
    void highPassFilter();
    void prewittHxFilter();
    void prewittHyFilter();
    void sobelHxFilter();
    void sobelHyFilter();

signals:
    void convolution(const  std::vector<std::vector<float>> &kernel);
};

#endif // CONVOLUTIONWINDOW_H

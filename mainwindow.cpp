#include "mainwindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QFont>
#include <CL/opencl.hpp>
#include <fstream>
#include <vector>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);
    runButton = new QPushButton("Lancer le calcul OpenCL", this);
    resultLabel = new QLabel("Résultat : ", this);
    QFont font = resultLabel->font();
    font.setPointSize(24); // Police plus grande
    resultLabel->setFont(font);
    layout->addWidget(runButton);
    layout->addWidget(resultLabel);
    setCentralWidget(central);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runKernel);
}

void MainWindow::runKernel() {
    // Lire le code source
    std::ifstream file("hello.cl");
    std::string src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    cl::Program::Sources sources(1, {src.c_str(), src.size()});

    // Plateforme et device
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    auto platform = platforms.front();
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    auto device = devices.front();

    // Contexte + queue
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Programme et kernel
    cl::Program program(context, sources);
    program.build({device});
    cl::Kernel kernel(program, "hello");

    // Buffer pour le message (int)
    cl::Buffer buffer(context, CL_MEM_WRITE_ONLY, sizeof(int));
    kernel.setArg(0, buffer);

    // Lancer le kernel
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1));
    queue.finish();

    // Lire la sortie (int)
    int result = 0;
    queue.enqueueReadBuffer(buffer, CL_TRUE, 0, sizeof(result), &result);
    resultLabel->setText(QString("Résultat : %1").arg(result));
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "parallelizer.h"

#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_convert_uploadCode_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open C++ File"), "", tr("C++ Files (*.cpp *.h *.c *.hpp);;All Files (*)"));

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString contents = stream.readAll();
            file.close();
            ui->plainTextEdit_SequentialCode->setPlainText(contents);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Could not open file."));
        }
    }
}

void MainWindow::on_pb_ParallelizeCode_clicked()
{
    QString inputCode = ui->plainTextEdit_SequentialCode->toPlainText();
    QString outputCode = Parallelizer::addOpenMPPragmas(inputCode);
    ui->plainTextEdit_ParallelizedCode->setPlainText(outputCode);
}

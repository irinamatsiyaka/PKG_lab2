#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QHeaderView>
#include <QTableWidget>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QImageReader>
#include <QFile>
#include <QDebug>
#include <cmath>


MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(900, 600);

    QAction *exitAct = new QAction("Exit");
    QAction *loadAct = new QAction("Load directory");
    QMenuBar *m_pMenu = new QMenuBar(this);
    QMenu *fileMenu = m_pMenu->addMenu("&File");
    fileMenu->addAction(loadAct);
    fileMenu->addAction(exitAct);

    m_pTableWidget = new QTableWidget(this);
    m_pTableWidget->setRowCount(0); // Initialize with 0 rows
    m_pTableWidget->setColumnCount(7);
    m_pTableWidget->setColumnWidth(5, 200);

    QStringList m_TableHeader;
    m_TableHeader << "File" << "Height" << "Width" << "Bits" << "DPI" << "Compression Ratio" << "GIF/ColorCount";
    m_pTableWidget->setHorizontalHeaderLabels(m_TableHeader);

    QPushButton *m_pBtn = new QPushButton("Load Directory", this);
    QPushButton *m_pBtntwo = new QPushButton("Load Images", this);
    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(m_pMenu);
    l->addWidget(m_pBtn);
    l->addWidget(m_pBtntwo);
    l->addWidget(m_pTableWidget);

    QObject::connect(exitAct, &QAction::triggered, qApp, &QApplication::quit);
    QObject::connect(loadAct, &QAction::triggered, this, &MainWindow::chooseDirectory);
    QObject::connect(m_pBtn, &QPushButton::clicked, this, &MainWindow::chooseDirectory);
    QObject::connect(m_pBtntwo, &QPushButton::clicked, this, &MainWindow::chooseImages);
}

void MainWindow::chooseDirectory()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, "Select Directory", QDir::currentPath());

    if (!directoryPath.isEmpty()) {
        QDir directory(directoryPath);
        QStringList fileNames = directory.entryList(QDir::Files);

        for (int i = 0; i < fileNames.size(); i++) {
            fileNames[i] = directoryPath + "/" + fileNames[i];
        }

        fillInfo(fileNames);
    }
}

void MainWindow::chooseImages()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Select Image Files", QDir::currentPath(), "Images (*.jpg *.png *.bmp *.jpeg *.gif *.tiff)");

    if (!fileNames.isEmpty()) {
        fillInfo(fileNames);
    }
}


void MainWindow::fillInfo(QStringList fileNames)
{
    int initialSize = m_pTableWidget->rowCount();
    m_pTableWidget->setRowCount(initialSize + fileNames.size());

    for (int i = initialSize; i < m_pTableWidget->rowCount(); i++)
    {
        int idx = i - initialSize;
        QImage img;
        img.load(fileNames[idx]);
        QTableWidgetItem *item = new QTableWidgetItem(QFileInfo(fileNames[idx].trimmed()).fileName());
        m_pTableWidget->setItem(i, 0, item);

        QTableWidgetItem *item2 = new QTableWidgetItem(QString::number(img.height()));
        m_pTableWidget->setItem(i, 1, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(img.width()));
        m_pTableWidget->setItem(i, 2, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem(QString::number(img.bitPlaneCount()));
        m_pTableWidget->setItem(i, 3, item4);

        QTableWidgetItem *item5 = new QTableWidgetItem(QString::number(std::round(img.dotsPerMeterX() / 39.3701)));
        m_pTableWidget->setItem(i, 4, item5);

        QFile file(fileNames[idx]);
        file.open(QIODevice::ReadOnly);
        QTableWidgetItem *item6;

        if (fileNames[idx].toLower().endsWith("bmp") || fileNames[idx].toLower().endsWith("pcx")) {
            item6 = new QTableWidgetItem("N/A");
        }
        else {
            double pureSize = img.height() * img.width() * img.bitPlaneCount() / 8;
            int fileSize = file.size();
            float ratio = (float)((int)(pureSize / fileSize * 100 + 0.5));
            item6 = new QTableWidgetItem(QString::number(ratio / 100));
        }
        m_pTableWidget->setItem(i, 5, item6);

        // Obtain the file size using QFile
        qint64 fileSize = file.size();
        QTableWidgetItem *item7 = new QTableWidgetItem(QString::number(fileSize));
        m_pTableWidget->setItem(i, 6, item7);

        // Create an image reader to access additional properties
        QImageReader reader(fileNames[idx]);
        reader.setAutoTransform(true);  // Enable automatic image transformation
        reader.setDecideFormatFromContent(true);
        reader.read();

        // Get the number of colors in the palette for GIF images
        if (reader.format() == "GIF") {
            QString colorCount = reader.text("GIF/ColorCount");
            if (!colorCount.isEmpty()) {
                int numColors = colorCount.toInt();
                qDebug() << "Number of colors in GIF: " << numColors;
            }
        }
    }

    m_pTableWidget->resizeColumnsToContents();
    m_pTableWidget->resizeRowsToContents();
}


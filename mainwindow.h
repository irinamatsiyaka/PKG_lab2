#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QMenuBar>
#include <QAction>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void chooseDirectory();
    void fillInfo(QStringList fileNames);

private:
    QTableWidget *m_pTableWidget;
};

#endif // MAINWINDOW_H

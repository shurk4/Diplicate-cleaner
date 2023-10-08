#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>

#include "compareEngine.h"
#include "qlistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QVector<QVector<QFileInfo>> filesList;
    void clearAll();
    void clearOrig();
    void clearDup();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void getAction(QString act);

private slots:
    void on_pushButtonSourcePath_clicked();

    void on_pushButtonAnalize_clicked();

    void on_listWidgetOrig_currentRowChanged(int currentRow);

    void on_listWidgetDup_currentRowChanged(int currentRow);

private:
    Ui::MainWindow *ui;

    CompareEngine engine;
};
#endif // MAINWINDOW_H

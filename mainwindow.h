#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsView>

#include "compareEngine.h"
#include "qlistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QVector<QVector<QFileInfo>> filesList;  // Список файлов с копиями
//    QVector<QFileInfo> selectedFileCopies; // Список копий выбранного файла
    CompareEngine engine;

    int selectedFileId; // Выбранный файл оригинала
    int selectedDupFileId; // Выбранный файл дупликата

    void showImage(const QString &path, QGraphicsView *view, QGraphicsScene *scene);
    void clearAll();
    void clearOrig();
    void clearDup();

    QGraphicsScene *origScene;
    QGraphicsScene *dupScene;

protected:
    void resizeEvent(QResizeEvent *event);

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

    void on_pushButtonRotateLeft_clicked();

    void on_pushButtonRotateRight_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

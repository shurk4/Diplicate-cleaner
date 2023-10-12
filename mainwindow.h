#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QThread>

#include "compareEngine.h"
#include "qlistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QVector<QVector<QFileInfo>> filesList;  // Список файлов с копиями
    QString sourcePath;

    CompareEngine engine;
    QThread thread;

    int selectedFileId; // Выбранный файл оригинала
    int selectedDupFileId; // Выбранный файл дубликата

    int filesWithDup = 0;
    int files2Dup = 0;
    int files3Dup = 0;
    int filesMoreDup = 0;

    void showImage(const QString &path, QGraphicsView *view, QGraphicsScene *scene);
    void clearAll();
    void clearOrig();
    void clearDup();
    void clearCounters();
    void showFilesList();

    QGraphicsScene *origScene;
    QGraphicsScene *dupScene;

protected:
    void resizeEvent(QResizeEvent *event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void runCompare();
    void runFull();
    void runDelete();

public slots:
    void getAction(QString act);

    void finishedCompare();
    void finishedFull();
    void finishedDelete();

private slots:
    void on_pushButtonSourcePath_clicked();
    void on_pushButtonAnalize_clicked();
    void on_listWidgetOrig_currentRowChanged(int currentRow);
    void on_listWidgetDup_currentRowChanged(int currentRow);
    void on_pushButtonRotateLeft_clicked();
    void on_pushButtonRotateRight_clicked();
    void on_pushButtonFullCompare_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

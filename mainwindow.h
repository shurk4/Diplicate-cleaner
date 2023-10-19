#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsItem>
#include <QGraphicsView>
#include <QThread>
#include <QTabBar>
#include <QDateTime>

#include "compareEngine.h"
#include "qlistwidget.h"

enum TAB
{
    BASIC,
    SAVE
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QVector<QVector<QFileInfo>> filesList;  // Список файлов с копиями
    QString sourcePath;
    QString savePath;

    CompareEngine engine;
    QThread thread;

    int selectedFileId; // Выбранный файл оригинала
    int selectedDupFileId; // Выбранный файл дубликата

    int filesWithDup = 0;
    int files2Dup = 0;
    int files3Dup = 0;
    int filesMoreDup = 0;

    //Меню
    void openFolder(TAB tab);
    void analyze();
    void fullCompare();
    void dupDelete();
    void saveFiles();
    //Меню

    void disableBasicActions();
    void disableSaveActions();
    void showImage(const QString &path, QGraphicsView *view, QGraphicsScene *scene);
    void clearAll();
    void clearOrig();
    void clearDup();
    void clearCounters();
    void showFilesList();
    void createSaveName(); // Не реализован
    void showExample();

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
    void on_listWidgetOrig_currentRowChanged(int currentRow);
    void on_listWidgetDup_currentRowChanged(int currentRow);
    void on_pushButtonRotateLeft_clicked();
    void on_pushButtonRotateRight_clicked();

    void on_openDir_triggered();
    void on_analize_triggered();
    void on_fullCompare_triggered();
    void on_dupDelete_triggered();
    void on_showLog_triggered(bool checked);
    void on_saveOrig_triggered();

    void on_checkBoxSortDirs_stateChanged(int arg1);

    void on_pushButtonCancelSave_clicked();

    void on_pushButtonSaveFolder_clicked();

    void on_checkBoxRename_stateChanged(int arg1);

    void on_checkBoxSortDates_stateChanged(int arg1);

    void on_checkBoxSortYears_stateChanged(int arg1);

    void on_checkBoxSortMonths_stateChanged(int arg1);

    void on_checkBoxSortFileType_stateChanged(int arg1);

    void on_checkBoxSortExtension_stateChanged(int arg1);

    void on_radioButtonDateName_clicked();

    void on_radioButtonCustomName_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

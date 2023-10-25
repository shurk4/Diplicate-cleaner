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
#include <QStorageInfo>

#include "compareEngine.h"
#include "saveEngine.h"

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
//--Basic
    QVector<QVector<QFileInfo>> filesList;  // Список файлов с копиями
    QString sourcePath;

    CompareEngine engine;
    QThread thread;
    SaveEngine saveEngine;
    QThread saveThread;

    QGraphicsScene *origScene;
    QGraphicsScene *dupScene;

    int selectedFileId; // Выбранный файл оригинала
    int selectedDupFileId; // Выбранный файл дубликата

    int filesWithDup = 0;
    int files2Dup = 0;
    int files3Dup = 0;
    int filesMoreDup = 0;

    //Menu
    void openFolder(TAB tab);
    void analyze();
    void fullCompare();
    void dupDelete();

    //Actions
    void disableBasicActions();
    void disableSaveActions();
    void showImage(const QString &path, QGraphicsView *view, QGraphicsScene *scene);
    void clearAll();
    void clearOrig();
    void clearDup();
    void clearCounters();
    void showFilesList();
    QString getSizeString(qint64 bytes);


//--Save
    QString saveSplitter = "";

    QString createSaveName(QDateTime dateTime = QDateTime::currentDateTime().toLocalTime());
    void showExample();
    void saveFiles();

    void hideSaveWidgets();

protected:
    void resizeEvent(QResizeEvent *event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    //Basic
    void runCompare();
    void runFull();
    void runDelete();

    //Save
//    void calcSaveSize();
    void startCopy();

public slots:
    //Basic
    void getAction(QString act); // Log

    void finishedCompare();
    void finishedFull();
    void finishedDelete();

    //Save
    void error(QString error);
    void progress(int percent);
//    void calcSaveSizeReady();
    void copyFinished();

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

    void on_checkBoxYear_stateChanged(int arg1);

    void on_lineEditCustomName_textEdited(const QString &arg1);

    void on_comboBoxSplitter_activated(int index);

    void on_comboBoxNumeric_activated(int index);

    void on_checkBoxMonth_stateChanged(int arg1);

    void on_checkBoxTime_stateChanged(int arg1);

    void on_radioButtonYYYY_clicked();

    void on_radioButtonYY_clicked();

    void on_radioButtonMM_clicked();

    void on_radioButtonMMM_clicked();

    void on_radioButtonMMMM_clicked();

    void on_radioButtonHHMM_clicked();

    void on_radioButtonHHMMSS_clicked();

    void on_checkBoxDate_stateChanged(int arg1);

    void on_pushButtonStartSave_clicked();

    void on_radioButtonOnlyNum_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

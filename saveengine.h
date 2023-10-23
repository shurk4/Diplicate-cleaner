#ifndef SAVEENGINE_H
#define SAVEENGINE_H

#include <QObject>
#include <QWidget>
#include <QDateTime>
#include <QFileInfo>
#include <QThread>

class SaveEngine : public QWidget
{
    Q_OBJECT

    QString savePath;

    int saveSize;
    int remainingCopySize;

    QVector<QFileInfo> filesList;
    QString dateTimeFormat;


public:
    explicit SaveEngine(QWidget *parent = nullptr);

//    void setSavePath(QString const path);
//    QString getSavePath();

//    int getSaveSize();

//    void setFilesList(QVector<QFileInfo> const &_filesList);
//    QVector<QFileInfo> getFilesList();

//    void setdateTimeFormat();
    enum SaveSettings
    {
        SORT_BY_DIRS = 1,
        RENAME = 2
    };
    int saveSettings = 0;
    void setSaveSettings(SaveSettings settings);
    void deleteSaveSettings(SaveSettings settings);

    enum FoldersFormats
    {
        YEAR = 1,
        MONTH = 2,
        DATE = 4,
        TYPE = 8,
        EXT = 16
    };
    int foldersFormat = 0;
    void setFoldersFormat(FoldersFormats format);
    void deleteFoldersFormat(FoldersFormats format);
    QString exampleFoldersFormat();

    enum RenameFormat
    {
        ONLY_NUM,
        CUSTOM_NAME,
        DATE_TIME,
        AS_IS
    };
    RenameFormat renameFormat;
    QString renameStr; // Пользовательское имя или формат даты/времени
    QString renameSplitter;
    int counterSize;
    void setRenameFormat(RenameFormat format, int _counterSize = 5, QString _renameStr = "", QString _splitter = "");
    QString exampleRenameFormat();

public slots:
//    void calcSaveSize();
//    int getAvailableSize();
//    void startCopy();

signals:
    void error(QString);
    void copyFinished();

};

#endif // SAVEENGINE_H

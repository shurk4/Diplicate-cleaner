#ifndef COMPAREENGINE_H
#define COMPAREENGINE_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QCryptographicHash>
#include <QDebug>
#include <QThread>

class CompareEngine : public QObject
{
    Q_OBJECT

    QString splitter = "/";

    QString path = "0";

    QMap<QByteArray, int> filesIdByHash; // Список ИД файлов по хешу, используется для поиска копий при заполнении filesById

    QVector<QVector<QFileInfo>> filesById; // Список файлов по ИД

    qint64 scannedSize = 0;
    qint64 originalsSize = 0;

    int scannedFilesNum = 0;
    int deletedFilesNum = 0;

    void listDirs(const QString _path); // просмотр директорий и запуск просмотра файлов
    void listFiles(const QString _path); // составление списков оригиналов и копий файлов в указанной директории
    void addFile(const QString filePath); // анализ файла и добавление в список оригиналов или копий
    void log(const QString text);
    QByteArray getHash(const QString filePath);
    bool fullCompare(QFileInfo fileInfo1, QFileInfo fileInfo2);

public:
    CompareEngine();

    void setPath(const QString _path);
    bool startCompare(); // запуск движка, если путь пустой (== "0") возвращает false
    QVector<QVector<QFileInfo>> getFilesList(); // получить список информации о файлах // Перемотрен

    void startFullCompare();
    int getScannedFilesNum();
    // Оригиналом считается файл с индексом 0, дубликаты с индексом 1+
    int getOrigNum();
    int getDupNum(int origId);

    qint64 getScannedSize();
    qint64 getOriginalsSize();

    void startDeleteDuplicates();

    QString getOrigName(int id);
    QFileInfo getOrigInfo(int id);

    QString getDupName(int origId, int dupId);
    QFileInfo getDubFileInfo(int origId, int dupId);

    enum Tolerance
    {
        MORE,
        LESS,
        EXACTLY
    };
    QVector<int> getIdsWithDup(int dupNum, Tolerance value); // создаёт и возвращает список id файлов имеющийх большее, меньшее или равное(value) количество копий указанное в dupNum. Если у файла нет копий он будет равен dupNum 1;
    int getDeletedFilesNum();

    void clear();
    void clearLists();

signals:
    void currentAction(QString);
    void finishedCompare();
    void finishedFull();
    void finishedDelete();

public slots:
    void runCompare();
    void runFullCompare();
    void runDelete();
};
#endif // COMPAREENGINE_H

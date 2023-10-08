#ifndef COMPAREENGINE_H
#define COMPAREENGINE_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QMap>
#include <QCryptographicHash>
#include <QDebug>

class CompareEngine : public QObject
{
    Q_OBJECT

    QString splitter = "/";

    QString path = "0";

    QMap<QByteArray, int> filesIdByHash; // Список ИД файлов по хешу
    QVector<QVector<QFileInfo>> filesById; // Список файлов по ИД

    int maxFileSize = 1000000000; // == 1ГБ макимальный размер файла для полного сравнения в байтах

    void listDirs(const QString _path); // просмотр директорий и запуск просмотра файлов
    void listFiles(const QString _path); // составление списков оригиналов и копий файлов в указанной директории
    void addFile(const QString filePath); // анализ файла и добавление в список оригиналов или копий
    void log(const QString text);
    QByteArray getHash(const QString filePath);
    bool fullCompare(const QString path, const int id);

public:
    CompareEngine();

    void setPath(const QString _path);
    bool startCompare(); // запуск движка, если путь пустой (== "0") возвращает false
    QVector<QVector<QFileInfo>> getFilesList(); // получить список ид файлов
    void clear();


signals:
    void currentAction(QString);
};

#endif // COMPAREENGINE_H

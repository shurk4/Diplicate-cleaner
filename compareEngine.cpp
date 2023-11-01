#include "compareEngine.h"

CompareEngine::CompareEngine()
{

}

void CompareEngine::setMultiThread(const bool arg)
{
    multiThread = arg;
}

void CompareEngine::setPath(const QString _path)
{
    path = _path;
}

void CompareEngine::log(const QString text)
{
    qDebug() << text;
    emit currentAction(text);
}

void CompareEngine::clear()
{
    path.clear();
    clearLists();
}

void CompareEngine::clearLists()
{
    filesById.clear();
    filesIdByHash.clear();
}

void CompareEngine::runCompare()
{
    qDebug() << "Engine thread: " << QThread::currentThreadId();
    startCompare();
    emit finishedCompare();
}

void CompareEngine::runFullCompare()
{
    startFullCompare();
    emit finishedFull();
}

void CompareEngine::runDelete()
{
    startDeleteDuplicates();
    emit finishedDelete();
}

void CompareEngine::info(QString type, qint64 size)
{
    if(type == "orig")
    {

        originalsSize += size;
    }
    else if(type == "scan")
    {
        scannedSize += size;
    }
}

QByteArray CompareEngine::getHash(const QString filePath)
{
    QFile file(filePath);
    if(file.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        while(!file.atEnd()){
            hash.addData(file.read(8192));
        }
        return hash.result();
    }
    return QByteArray();
}

bool CompareEngine::startCompare()
{
    log("Directory analysis started\n");
    if(path != "0")
    {
        listDirs(path);

        log("\n--- Searsh for duplicates");

        for(auto i : filesIdByHash.keys())
        {
            int id = filesIdByHash[i];
            if(filesById[id].size() > 1)
            {
                log("HASH:");
                log(i.toHex());
                log("FILES:");
                for(auto j : filesById[id])
                {
                    log(j.absoluteFilePath());
                }
                log("");
            }
        }
        return true;
    }

    return false;
}

QVector<QVector<QFileInfo> > CompareEngine::getFilesList() // Пересмотрен
{
    return filesById;
}

// Побитовое сравнение ранее отобранных файлов
void CompareEngine::startFullCompare()
{
    log("\n--- Full compare duplicate files started");
    for(int i = 0; i < filesById.size(); i++)
    {
        if(filesById[i].size() > 1)
        {
            for(int j = 1; j < filesById[i].size(); j++)
            {
                // Если файлы разные, удаляем его из копий и добавляем в основной список
                if(!fullCompare(filesById[i][0], filesById[i][j]))
                {
                    log("Not equal file detected: " + filesById[i][j].filePath());
                    QVector<QFileInfo> temp;
                    temp.push_back(filesById[i][j]);
                    filesById.push_back(temp);

                    filesById[i].erase(filesById[i].begin() + j);
                    j--;
                }
            }
        }
    }
}

bool CompareEngine::fullCompare(QFileInfo fileInfo1, QFileInfo fileInfo2)
{
    log("Compare files: " + fileInfo1.absoluteFilePath() + " & " + fileInfo2.absoluteFilePath());
    QFile file1(fileInfo1.absoluteFilePath());
    QFile file2(fileInfo2.absoluteFilePath());

    if(file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly))
    {
        const uint bufSize = 1024;
        char buf1[bufSize];
        char buf2[bufSize];

        while(file1.read(buf1, bufSize) && file2.read(buf2, bufSize))
        {
            for(int i = 0; i < bufSize; i++)
            {
                if(buf1[i] != buf2[i]) return false;
            }
        }
    }
    log("Files is equal");
    return true;
}

int CompareEngine::getScannedFilesNum()
{
    return scannedFilesNum;
}

int CompareEngine::getOrigNum()
{
    return filesById.size();
}

int CompareEngine::getDupNum(int origId)
{
    return filesById[origId].size();
}

qint64 CompareEngine::getScannedSize()
{
    return scannedSize;
}

qint64 CompareEngine::getOriginalsSize()
{
    return originalsSize;
}

QString CompareEngine::getOrigName(int id)
{
    return filesById[id][0].fileName();
}

QFileInfo CompareEngine::getOrigInfo(int id)
{
    return filesById[id][0];
}

QString CompareEngine::getDupName(int origId, int dupId)
{
    return filesById[origId][dupId].fileName();
}

QFileInfo CompareEngine::getDubFileInfo(int origId, int dupId)
{
    return filesById[origId][dupId];
}

QVector<QFileInfo> CompareEngine::getOrigList()
{
    QVector<QFileInfo> result;
    result.reserve(filesById.size());
    for(auto i : filesById)
    {
        result.push_back(i[0]);
    }
    return result;
}

QVector<int> CompareEngine::getIdsWithDup(int dupNum, Tolerance value)
{
    QVector<int> result;
    for(int i = 0; i < filesById.size(); i++)
    {
        switch (value) {
        case MORE:
            if(filesById[i].size() > dupNum) result.push_back(i);
            break;

        case LESS:
            if(filesById[i].size() < dupNum) result.push_back(i);
            break;

        case EXACTLY:
            if(filesById[i].size() == dupNum) result.push_back(i);
            break;
        }
    }
    return result;
}

void CompareEngine::startDeleteDuplicates()
{
    for(auto &i : filesById)
    {
        if(i.size() > 1)
        {
            for(int j = i.size() - 1; j > 0; j--)
            {
                QFile::remove(i[j].absoluteFilePath());
                i.pop_back();
            }
        }
    }
}

int CompareEngine::getDeletedFilesNum()
{
    return deletedFilesNum;
}

void CompareEngine::listDirs(const QString _path)
{
    QStringList dirsList = QDir(_path).entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);
    for(const auto &i : dirsList)
    {
        QString currentDir = _path + splitter + i;
        log("Dir: " + currentDir);
        listDirs(currentDir);
    }

    listFiles(_path);
}

void CompareEngine::listFiles(const QString _path)
{
    QStringList filesList = QDir(_path).entryList(QDir::Files | QDir::Hidden);
    for(const auto &i : filesList)
    {
        QString currentFile = _path + splitter + i;
        log("   File: " + currentFile);

        if(multiThread)
        {
            HashComparer *addFileThread = new HashComparer(currentFile, &mutex, &filesIdByHash, &filesById);
            addFileThread->setAutoDelete(true);
            QThreadPool::globalInstance()->start(addFileThread);
        }
        else addFile(currentFile);
        scannedFilesNum++;
    }
    QThreadPool::globalInstance()->waitForDone();
}

void CompareEngine::addFile(const QString filePath)
{
    QByteArray hash = getHash(filePath); // Получаем хеш файла
    QFileInfo fileInfo(filePath); // получаем информацию о файле

    if(filesIdByHash.contains(hash))
    {
        // Если такой хеш файла уже имеется, добавляем в ид списока файлов текущий файл
        // Если файл изменён раньше чем первый в списке, добавляем в начало списка
        if(filesById[filesIdByHash[hash]][0].lastModified().toMSecsSinceEpoch() < fileInfo.lastModified().toMSecsSinceEpoch())
        {
            filesById[filesIdByHash[hash]].push_back(fileInfo);
        }
        else
        {
            filesById[filesIdByHash[hash]].push_front(fileInfo);
        }
        scannedSize += fileInfo.size();
    }
    else
    {
        // Иначе добавляем новый ид в список файлов
        QVector<QFileInfo> tempFileList;
        tempFileList.push_back(fileInfo);
        filesById.push_back(tempFileList);
        filesIdByHash[hash] = filesById.size()-1;
        originalsSize += fileInfo.size();
    }
}

HashComparer::HashComparer(const QString _filePath, QMutex *_mutex, QMap<QByteArray, int> *_filesIdByHash, QVector<QVector<QFileInfo> > *_filesById)
    : filePath(_filePath), mutex(_mutex), filesIdByHash(_filesIdByHash), filesById(_filesById)
{

}

QByteArray HashComparer::getHash()
{
    QFile file(filePath);
    if(file.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        while(!file.atEnd()){
            hash.addData(file.read(8192));
        }
        return hash.result();
    }
    return QByteArray();
}

void HashComparer::run()
{
    QByteArray hash = getHash(); // Получаем хеш файла
    QFileInfo fileInfo(filePath); // получаем информацию о файле

    mutex->lock();
    if(filesIdByHash->contains(hash))
    {
        // Если такой хеш файла уже имеется, добавляем в ид списока файлов текущий файл
        // Если файл изменён раньше чем первый в списке, добавляем в начало списка
        if((*filesById)[filesIdByHash->value(hash)][0].lastModified().toMSecsSinceEpoch() < fileInfo.lastModified().toMSecsSinceEpoch())
        {
            (*filesById)[filesIdByHash->value(hash)].push_back(fileInfo);
        }
        else
        {
            (*filesById)[filesIdByHash->value(hash)].push_front(fileInfo);
        }
        emit info("scan", fileInfo.size());
    }
    else
    {
        // Иначе добавляем новый ид в список файлов
        QVector<QFileInfo> tempFileList;
        tempFileList.push_back(fileInfo);
        filesById->push_back(tempFileList);
        filesIdByHash->insert(hash, filesById->size()-1);
        emit info("orig", fileInfo.size());
    }
    mutex->unlock();
}

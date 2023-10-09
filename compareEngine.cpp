#include "compareEngine.h"

CompareEngine::CompareEngine()
{

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
    filesById.clear();
    filesIdByHash.clear();
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
    if(path != "0")
    {
        listDirs(path);

        log("\n\n!!! --- RESULT --- !!!");

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
        log("File: " + currentFile);
        addFile(currentFile);
        scannedFilesNum++;
    }
}

void CompareEngine::addFile(const QString filePath)
{
    QByteArray hash = getHash(filePath); // Получаем хеш файла
    QFileInfo fileInfo(filePath); // получаем информацию о файле

    if(filesIdByHash.contains(hash))
    {
        // Если такой хеш файла уже имеется, добавляем в ид списока файлов текущий файл
        filesById[filesIdByHash[hash]].push_back(fileInfo);
    }
    else
    {
        // Иначе добавляем новый ид в список файлов
        QVector<QFileInfo> tempFileList;
        tempFileList.push_back(fileInfo);
        filesById.push_back(tempFileList);
        filesIdByHash[hash] = filesById.size()-1;
    }
}

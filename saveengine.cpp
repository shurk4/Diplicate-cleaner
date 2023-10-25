#include "saveEngine.h"

SaveEngine::SaveEngine(QWidget *parent)
    : QWidget{parent}
{

}

void SaveEngine::setSavePath(const QString &path)
{
    savePath = path;
}

int SaveEngine::getSaveNum()
{
    return filesList.size();
}

int SaveEngine::getSaveSize()
{
    return saveSize;
}

void SaveEngine::setFilesList(const QVector<QFileInfo> &_filesList)
{
    filesList = _filesList;
    for(const auto &i : filesList)
    {
        saveSize += i.size();
    }
}

void SaveEngine::setSaveSettings(SaveSettings settings)
{
    saveSettings |= settings;
}

void SaveEngine::deleteSaveSettings(SaveSettings settings)
{
    saveSettings &= (~settings);
}

void SaveEngine::setFoldersFormat(FoldersFormats format)
{
    foldersFormat |= format;
}

void SaveEngine::deleteFoldersFormat(FoldersFormats format)
{
    foldersFormat &= (~format);
}

void SaveEngine::resetFoldersFormat()
{
    foldersFormat = 0;
}

QString SaveEngine::exampleFoldersFormat()
{
    QString result = "/";
    if(foldersFormat & YEAR) result += "YEAR/";
    if(foldersFormat & MONTH) result += "MONTH/";
    if(foldersFormat & DATE) result += "DATE/";
    if(foldersFormat & TYPE) result += "TYPE/";
    if(foldersFormat & EXT) result += "EXT/";
    return result;
}

void SaveEngine::setRenameFormat(RenameFormat format, int _counterSize, QString _splitter, QString _renameStr)
{
    renameFormat = format;
    counterSize = _counterSize;
    renameStr = _renameStr;
    renameSplitter = _splitter;
}

QString SaveEngine::exampleRenameFormat()
{
    QString name;
    if(saveSettings & RENAME)
    {
        if(renameFormat & CUSTOM_NAME)
        {
            name = renameStr + renameSplitter;
        }
        else if(renameFormat & DATE_TIME)
        {
            name = QDateTime::currentDateTime().toLocalTime().toString(renameStr);
        }
        name += QString::number(1).rightJustified(counterSize, '0');
    }
    else name = "fileName";

    return name;
}

bool SaveEngine::ready()
{
    return !savePath.isEmpty();
}

void SaveEngine::resetCounters()
{
    counters.clear();
}

void SaveEngine::startCopy()
{
    if(!ready())
    {
        emit error("Не выбран каталог для копирования!");
        return;
    }
    if(filesList.isEmpty())
    {
        emit error("Нечего копировать!");
    }

    int percentSize = saveSize / 100;

    for(const auto &i : filesList)
    {
        QString filePath = savePath + "/";
        if(saveSettings & SORT_BY_DIRS)
        {
            if(foldersFormat & YEAR)
            {
                filePath += i.lastModified().toString("yyyy") + "/";
                if(!QDir(filePath).exists())
                {
                    QDir(i.lastModified().toString("yyyy")).mkdir(filePath);
                }
            }
            if(foldersFormat & MONTH)
            {
                filePath += i.lastModified().toString("MM") + "/";
                if(!QDir(filePath).exists())
                {
                    QDir(i.lastModified().toString("MM")).mkdir(filePath);
                }
            }
            if(foldersFormat & DATE)
            {
                filePath += i.lastModified().toString("dd") + "/";
                if(!QDir(filePath).exists())
                {
                    QDir(i.lastModified().toString("dd")).mkdir(filePath);
                }
            }
            if(foldersFormat & TYPE)
            {
                filePath += "uncnownType/";
                if(!QDir(filePath).exists())
                {
                    QDir("uncnownType").mkdir(filePath);
                }
            }
            if(foldersFormat & EXT)
            {
                filePath += i.suffix() + "/";
                if(!QDir(filePath).exists())
                {
                    QDir(i.suffix()).mkdir(filePath);
                }
            }
        }
//-----------------------------
        if(!counters.contains(filePath))
        {
            counters[filePath] = 1;
        }

        QString name = i.completeBaseName();
        if(saveSettings & RENAME)
        {
            if(renameFormat & CUSTOM_NAME)
            {
                name = renameStr + QString::number(counters[filePath]).rightJustified(counterSize, '0');
                counters[filePath]++;
            }
            else if(renameFormat & DATE_TIME)
            {
                name = i.lastModified().toString(renameStr);
            }
            else
            {
                name = QString::number(counters[filePath]).rightJustified(counterSize, '0');
                counters[filePath]++;
            }
        }

//        filePath += name;
        QString fullFilePath = filePath + name + "."  + i.suffix().toLower();

        QFile fileToSave(i.absoluteFilePath());

        if(QFile(fullFilePath).exists())
        {
            if(counters.contains(fullFilePath))
            {
                counters[fullFilePath]++;
            }
            else
            {
                counters[fullFilePath] = 1;
            }

            fullFilePath = filePath + name + renameSplitter + QString::number(counters[fullFilePath]).rightJustified(counterSize, '0') + "." + i.suffix().toLower();
            counters[fullFilePath]++;
        }

        if(!fileToSave.copy(fullFilePath)) emit error("Не сохранён");
        copiedSize += i.size();
        emit progress(copiedSize / percentSize);
    }
}

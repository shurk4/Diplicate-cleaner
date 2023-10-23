#include "saveEngine.h"

SaveEngine::SaveEngine(QWidget *parent)
    : QWidget{parent}
{

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

void SaveEngine::setRenameFormat(RenameFormat format, int _counterSize, QString _renameStr, QString _splitter)
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

#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Разобраться с ресайзом
// Добавить функцию отсеивания файлов со 100% совпадением

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&engine, &CompareEngine::currentAction, this, &MainWindow::getAction);
    origScene = new QGraphicsScene();
    ui->graphicsViewOrig->setScene(origScene);
    dupScene = new QGraphicsScene();
    ui->graphicsViewDup->setScene(dupScene);
    ui->textBrowserLog->hide();


    engine.moveToThread(&thread);
    connect(this, &MainWindow::runCompare, &engine, &CompareEngine::runCompare);
    connect(&engine, &CompareEngine::finishedCompare, this, &MainWindow::finishedCompare);
    connect(this, &MainWindow::runFull, &engine, &CompareEngine::runFullCompare);
    connect(&engine, &CompareEngine::finishedFull, this, &MainWindow::finishedFull);
    connect(this, &MainWindow::runDelete, &engine, &CompareEngine::runDelete);
    connect(&engine, &CompareEngine::finishedDelete, this, &MainWindow::finishedDelete);

    disableBasicActions();
    disableSaveActions();

    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setCurrentIndex(0);

    ui->radioButtonYY->setText(QDateTime::currentDateTime().toLocalTime().toString("yy"));
    ui->radioButtonYYYY->setText(QDateTime::currentDateTime().toLocalTime().toString("yyyy"));
    ui->radioButtonMM->setText(QDateTime::currentDateTime().toLocalTime().toString("MM"));
    ui->radioButtonMMM->setText(QDateTime::currentDateTime().toLocalTime().toString("MMM"));
    ui->radioButtonMMMM->setText(QDateTime::currentDateTime().toLocalTime().toString("MMMM"));
    hideSaveWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}
//Основные функции
void MainWindow::disableBasicActions()
{
    ui->analize->setDisabled(true);
    ui->fullCompare->setDisabled(true);
    ui->dupDelete->setDisabled(true);    
    ui->saveOrig->setDisabled(true);
}

void MainWindow::disableSaveActions()
{
    ui->radioButtonDateName->setDisabled(true);
    ui->radioButtonCustomName->setDisabled(true);
    ui->widgetRanameOther->setDisabled(true);
    ui->widgetRenameFormat->setDisabled(true);
    ui->lineEditCustomName->setDisabled(true);
}

void MainWindow::clearAll()
{
    engine.clear();
    filesList.clear();

    ui->textBrowserLog->clear();
    ui->labelSourcePath->clear();
    ui->labelSavePath->clear();

    sourcePath.clear();
    savePath.clear();

    ui->saveOrig->setEnabled(false);

    clearOrig();
    clearDup();
    clearCounters();
}

void MainWindow::clearOrig()
{
    ui->graphicsViewOrig->scene()->clear();

    ui->listWidgetOrig->blockSignals(true);
    ui->listWidgetOrig->clear();
    ui->listWidgetOrig->blockSignals(false);

    ui->labelOrigChangeDate->clear();
    ui->labelOrigSaveDate->clear();
    ui->labelOrigPath->clear();
    ui->labelOrigSize->clear();
    ui->labelOrigType->clear();
    ui->labelOrigExt->clear();
}

void MainWindow::clearDup()
{
    ui->graphicsViewDup->scene()->clear();

    ui->listWidgetDup->blockSignals(true);
    ui->listWidgetDup->clear();
    ui->listWidgetDup->blockSignals(false);

    ui->labelDupType->clear();
    ui->labelDupChangeDate->clear();
    ui->labelDupSaveDate->clear();
    ui->labelDupExt->clear();
    ui->labelDupPath->clear();
    ui->labelDupSize->clear();
}

void MainWindow::clearCounters()
{
    filesWithDup = 0;
    files2Dup = 0;
    files3Dup = 0;
    filesMoreDup = 0;
    ui->labelFilesNum->clear();
    ui->labelFilesWithDup->clear();
    ui->label2Dup->clear();
    ui->label3Dup->clear();
    ui->labelMoreDup->clear();
}

void MainWindow::showFilesList()
{
    clearOrig();
    clearDup();
    clearCounters();

    int filesNum = engine.getOrigNum();

    for(int i = 0; i < filesNum; i++)
    {
        QListWidgetItem *item = new QListWidgetItem;
        item->setText(engine.getOrigName(i));

        int dupNum = engine.getDupNum(i);
        if(dupNum > 1)
        {
            filesWithDup++;
        }
        if(dupNum == 2)
        {
            files2Dup++;
            item->setBackground(Qt::green);
        }
        else if(dupNum == 3)
        {
            files3Dup++;
            item->setBackground(Qt::yellow);
        }
        else if(dupNum > 3)
        {
            filesMoreDup++;
            item->setBackground(Qt::red);
        }
        ui->listWidgetOrig->addItem(item);
        ui->labelFilesNum->setText(QString::number(engine.getScannedFilesNum()));
        ui->labelFilesWithDup->setText(QString::number(filesWithDup));
        ui->label2Dup->setText(QString::number(files2Dup));
        ui->label3Dup->setText(QString::number(files3Dup));
        ui->labelMoreDup->setText(QString::number(filesMoreDup));
    }
    ui->listWidgetOrig->setCurrentRow(0);
    if(filesWithDup == 0)
    {
        QMessageBox::information(this, "Ура!", "В указанной директории дубликатов не обнаружено.\n"
                                               "Можно создать обработанную копию папки нажав \"Сохранить оригиналы\"");
        ui->fullCompare->setEnabled(false);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->graphicsViewOrig->scene()->clear();
    showImage(ui->labelOrigPath->text(), ui->graphicsViewOrig, origScene);
    ui->graphicsViewDup->scene()->clear();
    showImage(ui->labelDupPath->text(), ui->graphicsViewDup, dupScene);
    QWidget::resizeEvent(event);
}

void MainWindow::getAction(QString act)
{
    ui->textBrowserLog->append(act);
    ui->statusBar->showMessage(act);
}

void MainWindow::showImage(const QString &path, QGraphicsView *view, QGraphicsScene *scene)
{
    QImage image(path);
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image.scaled(view->size(), Qt::KeepAspectRatio)));
    scene->addItem(item);
}
//Основные функции
//Меню
void MainWindow::openFolder(TAB tab)
{
    QString path = QFileDialog::getExistingDirectory(this, "Выбор расположения файлов");
    if(!path.isEmpty())
    {
        switch (tab) {
        case BASIC:
            clearAll();
            disableBasicActions();
            ui->labelSourcePath->setText(path);
            sourcePath = path;
            engine.setPath(path);
            ui->analize->setEnabled(true);
            break;
        case SAVE:
            ui->labelSavePath->setText(path);
            savePath = path;
            ui->pushButtonStartSave->setEnabled(true);
            break;
        }
    }
}

void MainWindow::analyze()
{
    engine.clearLists();
    qDebug() << "MainWindow thread: " << QThread::currentThreadId();
    if(sourcePath.isEmpty())
    {
        QMessageBox::warning(this, "!", "Не указан путь к файлам!");
        return;
    }

    thread.start();
    emit runCompare();
    ui->analize->setEnabled(false);
}

void MainWindow::finishedCompare()
{
    thread.terminate();
    ui->statusBar->showMessage("Анализ выбранной директории закончен");
    ui->fullCompare->setEnabled(true);
    ui->saveOrig->setEnabled(true);
    showFilesList();
}

void MainWindow::fullCompare()
{
    thread.start();
    emit runFull();
    ui->fullCompare->setEnabled(false);
}

void MainWindow::finishedFull()
{
    showFilesList();
    thread.terminate();
    ui->statusBar->showMessage("Полная проверка дубликатов закончена");
    ui->dupDelete->setEnabled(true);
}

void MainWindow::dupDelete()
{
    thread.start();
    emit runDelete();
    ui->dupDelete->setEnabled(false);
}

void MainWindow::finishedDelete()
{
    QMessageBox::information(this, "Удаление завершено!", "Удаление дубликатов завершено!");
    ui->statusBar->showMessage("Все дубликаты удалены");
    showFilesList();
}

void MainWindow::saveFiles()
{

}

void MainWindow::hideSaveWidgets()
{
    ui->widgetYearFormat->hide();
    ui->widgetMonthFormat->hide();
    ui->widgetTimeFormat->hide();
}

void MainWindow::on_pushButtonRotateLeft_clicked()
{
    ui->graphicsViewOrig->rotate(-90);
    ui->graphicsViewDup->rotate(-90);
}

void MainWindow::on_pushButtonRotateRight_clicked()
{
    ui->graphicsViewOrig->rotate(90);
    ui->graphicsViewDup->rotate(90);
}

void MainWindow::on_openDir_triggered()
{
    openFolder(BASIC);
}

void MainWindow::on_analize_triggered()
{
    analyze();
}

void MainWindow::on_fullCompare_triggered()
{
    fullCompare();
}

void MainWindow::on_dupDelete_triggered()
{
    dupDelete();
}

void MainWindow::on_showLog_triggered(bool checked)
{
    if(checked) ui->textBrowserLog->show();
    else ui->textBrowserLog->hide();
}
//Меню

void MainWindow::on_listWidgetOrig_currentRowChanged(int currentRow)
{
    clearDup();
    selectedFileId = currentRow; // сделать глобально
    QFileInfo fileInfo = engine.getOrigInfo(currentRow);

    ui->labelOrigPath->setText(fileInfo.absoluteFilePath());
    ui->labelOrigChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelOrigSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelOrigSize->setText(QString::number(fileInfo.size()));
    ui->labelOrigType->setText(fileInfo.suffix());
    ui->labelOrigExt->setText(fileInfo.suffix());

    int dupNum = engine.getDupNum(currentRow);

    if(dupNum > 1)
    {
        for(int i = 1; i < dupNum; i++)
        {
            ui->listWidgetDup->addItem(engine.getDupName(currentRow, i));
        }
        ui->listWidgetDup->setCurrentRow(0);
    }

    showImage(fileInfo.absoluteFilePath(), ui->graphicsViewOrig, origScene);
}

void MainWindow::on_listWidgetDup_currentRowChanged(int currentRow)
{
    selectedDupFileId = currentRow + 1;
    QFileInfo fileInfo = engine.getDubFileInfo(selectedFileId, selectedDupFileId);

    ui->labelDupPath->setText(fileInfo.absoluteFilePath());
    ui->labelDupChangeDate->setText(fileInfo.lastModified().toString());
    ui->labelDupSaveDate->setText(fileInfo.birthTime().toString());
    ui->labelDupSize->setText(QString::number(fileInfo.size()));
    ui->labelDupType->setText(fileInfo.suffix());
    ui->labelDupExt->setText(fileInfo.suffix());

    showImage(fileInfo.absoluteFilePath(), ui->graphicsViewDup, dupScene);
}

void MainWindow::on_saveOrig_triggered()
{
    ui->tabWidget->setCurrentIndex(1);
    ui->toolBarMain->hide();
    if(ui->saveOrig->isEnabled())
    {
        ui->statusBar->showMessage("Для настройки параметров сохранения выберите целоевую директорию.");
    }
    else
    {
        ui->statusBar->showMessage("Настройте параметры сохранения и нажмите \"Старт\".");
    }
}
void MainWindow::on_checkBoxSortDirs_stateChanged(int arg1)
{
        ui->checkBoxSortExtension->setEnabled(arg1);
        ui->checkBoxSortFileType->setEnabled(arg1);
        ui->checkBoxSortDates->setEnabled(arg1);
        ui->checkBoxSortMonths->setEnabled(arg1);
        ui->checkBoxSortYears->setEnabled(arg1);
}

void MainWindow::on_pushButtonCancelSave_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
    ui->toolBarMain->show();
    ui->statusBar->clearMessage();
}

void MainWindow::on_pushButtonSaveFolder_clicked()
{
    openFolder(SAVE);
}

void MainWindow::on_checkBoxSortDates_stateChanged(int arg1)
{
    showExample();
}

void MainWindow::on_checkBoxSortYears_stateChanged(int arg1)
{
    showExample();
}

void MainWindow::on_checkBoxSortMonths_stateChanged(int arg1)
{
    showExample();
}

void MainWindow::on_checkBoxSortFileType_stateChanged(int arg1)
{
    showExample();
}

void MainWindow::on_checkBoxSortExtension_stateChanged(int arg1)
{
    showExample();
}
// Показывает пример сохранения файла
void MainWindow::showExample()
{
    QString example = "/";
    saveNameExample = createSaveName();
    if(ui->checkBoxSortDirs->isChecked())
    {
        QDateTime dateTime;
        dateTime = QDateTime::currentDateTime().toLocalTime();
        if(ui->checkBoxSortYears->isChecked())
        {
            example += dateTime.toString("yyyy") + "/";
        }
        if(ui->checkBoxSortMonths->isChecked())
        {
            example += dateTime.toString("MM") + "/";
        }
        if(ui->checkBoxSortDates->isChecked())
        {
            example += dateTime.toString("dd") + "/";
        }
        if(ui->checkBoxSortFileType->isChecked())
        {
            example += "images/";
        }
        if(ui->checkBoxSortExtension->isChecked())
        {
            example += "jpeg/";
        }
    }
    example += saveNameExample + ".jpg";
    ui->labelExample->setText(example);
}

QString MainWindow::createSaveName(QDateTime dateTime)
{
    QString name;
    if(ui->checkBoxRename->isChecked())
    {
        if(ui->radioButtonCustomName->isChecked())
        {
            name = ui->lineEditCustomName->text() + saveSplitter + ui->comboBoxNumeric->currentText();
        }
        else
        {
            saveDateTimeFormat = "";
            if(ui->checkBoxYear->isChecked())
            {
                if(ui->radioButtonYYYY->isChecked()) saveDateTimeFormat += "yyyy";
                else saveDateTimeFormat += "yy";
                saveDateTimeFormat += saveSplitter;
            }
            if(ui->checkBoxMonth->isChecked())
            {
                if(ui->radioButtonMMMM->isChecked()) saveDateTimeFormat += "MMMM";
                else if(ui->radioButtonMMM->isChecked()) saveDateTimeFormat += "MMM";
                else saveDateTimeFormat += "MM";
                saveDateTimeFormat += saveSplitter;
            }
            if(ui->checkBoxDate->isChecked())
            {
                saveDateTimeFormat += "dd";
                saveDateTimeFormat += saveSplitter;
            }
            if(ui->checkBoxTime->isChecked())
            {
                if(ui->radioButtonHHMM->isChecked()) saveDateTimeFormat += "hhmm";
                if(ui->radioButtonHHMMSS->isChecked()) saveDateTimeFormat += "hhmmss";
                saveDateTimeFormat += saveSplitter;
            }
            name = dateTime.toString(saveDateTimeFormat);
        }
    }
    else name = "fileName";

    return name;
}

void MainWindow::on_checkBoxRename_stateChanged(int arg1)
{
    if(arg1)
    {
        saveNameExample = createSaveName();
    }
    else
    {
        ui->widgetRenameFormat->setDisabled(true);
        ui->lineEditCustomName->setDisabled(true);
    }

    ui->widgetRanameOther->setEnabled(arg1);
    ui->radioButtonDateName->setEnabled(arg1);
    ui->radioButtonCustomName->setEnabled(arg1);

    ui->radioButtonCustomName->setChecked(true);
    ui->lineEditCustomName->setEnabled(arg1);
    showExample();
}

void MainWindow::on_radioButtonDateName_clicked()
{
    if(ui->radioButtonDateName->isChecked())
    {
        ui->widgetRenameFormat->setEnabled(true);
        ui->lineEditCustomName->setEnabled(false);
    }
}

void MainWindow::on_radioButtonCustomName_clicked()
{
    if(ui->radioButtonCustomName->isChecked())
    {
        ui->widgetRenameFormat->setEnabled(false);
        ui->lineEditCustomName->setEnabled(true);
    }
    showExample();
}

void MainWindow::on_checkBoxYear_stateChanged(int arg1)
{
    if(arg1) ui->widgetYearFormat->show();
    else ui->widgetYearFormat->hide();
    showExample();
}

void MainWindow::on_checkBoxMonth_stateChanged(int arg1)
{
    if(arg1) ui->widgetMonthFormat->show();
    else ui->widgetMonthFormat->hide();
    showExample();
}

void MainWindow::on_lineEditCustomName_textEdited(const QString &arg1)
{
    showExample();
}

void MainWindow::on_comboBoxSplitter_activated(int index)
{
    if(index == 0)
    {
        saveSplitter = " ";
    }
    else if(index == 1)
    {
        saveSplitter = "";
    }
    else
    {
        saveSplitter = ui->comboBoxSplitter->currentText();
    }
    showExample();
}

void MainWindow::on_comboBoxNumeric_activated(int index)
{
    showExample();
}

void MainWindow::on_checkBoxTime_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->widgetTimeFormat->show();
        ui->radioButtonHHMM->setText(QDateTime::currentDateTime().toLocalTime().toString("hh:mm"));
        ui->radioButtonHHMMSS->setText(QDateTime::currentDateTime().toLocalTime().toString("hh:mm:ss"));
    }
    else ui->widgetTimeFormat->hide();
    showExample();
}

void MainWindow::on_radioButtonYYYY_clicked()
{
    showExample();
}

void MainWindow::on_radioButtonYY_clicked()
{
    showExample();
}

void MainWindow::on_radioButtonMM_clicked()
{
    showExample();
}

void MainWindow::on_radioButtonMMM_clicked()
{
    showExample();
}

void MainWindow::on_radioButtonMMMM_clicked()
{
    showExample();
}

void MainWindow::on_radioButtonHHMM_clicked()
{
    showExample();
}

void MainWindow::on_radioButtonHHMMSS_clicked()
{
    showExample();
}

void MainWindow::on_checkBoxDate_stateChanged(int arg1)
{
    showExample();
}

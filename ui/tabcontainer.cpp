#include "tabcontainer.hpp"
#include "ui_tabcontainer.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QFileInfo>
#include <exception>

using namespace std;

TabContainer::TabContainer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TabContainer)
{
    ui->setupUi(this);
    ui->avrdudeProgress->setVisible(false);

    dude = new Avrdude(this);
    mcuList = new McuList();

    setupMcuSelectCombo();
    connect(dude,SIGNAL(data_found(QString)),this,SLOT(readAvrdudeData(QString)));
    connect(dude,SIGNAL(progress_report(int)),this,SLOT(showAvrdudeProgress(int)));
    connect(dude,SIGNAL(error_report()),this,SLOT(showAvrdudeError()));
    connect(dude,SIGNAL(lfuse_read(QString)),this,SLOT(showAvrdudeLFuse(QString)));
    connect(dude,SIGNAL(hfuse_read(QString)),this,SLOT(showAvrdudeHFuse(QString)));
    connect(dude,SIGNAL(efuse_read(QString)),this,SLOT(showAvrdudeEFuse(QString)));
    connect(dude,SIGNAL(end_of_progress()),this,SLOT(hideAvrdudeProgressBar()));
}

TabContainer::~TabContainer()
{
    delete ui;
}


void TabContainer :: setupMcuSelectCombo()
{
    try {
        mcuList->parse_config();
    } catch (exception &ex) {
        QMessageBox::warning(this, "Error", QString(ex.what()));
    }
    ui->mcuSelectCombo->addItems(mcuList->mcu_labels);
    ui->mcuSelectCombo->setCurrentIndex(mcuList->default_label());
}

void TabContainer::on_mcuSelectCombo_currentIndexChanged(int index)
{
    Mcu mcu = mcuList->mcus_by_label[mcuList->mcu_labels[index]];
    ui->mcuIdLabel->setText(mcu.id);
    ui->mcuDescLabel->setText(mcu.desc);
    ui->mcuSigLabel->setText(mcu.signature);
    ui->mcuFlashSzLabel->setText(mcu.flash_size);
    ui->mcuEepSzLabel->setText(mcu.eeprom_size);
}

void TabContainer::on_writeLFuseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    if(dude->isValidFuse(ui->lfuseValue->text()))
    {
        if(QMessageBox::information(
                    this,
                    "Write Low Fuse",
                    "Are you sure to write the low fuse to 0x" + ui->lfuseValue->text().toUpper(),
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel) == QMessageBox::Ok) {
            dude->writeLFuse(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id,
                             ui->lfuseValue->text());
        }
    }else
    {
        QMessageBox::warning(this,
                             "Error",
                             "Please check the Low Fuse value again.");
    }
}

void TabContainer :: readAvrdudeData(QString data)
{
    ui->console->append(data);
    QScrollBar *sb = ui->console->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void TabContainer :: showAvrdudeProgress(int pos)
{
    if(pos && !ui->avrdudeProgress->isVisible())
        ui->avrdudeProgress->setVisible(true);
    if(pos >= 100)
        ui->avrdudeProgress->setVisible(false);
    ui->avrdudeProgress->setValue(pos);
}

void TabContainer :: showAvrdudeLFuse(QString fuse)
{
    ui->lfuseValue->setText(fuse);
}

void TabContainer :: showAvrdudeHFuse(QString fuse)
{
    ui->hfuseValue->setText(fuse);
}

void TabContainer :: showAvrdudeEFuse(QString fuse)
{
    ui->efuseValue->setText(fuse);
}

void TabContainer ::hideAvrdudeProgressBar()
{
    ui->avrdudeProgress->setVisible(false);
}

void TabContainer :: showAvrdudeError()
{
    if(ui->avrdudeProgress->isVisible())
        ui->avrdudeProgress->setVisible(false);
    QMessageBox::warning(this,
                         "Error",
                         "Something went wrong. Please check the console output.");
}

void TabContainer::on_openHexButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                "Select the hex file to write",
                                                qApp->applicationDirPath(),
                                                "(*.hex *.HEX)");
    if(!file.isEmpty())
    {
        dudeHexToWrite = file;
        updateHexFilePathLabel();
        ui->console->append(dudeHexToWrite+" selected to write");
    }
}

void TabContainer::updateHexFilePathLabel() {
    if(ui->showFullPathCheckbox->isChecked()) {
        ui->curHexFileLabel->setText(dudeHexToWrite);
    }else {
        QFileInfo info(dudeHexToWrite);
        ui->curHexFileLabel->setText(info.fileName());
    }
}

void TabContainer::on_writeFlashButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    dude->writeFlash(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id,dudeHexToWrite);
}

void TabContainer::on_readFlashButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    dude->readFlash(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id);
}

void TabContainer::on_saveHexButton_clicked()
{
    QString file = QFileDialog::getSaveFileName(this,
                                                "Save Flash data",
                                                qApp->applicationDirPath(),
                                                "(*.hex *.HEX)");
    if(!file.isEmpty())
    {
        if(!file.contains(QRegExp("\\.(hex|HEX)")))
            file += ".hex";
        if(!dude->saveFlash(file))
        {
            QMessageBox::warning(this,
                                 "Error",
                                 "Error saving hex file. Check proper permissions.");
        }
    }
}

void TabContainer::on_readLFuseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    dude->readLFuse(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id);
}

void TabContainer::on_readHFuseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    dude->readHFuse(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id);
}

void TabContainer::on_readEFuseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    dude->readEFuse(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id);
}

void TabContainer::on_writeHFuseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    if(dude->isValidFuse(ui->hfuseValue->text()))
    {
        if(QMessageBox::information(
                    this,
                    "Write High Fuse",
                    "Are you sure to write the high fuse to 0x" + ui->hfuseValue->text().toUpper(),
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel) == QMessageBox::Ok) {
            dude->writeHFuse(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id,
                             ui->hfuseValue->text());
        }
    }else
    {
        QMessageBox::warning(this,
                             "Error",
                             "Please check the High Fuse value again.");
    }
}

void TabContainer::on_writeEFuseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }

    if(dude->isValidFuse(ui->efuseValue->text()))
    {
        if(QMessageBox::information(
                    this,
                    "Write Ext. Fuse",
                    "Are you sure to write the extended fuse to 0x" + ui->efuseValue->text().toUpper(),
                    QMessageBox::Ok | QMessageBox::Cancel,
                    QMessageBox::Cancel) == QMessageBox::Ok) {
            dude->writeEFuse(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id,
                             ui->efuseValue->text());
        }
    }else
    {
        QMessageBox::warning(this,
                             "Error",
                             "Please check the Ext. Fuse value again.");
    }
}

void TabContainer::on_showFullPathCheckbox_stateChanged(int arg1)
{
    if(dudeHexToWrite.isEmpty()) {
        return;
    }

    qDebug() << "Show full hex path:" << arg1;

    updateHexFilePathLabel();
}

void TabContainer::on_chipEraseButton_clicked()
{
    if(ui->mcuSelectCombo->currentIndex()<0) {
        return;
    }


    if(QMessageBox::information(
                this,
                "Chip Erase",
                "Are you sure to erase the target chip (" + ui->mcuSelectCombo->currentText() + ") ?",
                QMessageBox::Ok | QMessageBox::Cancel,
                QMessageBox::Cancel) == QMessageBox::Ok) {
        dude->chipErase(mcuList->mcus_by_label[ui->mcuSelectCombo->currentText()].id);
    }
}

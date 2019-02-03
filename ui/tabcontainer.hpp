#ifndef TABCONTAINER_HPP
#define TABCONTAINER_HPP

#include <QDialog>
#include <QString>
#include <QStringList>
#include <QMap>

#include <Mcu.hpp>
#include <Avrdude.hpp>

namespace Ui {
    class TabContainer;
}

class TabContainer : public QDialog
{
        Q_OBJECT

    public:
        explicit TabContainer(QWidget *parent = 0);
        ~TabContainer();

    private slots:
        void readAvrdudeData(QString data);
        void showAvrdudeProgress(int);
        void showAvrdudeError();
        void showAvrdudeLFuse(QString);
        void showAvrdudeHFuse(QString);
        void showAvrdudeEFuse(QString);
        void hideAvrdudeProgressBar();
        void on_mcuSelectCombo_currentIndexChanged(int index);
        void on_writeLFuseButton_clicked();
        void on_openHexButton_clicked();
        void on_writeFlashButton_clicked();
        void on_readFlashButton_clicked();
        void on_saveHexButton_clicked();
        void on_readLFuseButton_clicked();
        void on_readHFuseButton_clicked();
        void on_readEFuseButton_clicked();

        void on_writeHFuseButton_clicked();

        void on_writeEFuseButton_clicked();

private:
            Ui::TabContainer *ui;


           void setupMcuSelectCombo();


           Avrdude * dude;
           McuList *mcuList;
           QString dudeHexToWrite;//LOL name for a hex file
};

#endif // TABCONTAINER_HPP

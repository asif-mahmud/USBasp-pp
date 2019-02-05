#ifndef AVRDUDE_HPP
#define AVRDUDE_HPP

#include <QProcess>
#include <QString>
#include <QRegExp>

class Avrdude : public QProcess
{
    Q_OBJECT
    public:
        explicit Avrdude(QObject *parent = 0);
        void setHexFile(QString filePath,bool writeFlag=true);//if false will be set to save file name
        void writeFlash(QString mcuLabel,QString file);
        void readFlash(QString mcuLabel);//returns tmp file name or empty string
        bool saveFlash(QString filePath);
        bool isValidFuse(QString fuse);
        void writeLFuse(QString mcuLabel,QString fuse);
        void writeHFuse(QString mcuLabel,QString fuse);
        void writeEFuse(QString mcuLabel,QString fuse);
        void readLFuse(QString);
        void readHFuse(QString);
        void readEFuse(QString);
        void chipErase(QString);
        static QRegExp prog_exp;
        static QRegExp fuse_exp;

    signals:
        void data_found(QString data);
        void progress_report(int value);
        void error_report();
        void lfuse_read(QString);
        void hfuse_read(QString);
        void efuse_read(QString);
        void end_of_progress();

    public slots:
        void rcvStdout();
        void rcvStderr();
        void rcvExitStatus(int);

    private:
        QString hexFileToWrite,hexFileToSave;
        QString avrdudeExe;
        bool writeFlag,readFlag,fuseOpFlag;
        bool lfuseReadFlag, hfuseReadFlag,efuseReadFlag;
        int progressPos;
};

#endif // AVRDUDE_HPP

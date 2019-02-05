#include <Avrdude.hpp>
#include <Mcu.hpp>
#include <QString>
#include <QApplication>
#include <QStringList>
#include <QDebug>
#include <QDir>

Avrdude::Avrdude(QObject *parent) :
    QProcess(parent)
{
#if defined(_WIN32)
    avrdudeExe = qApp->applicationDirPath() + "//avrdude.exe";
#elif defined(__unix__)
    avrdudeExe = "avrdude";
#endif
    writeFlag = false;
    readFlag = false;
    fuseOpFlag = false;
    connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(rcvStdout()));
    connect(this,SIGNAL(readyReadStandardError()),this,SLOT(rcvStderr()));
    connect(this,SIGNAL(finished(int)),this,SLOT(rcvExitStatus(int)));
}

//TODO : try non-greedy search
QRegExp Avrdude :: prog_exp = QRegExp("#");
QRegExp Avrdude :: fuse_exp = QRegExp("^:010{6,6}(.{2,2}).*$");

void Avrdude :: rcvStdout()
{
    QString data = this->readAllStandardOutput();
    if(Avrdude::prog_exp.indexIn(data) != -1 && !fuseOpFlag)//dont show the progress bar for fuse operation
    {
        if(readFlag)
            progressPos += 2;
        else
            progressPos ++;
        if(progressPos >= 100)
        {
            readFlag = false;
            writeFlag = false;
        }
//        qDebug()<<progressPos;
        emit progress_report(progressPos);
    }

    if(fuseOpFlag)
    {
//        qDebug()<<"[STDOUT]:<"<<data<<">";
        if(Avrdude::fuse_exp.indexIn(data) != -1)
        {
//            qDebug()<<Avrdude::fuse_exp.cap(1);
            if(lfuseReadFlag)
            {
                emit lfuse_read(Avrdude::fuse_exp.cap(1));
                lfuseReadFlag = false;
            }else if (hfuseReadFlag)
            {
                emit hfuse_read(Avrdude::fuse_exp.cap(1));
                hfuseReadFlag = false;
            }else if (efuseReadFlag)
            {
                emit efuse_read(Avrdude::fuse_exp.cap(1));
                efuseReadFlag = false;
            }
        }
    }
    emit data_found(data);
}

void Avrdude :: rcvStderr()
{
    QString data = this->readAllStandardError();
    if(Avrdude::prog_exp.indexIn(data) != -1 && !fuseOpFlag)
    {
        if(readFlag)
            progressPos += 2;
        else
            progressPos ++;
        if(progressPos >= 100)
        {
            readFlag = false;
            writeFlag = false;
        }
//        qDebug()<<progressPos;
//        qDebug() << Avrdude::prog_exp.captureCount();
//        qDebug()<<Avrdude::prog_exp.capturedTexts().count();
//        foreach(QString s,Avrdude::prog_exp.capturedTexts())
//            qDebug()<<s;
        emit progress_report(progressPos);
    }

    if(fuseOpFlag)
    {
//        qDebug()<<"[STDERR]:<"<<data<<">";
        if(Avrdude::fuse_exp.indexIn(data) != -1)
        {
//            qDebug()<<Avrdude::fuse_exp.cap(1);
            if(lfuseReadFlag)
            {
                emit lfuse_read(Avrdude::fuse_exp.cap(1));
                lfuseReadFlag = false;
            }else if (hfuseReadFlag)
            {
                emit hfuse_read(Avrdude::fuse_exp.cap(1));
                hfuseReadFlag = false;
            }else if (efuseReadFlag)
            {
                emit efuse_read(Avrdude::fuse_exp.cap(1));
                efuseReadFlag = false;
            }
        }
    }
    emit data_found(data);
}

void Avrdude ::rcvExitStatus(int statusCode)
{
    if(statusCode)
        emit error_report();
    if(fuseOpFlag)
        fuseOpFlag = false;//just reset it if it was a fuse operation
    emit end_of_progress();
}

bool Avrdude :: isValidFuse(QString fuse)
{
    if(fuse.isEmpty())
        return false;
    bool check = true;
//    int fInt = fuse.toInt(&check,16);
    fuse.toInt(&check, 16);
    return check;
}


void Avrdude :: setHexFile(QString filePath, bool writeFlag)
{
    if(writeFlag)
        hexFileToWrite = filePath;
    else
        hexFileToSave = filePath;
}

void Avrdude :: writeFlash(QString mcuId, QString file)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("flash:w:"+file+":i");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    writeFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude :: readFlash(QString mcuId)
{
#if defined(_WIN32)
    QString file = QDir::tempPath()+"\\"+mcuId+".hex";
#elif defined(__unix__)
    QString file = QDir::tempPath()+"//"+mcuId+".hex";
#endif
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-D");//needed for read opearation only
    args.append("-U");
    args.append("flash:r:"+file+":i");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    readFlag = true;
    progressPos =-1;
    hexFileToSave = file;
    qDebug()<<"temp file :"<<file;
    this->start(avrdudeExe,args);
}

void Avrdude :: readLFuse(QString mcuId)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("lfuse:r:-:i");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    readFlag = true;
    fuseOpFlag = true;
    lfuseReadFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude :: readHFuse(QString mcuId)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("hfuse:r:-:i");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    readFlag = true;
    fuseOpFlag = true;
    hfuseReadFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude :: readEFuse(QString mcuId)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("efuse:r:-:i");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    readFlag = true;
    fuseOpFlag = true;
    efuseReadFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude::writeLFuse(QString mcuId, QString fuse)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("lfuse:w:0x"+fuse+":m");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    writeFlag = true;
    fuseOpFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude::writeHFuse(QString mcuId, QString fuse)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("hfuse:w:0x"+fuse+":m");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    writeFlag = true;
    fuseOpFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude::chipErase(QString mcuId) {
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-e");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    writeFlag = true;
    fuseOpFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

void Avrdude::writeEFuse(QString mcuId, QString fuse)
{
    QStringList args;
    args.append("-c");
    args.append("usbasp");
    args.append("-p");
    args.append(mcuId);
    args.append("-U");
    args.append("efuse:w:0x"+fuse+":m");

    //Just for checking syntax
//    foreach(QString s,args)
//        qDebug()<<s;

    writeFlag = true;
    fuseOpFlag = true;
    progressPos =-1;
    this->start(avrdudeExe,args);
}

bool Avrdude :: saveFlash(QString filePath)
{
    if(hexFileToSave.isEmpty())
        return false;
    if(QFile::exists(filePath))
        QFile::remove(filePath);
    return QFile::copy(hexFileToSave,filePath);
}

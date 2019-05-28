#include "Avrdude.hpp"
#include <QDebug>
#include <QDir>

Avrdude::Avrdude(QObject *parent) : QProcess(parent) {
  m_curAction = NONE;
  connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(rcvStdout()));
  connect(this, SIGNAL(readyReadStandardError()), this, SLOT(rcvStderr()));
  connect(this, SIGNAL(finished(int)), this, SLOT(rcvExitStatus(int)));
}

// TODO : try non-greedy search
QRegExp Avrdude ::progExp = QRegExp("#");
QRegExp Avrdude ::fuseExp = QRegExp("^:010{6,6}(.{2,2}).*$");

QString Avrdude::avrdudeExePath() const { return m_avrdudeExePath; }

void Avrdude::setAvrdudeExePath(QString exePath) {
  if (!exePath.isEmpty() && exePath.compare(m_avrdudeExePath) != 0) {
    m_avrdudeExePath = exePath;
    emit avrdudeExePathChanged();
  }
}

void Avrdude::processConsoleData(QString data) {
  emit dataFound(data);  // let the gui know about the data string up front

  switch (m_curAction) {
    case READ_FLASH: {
      if (data.contains('#')) {
        m_progressPos += 2;
        emit progressReport(m_progressPos);
      }
    } break;
    case WRITE_FLASH: {
      if (data.contains('#')) {
        m_progressPos++;
        emit progressReport(m_progressPos);
      }
    } break;
    case READ_LOW_FUSE: {
    } break;
    case READ_HIGH_FUSE: {
    } break;
    case READ_EXT_FUSE: {
    } break;
    default:
      break;
  }
  qDebug() << "Progress pos:" << m_progressPos;
}

void Avrdude ::rcvStdout() {
  QString data = this->readAllStandardOutput();
  qDebug() << "[STDOUT]:<" << data << ">";

  if (Avrdude::progExp.indexIn(data) != -1 &&
      !fuseOpFlag)  // dont show the progress bar for fuse operation
  {
    if (readFlag)
      m_progressPos += 2;
    else
      m_progressPos++;
    if (m_progressPos >= 100) {
      readFlag = false;
      writeFlag = false;
    }
    //        qDebug()<<progressPos;
    emit progressReport(m_progressPos);
  }

  if (fuseOpFlag) {
    //        qDebug()<<"[STDOUT]:<"<<data<<">";
    if (Avrdude::fuseExp.indexIn(data) != -1) {
      //            qDebug()<<Avrdude::fuse_exp.cap(1);
      if (lfuseReadFlag) {
        emit lfuseRead(Avrdude::fuseExp.cap(1));
        lfuseReadFlag = false;
      } else if (hfuseReadFlag) {
        emit hfuseRead(Avrdude::fuseExp.cap(1));
        hfuseReadFlag = false;
      } else if (efuseReadFlag) {
        emit efuseRead(Avrdude::fuseExp.cap(1));
        efuseReadFlag = false;
      }
    }
  }
}

void Avrdude ::rcvStderr() {
  QString data = this->readAllStandardError();
  if (Avrdude::progExp.indexIn(data) != -1 && !fuseOpFlag) {
    if (readFlag)
      m_progressPos += 2;
    else
      m_progressPos++;
    if (m_progressPos >= 100) {
      readFlag = false;
      writeFlag = false;
    }
    //        qDebug()<<progressPos;
    //        qDebug() << Avrdude::prog_exp.captureCount();
    //        qDebug()<<Avrdude::prog_exp.capturedTexts().count();
    //        foreach(QString s,Avrdude::prog_exp.capturedTexts())
    //            qDebug()<<s;
    emit progressReport(m_progressPos);
  }

  if (fuseOpFlag) {
    //        qDebug()<<"[STDERR]:<"<<data<<">";
    if (Avrdude::fuseExp.indexIn(data) != -1) {
      //            qDebug()<<Avrdude::fuse_exp.cap(1);
      if (lfuseReadFlag) {
        emit lfuseRead(Avrdude::fuseExp.cap(1));
        lfuseReadFlag = false;
      } else if (hfuseReadFlag) {
        emit hfuseRead(Avrdude::fuseExp.cap(1));
        hfuseReadFlag = false;
      } else if (efuseReadFlag) {
        emit efuseRead(Avrdude::fuseExp.cap(1));
        efuseReadFlag = false;
      }
    }
  }
  emit dataFound(data);
}

void Avrdude ::rcvExitStatus(int statusCode) {
  if (statusCode != 0) emit errorReport();
  m_curAction = NONE;  // reset current action type
  emit endOfProgress();
}

bool Avrdude ::isValidFuse(QString fuse) {
  if (fuse.isEmpty()) return false;
  bool check = true;
  fuse.toInt(&check, 16);
  return check;
}

void Avrdude ::setHexFile(QString filePath, bool writeFlag) {
  if (writeFlag)
    m_hexFileToWrite = filePath;
  else
    m_hexFileToSave = filePath;
}

void Avrdude ::writeFlash(QString mcuId, QString file) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("flash:w:" + file + ":i");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = WRITE_FLASH;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude ::readFlash(QString mcuId) {
  QString file = QDir::tempPath() + QDir::separator() + mcuId + ".hex";

  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-D");  // needed for read opearation only
  args.append("-U");
  args.append("flash:r:" + file + ":i");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = READ_FLASH;
  m_progressPos = -1;
  m_hexFileToSave = file;
  qDebug() << "temp file :" << file;
  this->start(m_avrdudeExePath, args);
}

void Avrdude ::readLFuse(QString mcuId) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("lfuse:r:-:i");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = READ_LOW_FUSE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude ::readHFuse(QString mcuId) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("hfuse:r:-:i");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = READ_HIGH_FUSE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude ::readEFuse(QString mcuId) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("efuse:r:-:i");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = READ_EXT_FUSE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude::writeLFuse(QString mcuId, QString fuse) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("lfuse:w:0x" + fuse + ":m");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = WRITE_LOW_FUSE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude::writeHFuse(QString mcuId, QString fuse) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("hfuse:w:0x" + fuse + ":m");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = WRITE_HIGH_FUSE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude::chipErase(QString mcuId) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-e");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = CHIP_ERASE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

void Avrdude::writeEFuse(QString mcuId, QString fuse) {
  QStringList args;
  args.append("-c");
  args.append("usbasp");
  args.append("-p");
  args.append(mcuId);
  args.append("-U");
  args.append("efuse:w:0x" + fuse + ":m");

  // Just for checking syntax
  //    foreach(QString s,args)
  //        qDebug()<<s;

  m_curAction = WRITE_EXT_FUSE;
  m_progressPos = -1;
  this->start(m_avrdudeExePath, args);
}

bool Avrdude ::saveFlash(QString filePath) {
  if (m_hexFileToSave.isEmpty()) return false;
  if (QFile::exists(filePath)) QFile::remove(filePath);
  return QFile::copy(m_hexFileToSave, filePath);
}

#ifndef AVRDUDE_HPP
#define AVRDUDE_HPP

#include <QProcess>
#include <QRegExp>
#include <QString>

enum AvrdudeActionType {
  NONE,
  READ_FLASH,
  WRITE_FLASH,
  READ_LOW_FUSE,
  READ_HIGH_FUSE,
  READ_EXT_FUSE,
  WRITE_LOW_FUSE,
  WRITE_HIGH_FUSE,
  WRITE_EXT_FUSE,
  CHIP_ERASE
};

class Avrdude : public QProcess {
  Q_OBJECT
  Q_PROPERTY(QString avrdudeExePath READ avrdudeExePath WRITE setAvrdudeExePath
                 NOTIFY avrdudeExePathChanged)
 public:
  explicit Avrdude(QObject *parent = nullptr);
  Q_INVOKABLE QString avrdudeExePath() const;
  Q_INVOKABLE void setAvrdudeExePath(QString exePath);
  Q_INVOKABLE void setHexFile(QString filePath, bool writeFlag = true);
  Q_INVOKABLE void writeFlash(QString mcuId, QString file);
  Q_INVOKABLE void readFlash(QString mcuId);
  Q_INVOKABLE bool saveFlash(QString filePath);
  Q_INVOKABLE bool isValidFuse(QString fuse);
  Q_INVOKABLE void writeLFuse(QString mcuId, QString fuse);
  Q_INVOKABLE void writeHFuse(QString mcuId, QString fuse);
  Q_INVOKABLE void writeEFuse(QString mcuId, QString fuse);
  Q_INVOKABLE void readLFuse(QString);
  Q_INVOKABLE void readHFuse(QString);
  Q_INVOKABLE void readEFuse(QString);
  Q_INVOKABLE void chipErase(QString);
  static QRegExp progExp;
  static QRegExp fuseExp;

 private:
  void processConsoleData(QString data);

 signals:
  void avrdudeExePathChanged();
  void dataFound(QString data);
  void progressReport(int value);
  void errorReport();
  void lfuseRead(QString lfuse);
  void hfuseRead(QString hfuse);
  void efuseRead(QString efuse);
  void endOfProgress();

 public slots:
  void rcvStdout();
  void rcvStderr();
  void rcvExitStatus(int);

 private:
  QString m_hexFileToWrite, m_hexFileToSave;
  QString m_avrdudeExePath;
  bool writeFlag, readFlag, fuseOpFlag;
  bool lfuseReadFlag, hfuseReadFlag, efuseReadFlag;
  int m_progressPos;
  AvrdudeActionType m_curAction;
};

#endif  // AVRDUDE_HPP

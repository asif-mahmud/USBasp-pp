#ifndef SETTINGSPROVIDER_HPP
#define SETTINGSPROVIDER_HPP

#include <QObject>

class QSettings;

class SettingsProvider : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString avrdudePath READ avrdudePath NOTIFY avrdudePathChanged)
  Q_PROPERTY(QString avrdudeConf READ avrdudeConf NOTIFY avrdudeConfChanged)
  Q_PROPERTY(
      bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)
  Q_PROPERTY(QString partId READ partId WRITE setPartId NOTIFY partIdChanged)
  Q_PROPERTY(bool showFullHexPath READ showFullHexPath WRITE setShowFullHexPath
                 NOTIFY showFullHexPathChanged)
  Q_PROPERTY(bool preserveConsoleOutput READ preserveConsoleOutput WRITE
                 setPreserveConsoleOutput NOTIFY preserveConsoleOutputChanged)
  Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY
                 currentPageChanged)
  Q_PROPERTY(QString extraFlags READ extraFlags WRITE setExtraFlags NOTIFY
                 extraFlagsChanged)
 private:
  SettingsProvider(QObject *parent = nullptr);
  inline QString joinPaths(QString prefix, QString path);
  inline void findAvrdudeExe();
  inline void findAvrdudeConf();

 public:
  static SettingsProvider *getInstance();

  Q_INVOKABLE void loadSettings();
  Q_INVOKABLE QString avrdudePath() const;
  Q_INVOKABLE QString avrdudeConf() const;
  Q_INVOKABLE bool darkTheme() const;
  Q_INVOKABLE void setDarkTheme(bool);
  Q_INVOKABLE QString partId() const;
  Q_INVOKABLE void setPartId(QString id);
  Q_INVOKABLE bool showFullHexPath() const;
  Q_INVOKABLE void setShowFullHexPath(bool show);
  Q_INVOKABLE bool preserveConsoleOutput() const;
  Q_INVOKABLE void setPreserveConsoleOutput(bool preserve);
  Q_INVOKABLE int currentPage() const;
  Q_INVOKABLE void setCurrentPage(int page);
  Q_INVOKABLE QString extraFlags() const;
  Q_INVOKABLE void setExtraFlags(QString flags);

 private:
  static SettingsProvider *m_settingsProviderInstance;
  QSettings *m_settings;
  QString m_avrdudePath, m_avrdudeConfPath;

 signals:
  void avrdudePathChanged();
  void avrdudeConfChanged();
  void darkThemeChanged();
  void partIdChanged();
  void showFullHexPathChanged();
  void preserveConsoleOutputChanged();
  void currentPageChanged();
  void extraFlagsChanged();

 public slots:
};

#endif  // SETTINGSPROVIDER_HPP

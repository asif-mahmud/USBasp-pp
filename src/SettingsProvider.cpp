#include "SettingsProvider.hpp"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QSysInfo>

SettingsProvider *SettingsProvider::m_settingsProviderInstance = nullptr;

SettingsProvider::SettingsProvider(QObject *parent) : QObject(parent) {
  m_settings = new QSettings("Embedded Adda", "USBasp++");
}

SettingsProvider *SettingsProvider::getInstance() {
  if (SettingsProvider::m_settingsProviderInstance == nullptr) {
    SettingsProvider::m_settingsProviderInstance = new SettingsProvider();
  }
  return SettingsProvider::m_settingsProviderInstance;
}

QString SettingsProvider::joinPaths(QString prefix, QString path) {
  if (path.startsWith(QDir::separator())) {
    return path;
  }
  return QDir::cleanPath(prefix + QDir::separator() + path);
}

void SettingsProvider::findAvrdudeExe() {
  m_settings->beginGroup("avrdude");
  QVariant avrdudePath = m_settings->value("path", "none");
  if (avrdudePath.toString().compare("none") == 0) {
    // try to find avrdude in the system environmental paths
    // if not found set the path to "none" so that it gets a
    // chance to find avrdude again when loadSettings is called
    QStringList paths;
    QString sysPath = qgetenv("PATH");
    QChar separator = QDir::listSeparator();
    paths.append(sysPath.split(separator));
    paths.append(QDir::currentPath());

    // determine file name for avrdude executable
    QString avrdudeExe = "avrdude";
    if (QSysInfo::kernelType().compare("winnt") == 0) {
      avrdudeExe += ".exe";
    }

    // look into all paths for avrdude executable
    // including the current directory.
    qDebug() << "Searching for avrdude executable";
    bool found = false;
    for (QString path : paths) {
      QString avrdudePath = joinPaths(path, avrdudeExe);
      bool exists = QFileInfo::exists(avrdudePath);
      qDebug() << joinPaths(path, avrdudeExe) << ": " << exists;
      if (exists) {
        m_settings->setValue("path", avrdudePath);
        m_avrdudePath = avrdudePath;
        found = true;
        break;
      }
    }

    // check if not found
    if (!found) {
      m_avrdudePath = "none";
    }
  } else {  // set the private path variable to found path for runtime usage
    m_avrdudePath = m_settings->value("path").toString();
    // verify existence
    if (!QFileInfo::exists(m_avrdudePath)) {
      m_avrdudePath = "none";
      m_settings->setValue("path", "none");
    }
  }
  m_settings->endGroup();
}

QString SettingsProvider::avrdudePath() const { return m_avrdudePath; }

void SettingsProvider::findAvrdudeConf() {
  m_settings->beginGroup("avrdude");
  QVariant confPath = m_settings->value("conf", "none");
  if (confPath.toString().compare("none") == 0) {
    qDebug() << "Searching for avrdude configuration";
    QStringList paths;
    QString envPath = qgetenv("PATH");
    QStringList envPaths = envPath.split(QDir::listSeparator());
    if (QSysInfo::kernelType().compare("winnt") == 0) {
      // for windows search in the path folders, cause the
      // config file is supposed to be in the same folder as the exe
      // file.
      for (QString path : envPaths) {
        paths.append(joinPaths(path, "avrdude.conf"));
      }
      // finally add the current directory to the search paths too
      paths.append(joinPaths(QDir::currentPath(), "avrdude.conf"));
    } else {
      // search in the possible "etc" folder of the parent
      // directory of the path folders.
      // add the root path plus etc
      paths.append(
          joinPaths(QDir::rootPath(), joinPaths("etc", "avrdude.conf")));
      for (QString path : envPaths) {
        QDir parent = QFileInfo(path).dir();
        paths.append(
            joinPaths(parent.path(), joinPaths("etc", "avrdude.conf")));
      }
      // add the current directory as a last resort
      paths.append(joinPaths(QDir::currentPath(), "avrdude.conf"));
    }

    // now iterate the list of paths to see if theres a config file
    // in the file system
    bool found = false;
    for (QString path : paths) {
      bool exists = QFileInfo::exists(path);
      qDebug() << path << ":" << exists;
      if (exists) {
        m_settings->setValue("conf", path);
        m_avrdudeConfPath = path;
        found = true;
        break;
      }
    }

    // check for not found condition
    if (!found) {
      m_avrdudeConfPath = "none";
    }
  } else {
    m_avrdudeConfPath = m_settings->value("conf").toString();
    // verify the existence
    if (!QFileInfo::exists(m_avrdudeConfPath)) {
      m_avrdudeConfPath = "none";
      m_settings->setValue("conf", "none");
    }
  }
  m_settings->endGroup();
}

QString SettingsProvider::avrdudeConf() const { return m_avrdudeConfPath; }

bool SettingsProvider::darkTheme() const {
  return m_settings->value("User/darkTheme", true).toBool();
}

void SettingsProvider::setDarkTheme(bool dark) {
  m_settings->setValue("User/darkTheme", dark);
  emit darkThemeChanged();
}

QString SettingsProvider::partId() const {
  return m_settings->value("User/partId", "none").toString();
}

void SettingsProvider::setPartId(QString id) {
  QString curPartId = m_settings->value("User/partId", "none").toString();
  if (curPartId.compare(id) != 0) {
    m_settings->setValue("User/partId", id);
    emit partIdChanged();
  }
}

bool SettingsProvider::showFullHexPath() const {
  return m_settings->value("User/showFullHexPath", false).toBool();
}

void SettingsProvider::setShowFullHexPath(bool show) {
  m_settings->setValue("User/showFullHexPath", show);
  emit showFullHexPathChanged();
}

bool SettingsProvider::preserveConsoleOutput() const {
  return m_settings->value("User/preserveConsoleOutput", false).toBool();
}

void SettingsProvider::setPreserveConsoleOutput(bool preserve) {
  m_settings->setValue("User/preserveConsoleOutput", preserve);
  emit preserveConsoleOutputChanged();
}

int SettingsProvider::currentPage() const {
  return m_settings->value("User/currentPage", 0).toInt();
}

void SettingsProvider::setCurrentPage(int page) {
  m_settings->setValue("User/currentPage", page);
  emit currentPageChanged();
}

QString SettingsProvider::extraFlags() const {
  return m_settings->value("User/extraFlags", "").toString();
}

void SettingsProvider::setExtraFlags(QString flags) {
  m_settings->setValue("User/extraFlags", flags);
  emit extraFlagsChanged();
}

void SettingsProvider::loadSettings() {
  qDebug() << "Loading settings from" << m_settings->fileName();

  findAvrdudeExe();
  emit avrdudePathChanged();
  qDebug() << "AVRDUDE PATH:" << m_avrdudePath;

  findAvrdudeConf();
  emit avrdudeConfChanged();
  qDebug() << "AVRDUDE CONF:" << m_avrdudeConfPath;
}

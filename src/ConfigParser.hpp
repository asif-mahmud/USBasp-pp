#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <QList>
#include <QObject>
#include "Part.hpp"

class QFile;
class ConfigBlock;

/**
 * @brief The ConfigParser class
 *
 * Avrdude configuration script parser.
 */
class ConfigParser : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString configPath READ configPath WRITE setConfigPath NOTIFY
                 configPathChanged)
  Q_PROPERTY(QList<QObject *> partList READ partList NOTIFY partListChanged)
 public:
  ConfigParser(QObject *parent = nullptr);

  Q_INVOKABLE QString configPath() const;
  Q_INVOKABLE void setConfigPath(QString path);
  Q_INVOKABLE void loadPartList();
  Q_INVOKABLE QList<QObject *> partList() const;

 private:
  ConfigBlock nextBlock(QFile &file, QString blockName);
  inline QStringList parseTokens(QString &line);

 private:
  QString m_configPath;
  QList<QObject *> m_partList;

 signals:
  void configPathChanged();
  void partListChanged();

 public slots:
};

#endif  // CONFIGPARSER_HPP

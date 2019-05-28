#ifndef CONFIGBLOCK_HPP
#define CONFIGBLOCK_HPP

#include <QList>
#include <QObject>

class ConfigBlock : public QObject {
  Q_OBJECT
 public:
  ConfigBlock(QObject *parent = nullptr);
  ConfigBlock(QString name, QObject *parent = nullptr);
  ConfigBlock(const ConfigBlock &block);

  QString name;
  QList<QString> blockConfig;
  QList<QStringList> statements;
  QList<ConfigBlock> children;

  ConfigBlock &operator=(const ConfigBlock &);

 signals:

 public slots:
};

#endif  // CONFIGBLOCK_HPP

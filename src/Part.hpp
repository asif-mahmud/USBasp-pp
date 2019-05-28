#ifndef PART_HPP
#define PART_HPP

#include <QObject>

class Part : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString id READ id NOTIFY idChanged)
  Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
  Q_PROPERTY(QString signature READ signature NOTIFY signatureChanged)
  Q_PROPERTY(QString flashSize READ flashSize NOTIFY flashSizeChanged)
  Q_PROPERTY(QString eepromSize READ eepromSize NOTIFY eepromSizeChanged)
 public:
  Part(QObject *parent = nullptr);
  Part(QString id, QString description, QString signature, QString flashSize,
       QString eepromSize, QObject *parent = nullptr);
  Part(const Part &part);
  Q_INVOKABLE QString id() const;
  Q_INVOKABLE QString description() const;
  Q_INVOKABLE QString signature() const;
  Q_INVOKABLE QString flashSize() const;
  Q_INVOKABLE QString eepromSize() const;
  void setId(QString id);
  void setDescription(QString description);
  void setSignature(QString signature);
  void setFlashSize(QString flashSize);
  void setEepromSize(QString eepromSize);

  Q_INVOKABLE Part &operator=(const Part &other);

 signals:
  void idChanged();
  void descriptionChanged();
  void signatureChanged();
  void flashSizeChanged();
  void eepromSizeChanged();

 private:
  QString m_id, m_description, m_signature, m_flashSize, m_eepromSize;

 public slots:
};

#endif  // PART_HPP

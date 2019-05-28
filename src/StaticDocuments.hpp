#ifndef STATICDOCUMENTS_HPP
#define STATICDOCUMENTS_HPP

#include <QObject>

class StaticDocuments : public QObject {
  Q_OBJECT
  Q_PROPERTY(QString licenseText READ licenseText NOTIFY licenseTextReady)
  Q_PROPERTY(QString aboutText READ aboutText NOTIFY aboutTextReady)

 private:
  StaticDocuments(QObject *parent = nullptr);

 public:
  QString licenseText();
  QString aboutText();
  static StaticDocuments *getInstance();

 signals:
  void licenseTextReady();
  void aboutTextReady();

 public slots:

 private:
  static QString m_licenseText;
  static QString m_aboutText;
  static StaticDocuments *m_staticDocumentsInstance;
};

#endif  // STATICDOCUMENTS_HPP

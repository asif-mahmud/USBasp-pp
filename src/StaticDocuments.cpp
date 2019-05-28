#include "StaticDocuments.hpp"
#include <QDebug>
#include <QFile>
#include <QTextStream>

QString StaticDocuments::m_licenseText = "";
QString StaticDocuments::m_aboutText = "";
StaticDocuments* StaticDocuments::m_staticDocumentsInstance = nullptr;

StaticDocuments::StaticDocuments(QObject* parent) : QObject(parent) {}

QString StaticDocuments::licenseText() {
  if (StaticDocuments::m_licenseText.isEmpty()) {
    QFile file(":/docs/gpl-2.0.txt");
    QString data = "";
    if (file.open(QIODevice::ReadOnly)) {
      QTextStream in(&file);
      data = in.readAll();
      file.close();
    }
    StaticDocuments::m_licenseText = data;
  }
  return StaticDocuments::m_licenseText;
}

QString StaticDocuments::aboutText() {
  if (StaticDocuments::m_aboutText.isEmpty()) {
    QFile file(":/docs/about.txt");
    QString data = "";
    if (file.open(QIODevice::ReadOnly)) {
      QTextStream in(&file);
      data = in.readAll();
      file.close();
    }
    StaticDocuments::m_aboutText = data;
  }
  return StaticDocuments::m_aboutText;
}

StaticDocuments* StaticDocuments::getInstance() {
  if (StaticDocuments::m_staticDocumentsInstance == nullptr) {
    StaticDocuments::m_staticDocumentsInstance = new StaticDocuments();
  }
  return StaticDocuments::m_staticDocumentsInstance;
}

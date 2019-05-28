#include "Part.hpp"

Part::Part(QObject *parent) : QObject(parent) {
  m_id = "none";
  m_description = "none";
  m_signature = "none";
  m_flashSize = "0";
  m_eepromSize = "0";
}

Part::Part(QString id, QString description, QString signature,
           QString flashSize, QString eepromSize, QObject *parent)
    : QObject(parent),
      m_id(id),
      m_description(description),
      m_signature(signature),
      m_flashSize(flashSize),
      m_eepromSize(eepromSize) {}

Part::Part(const Part &part)
    : QObject(part.parent()),
      m_id(part.id()),
      m_description(part.description()),
      m_signature(part.signature()),
      m_flashSize(part.flashSize()),
      m_eepromSize(part.eepromSize()) {}

Part &Part::operator=(const Part &other) {
  m_id = other.m_id;
  m_description = other.m_description;
  m_signature = other.m_signature;
  m_flashSize = other.m_flashSize;
  m_eepromSize = other.m_eepromSize;
  return *this;
}

QString Part::id() const { return m_id; }

void Part::setId(QString id) {
  if (m_id.compare(id) != 0) {
    m_id = id;
    emit idChanged();
  }
}

QString Part::description() const { return m_description; }

void Part::setDescription(QString description) {
  if (m_description.compare(description) != 0) {
    m_description = description;
    emit descriptionChanged();
  }
}

QString Part::signature() const { return m_signature; }

void Part::setSignature(QString signature) {
  if (m_signature.compare(signature) != 0) {
    m_signature = signature;
    emit signatureChanged();
  }
}

QString Part::flashSize() const { return m_flashSize; }

void Part::setFlashSize(QString flashSize) {
  if (m_flashSize.compare(flashSize) != 0) {
    m_flashSize = flashSize;
    emit flashSizeChanged();
  }
}

QString Part::eepromSize() const { return m_eepromSize; }

void Part::setEepromSize(QString eepromSize) {
  if (m_eepromSize.compare(eepromSize) != 0) {
    m_eepromSize = eepromSize;
    emit eepromSizeChanged();
  }
}

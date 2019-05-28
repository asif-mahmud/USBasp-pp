#include "ConfigBlock.hpp"

ConfigBlock::ConfigBlock(QObject *parent) : QObject(parent) {}

ConfigBlock::ConfigBlock(QString c_name, QObject *parent)
    : QObject(parent), name(c_name) {}

ConfigBlock::ConfigBlock(const ConfigBlock &block)
    : QObject(block.parent()),
      name(block.name),
      blockConfig(block.blockConfig),
      statements(block.statements),
      children(block.children) {}

ConfigBlock &ConfigBlock::operator=(const ConfigBlock &other) {
  name = other.name;
  blockConfig.append(other.blockConfig);
  statements.append(other.statements);
  children.append(other.children);
  return *this;
}

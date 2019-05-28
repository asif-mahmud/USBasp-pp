#include "ConfigParser.hpp"
#include <QDebug>
#include <QFile>
#include <QList>
#include "ConfigBlock.hpp"

ConfigParser::ConfigParser(QObject *parent) : QObject(parent) {}

QString ConfigParser::configPath() const { return m_configPath; }

void ConfigParser::setConfigPath(QString path) {
  if (m_configPath.compare(path) != 0) {
    m_configPath = path;
    emit configPathChanged();
  }
}

QList<QObject *> ConfigParser::partList() const { return m_partList; }

QStringList ConfigParser::parseTokens(QString &line) {
  // parse the tokens in the current line,
  // tokens are not separated by space char,
  // instead they are separated by space char
  // or comma depending on the presence of quote
  // char.
  QStringList tokens;
  QString::size_type prevPos = 0, curPos = 0;
  while (curPos < line.size()) {
    // start of a token, can be alphanumeric or quote
    bool quoteStart = false;
    bool commentLine = false;
    for (; prevPos < line.size(); prevPos++) {
      if (line[prevPos] == '"') {
        prevPos++;  // skip the quote char
        quoteStart = true;
        break;
      } else if (line[prevPos] == '#') {
        // a line comment so skip the entire line
        commentLine = true;
        break;
      } else if (line[prevPos] == '/' && line[prevPos + 1] == '*') {
        // a block comment, so lets skip the block
        prevPos++;
        do {
          prevPos++;
        } while (!(line[prevPos] == '*' && line[prevPos + 1] == '/'));
        prevPos++;
      } else if (!line[prevPos].isSpace()) {
        break;
      }
    }
    if (commentLine) {
      break;
    }
    // find last position based on quote char finding
    // if quote char was found at beginning of token, it must
    // end with another quote char. in addition look out for
    // comma and statement ending.
    bool statementEndFound = false;
    for (curPos = prevPos + 1; curPos < line.size(); curPos++) {
      if (quoteStart) {
        if (line[curPos] == '"') {
          break;
        } else if (line[curPos] == ';') {
          statementEndFound = true;
          break;
        }
      } else if (!quoteStart) {
        if (line[curPos].isSpace() || line[curPos] == ',' ||
            line[curPos] == '#') {
          break;
        } else if (line[curPos] == ';') {
          statementEndFound = true;
          break;
        }
      }
    }
    if (curPos > prevPos) {
      tokens.append(line.mid(prevPos, curPos - prevPos));
      if (statementEndFound) {
        tokens.append(";");
        break;  // this is supposed to be the end of this line, so stop parsing
                // anymore
      }
      if (quoteStart && line[curPos] == ',') {
        curPos++;  // skip ending comma char after ending quote char
      }
    }

    // increment cursor
    curPos++;
    prevPos = curPos;
  }
  return tokens;
}

ConfigBlock ConfigParser::nextBlock(QFile &file, QString blockName) {
  ConfigBlock block(blockName);
  bool blockStart = false;
  bool blockEnd = false;
  qint64 prevFileCursor;
  do {
    if (file.atEnd()) {
      break;
    }
    // keep track of previous file cursor position, in case
    // we encounter a child block, we'll have to rewind a little bit
    prevFileCursor = file.pos();
    QString line = file.readLine();
    line = line.trimmed();
    // skip empty or comment lines
    if (line.isEmpty() || line.startsWith('#')) {
      continue;
    }
    // this is a new statement apparently
    // get the tokens of this line
    QStringList tokens = parseTokens(line);
    if (tokens.empty()) {
      continue;
    }
    //    qDebug() << tokens;
    if (!blockStart && tokens.at(0).compare(blockName) == 0) {
      blockStart = true;
      if (tokens.size() > 1) {
        for (QList<QString>::size_type pos = 1; pos < tokens.size(); pos++) {
          block.blockConfig.append(tokens.at(pos));
        }
      }
    } else if (blockStart && tokens.at(0).compare("memory") == 0) {
      // memory is a known child block of part. so rewind the file
      // and read the memory block as another block and append it to children.
      // this is a special case. every other child block would have been
      // parsed in the same manner.
      file.seek(prevFileCursor);
      ConfigBlock childBlock = nextBlock(file, "memory");
      block.children.append(childBlock);
    } else if (blockStart && !blockEnd && tokens.at(0).compare(";") == 0) {
      blockEnd = true;
    } else if (blockStart) {
      // this is a statement
      QList<QList<QString>>::size_type last = block.statements.size() - 1;
      if (last < 0) {
        // this is the very first statement of this block
        block.statements.append(tokens);
      } else {
        // this might be a part of a previous line/statement, so check if
        // last statement in the block has any ending semi-colon
        if (block.statements.last().last().compare(";") == 0) {
          // so this is a new statement, cause last one is ended with a
          // semi-colon
          block.statements.append(tokens);
        } else {
          // this is part of the previous statement so append to the last
          // line/statement
          block.statements.last().append(tokens);
        }
      }
    }
  } while (!(blockStart && blockEnd));  // only break the loop when
                                        // both block start and end
                                        // are found

  return block;
}

void ConfigParser::loadPartList() {
  QFile confFile(m_configPath);
  // open in text reading mode, so that newline chars
  // will be properly converted into one LN char in all
  // platforms.
  if (!confFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Could not open" << m_configPath << "to read";
    return;
  }

  // try to separate out the abstract parts
  QList<Part *> abstractParts;
  m_partList.clear();
  do {
    ConfigBlock block = nextBlock(confFile, "part");
    if (!confFile.atEnd()) {
      // create a new part
      Part part;
      // check if the part block is an inherited one
      if (!block.blockConfig.empty()) {
        // syntax part parent "id"
        QString parentId = block.blockConfig.at(1);
        // first check in the abstract part list
        bool found = false;
        for (Part *parent : abstractParts) {
          if (parent->id().compare(parentId) == 0) {
            part = *parent;
            found = true;
            break;
          }
        }
        // if parent isn't in the abstract parts then must be
        // in the part list
        if (!found) {
          for (QObject *parent : m_partList) {
            Part *parentPart = reinterpret_cast<Part *>(parent);
            if (parentPart->id().compare(parentId) == 0) {
              part = *parentPart;
              break;
            }
          }
        }
      }
      // now iterate through the statements and children to fill up the
      // part info
      bool isAbstractPart = false;
      for (QList<QString> statement : block.statements) {
        QString lhs = statement.at(0);
        if (lhs.compare("id") == 0) {
          part.setId(statement.at(2));
          // if the id starts with a . its an abstract part
          if (part.id().startsWith(".")) {
            isAbstractPart = true;
          }
        } else if (lhs.compare("desc") == 0) {
          part.setDescription(statement.at(2));
          // if the description is multiword its an abstract part
          if (part.description().count(' ') > 0) {
            isAbstractPart = true;
          }
        } else if (lhs.compare("signature") == 0) {
          part.setSignature(statement.at(2) + " " + statement.at(3) + " " +
                            statement.at(4));
        }
      }

      // walk through children as well to fill up additional info
      for (ConfigBlock child : block.children) {
        if (child.blockConfig.empty()) {
          continue;
        }
        if (child.blockConfig.at(0).compare("flash") == 0) {
          for (QList<QString> statement : child.statements) {
            if (statement.at(0).compare("size") == 0) {
              part.setFlashSize(statement.at(2));
              break;
            }
          }
        } else if (child.blockConfig.at(0).compare("eeprom") == 0) {
          for (QList<QString> statement : child.statements) {
            if (statement.at(0).compare("size") == 0) {
              part.setEepromSize(statement.at(2));
              break;
            }
          }
        }
      }
      // append the newly found part to the list
      // also need a pointer for the list so create a new
      // instance with new syntax
      if (isAbstractPart) {
        abstractParts.append(new Part(part));
      } else {
        m_partList.append(new Part(part));
      }
    }
  } while (!confFile.atEnd());
  //  qDebug() << partBlocks.size();
  //  qDebug() << partBlocks.last().statements;

  // must close config file
  if (confFile.isOpen()) {
    confFile.close();
  }
}

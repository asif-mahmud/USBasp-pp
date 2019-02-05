#include <Mcu.hpp>
#include <QDebug>
#include <QFile>
#include <QRegExp>
#include <QApplication>
#include <exception>
#include <stdexcept>

using namespace std;

Mcu::Mcu() {}

Mcu ::~Mcu(){}

Mcu & Mcu :: operator =(const Mcu & mcu)
{
    this->id = mcu.id;
    this->desc = mcu.desc;
    this->signature = mcu.signature;
    this->flash_size = mcu.flash_size;
    this->eeprom_size = mcu.eeprom_size;

    return *this;
}

McuList::McuList()
    :totalMcusFound(0)
{}

McuList::~McuList(){}

void McuList :: parse_config()
{
    QString conf;
    QRegExp part_exp("^part$"),
            child_part_exp("^part\\s*parent\\s*\"(.*)\"$"),
            id_exp("^\\s*id\\s*=\\s*\"(.*)\";$"),
            desc_exp("^\\s*desc\\s*=\\s*\"(.*)\";$"),
            signature_exp("^\\s*signature\\s*=\\s*0x(.{2,2})\\s*0x(.{2,2})\\s*0x(.{2,2})\\s*;$"),
            memory_exp("\\s*memory\\s\"(.*)\""),
            size_exp("^\\s*size\\s*=\\s*(.*)\\s*;$"),
            end_exp("^\\s*;$");

    QString line;

#define read() line=conf_file.readLine();\
                line=line.trimmed()

#if defined(__unix__)
    conf = "/etc/avrdude.conf";
#elif defined(_WIN32)
    qDebug()<<"App path :"<<qApp->applicationDirPath();
    conf = qApp->applicationDirPath()+"\\avrdude.conf";
#endif

    if(conf.isEmpty())
    {
        qDebug()<< "Could not determine OS type";
        throw  runtime_error("Could not determine OS type");
    }

    QFile conf_file(conf);
    if(!conf_file.exists())
    {
        qDebug()<< "Could not find config <"<<conf<<"> file";
        throw runtime_error("Could not find " + conf.toStdString());
    }

    if(!conf_file.open(QFile::ReadOnly))
    {
        qDebug()<< "Could not open config <"<<conf<<"> file";
        throw runtime_error("Could not open " + conf.toStdString() + " to read.");
    }

    while(!conf_file.atEnd())
    {
        bool mcuFound=false,childMcu=false;
        read();
        if(part_exp.indexIn(line) != -1)
        {
            mcuFound=true;
        }//part expression selection
        else if (child_part_exp.indexIn(line) != -1)
        {
            //a part with parent id found
            mcuFound=true;
            childMcu=true;
        }

        if(mcuFound)
        {
            qDebug()<<"Found an mcu";
            Mcu mcu;
            if(childMcu)
            {
                mcu = mcus_by_id[child_part_exp.cap(1)];
            }
            int lines=0;
            bool repeatingLine = false;
            //an mcu config starts now
            //read untill the end of it
            //consider the inside loop for memory configs
            do{
                if(!repeatingLine)//line is already read from memory block
                {
                    read();
                    lines++;
                }else
                    repeatingLine = false;//reset
                if(id_exp.indexIn(line) != -1)
                {
                    mcu.id = id_exp.cap(1);
                    qDebug()<<"ID "<<mcu.id;
                }
                else if(desc_exp.indexIn(line) != -1)
                {
                    mcu.desc = desc_exp.cap(1);
                    qDebug()<<"Desc "<<mcu.desc;
                }
                else if(signature_exp.indexIn(line) != -1)
                {
                    mcu.signature = "0x"+signature_exp.cap(1)+signature_exp.cap(2)+signature_exp.cap(3);
                    qDebug()<<"Signature "<<mcu.signature;
                }
                else if(memory_exp.indexIn(line) != -1)
                {
                    //it can be any type of memory
                    //we only need flash and eeprom
                    qDebug()<<"inside"<<memory_exp.cap(1)<<" block";
                    bool flash_memory = false, eeprom_memory = false;
                    if(memory_exp.cap(1) == "flash")
                    {
                        flash_memory = true;
                    }else if(memory_exp.cap(1) == "eeprom")
                    {
                        eeprom_memory = true;
                    }

                    do{
                        read();
                        lines++;
                        if(size_exp.indexIn(line) != -1)
                        {
                            if(flash_memory)
                            {
                                mcu.flash_size = size_exp.cap(1);
                                qDebug()<<"Flash "<<mcu.flash_size<<"bytes";
                            }else if(eeprom_memory)
                            {
                                mcu.eeprom_size = size_exp.cap(1);
                                qDebug()<<"EEPROM "<<mcu.eeprom_size<<"bytes";
                            }
                        }
                    }while(end_exp.indexIn(line) == -1);//end of memory block

                    //should read one more line for empty string
                    read();
                    if(line.size()>0)
                        repeatingLine = true;
                }
            }while(end_exp.indexIn(line) == -1);//end of part block

            qDebug()<<"Total lines in "<<mcu.id<<lines;

            totalMcusFound++;
            mcus_by_label.insert(mcu.desc,mcu);
            mcus_by_id.insert(mcu.id,mcu);
            //dont append any label without a valid signature
            if(mcu.signature.size() > 0)
                mcu_labels.append(mcu.desc);
        }
    }//end of read loop

    qDebug()<< "Total"<<totalMcusFound<<" Mcus found";
}

int McuList::default_label()
{
//    return mcu_labels.indexOf(QRegExp("^ATmega8$"));
    if(mcu_labels.contains("ATmega8")) {
        return mcu_labels.indexOf("ATmega8");
    }else if (mcu_labels.contains("ATMEGA8")) {
        return mcu_labels.indexOf("ATMEGA8");
    }

    return 0;
}

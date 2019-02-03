#ifndef MCU_HPP
#define MCU_HPP

#include <QString>
#include <QStringList>
#include <QMap>

class Mcu
{
    public:
        Mcu();
        ~Mcu();

        QString id,
                desc,
                signature,
                flash_size,
                eeprom_size;

        Mcu & operator = (const Mcu &mcu);
};

class McuList {
    public:
        McuList();
        ~McuList();

        void parse_config();
        int default_label();
        int totalMcusFound;
        QMap<QString,Mcu> mcus_by_label;
        QMap<QString,Mcu> mcus_by_id;
        QStringList mcu_labels;
};

#endif // MCU_HPP

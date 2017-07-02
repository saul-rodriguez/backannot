#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>

//valid device types
#define TWO_TERMINALS 0 //Used for vsource, isource, res, cap, ind
#define FOUR_TERMINALS_FET 1 //Used for

class Device : public QObject
{
    Q_OBJECT
public:

    QString m_value;
    int m_x;
    int m_y;
    int m_rotation;
    int m_flip;
    int m_type; //hold the device type

    QString m_schem_filename; //holds the schematics filename
    QString m_parent;
    QString m_spicename;

    void printvalues();

public:
    explicit Device(QObject *parent = 0);

signals:

public slots:
};

#endif // DEVICE_H

#ifndef XSCHEM_H
#define XSCHEM_H

#include <QObject>
#include "device.h"
#include "spicedev.h"
#include "ngspiceop.h"

class XSchem : public QObject
{
    Q_OBJECT
private:
    Device m_devices[100];
    int m_device_index;

    SPICEdev m_spicedev[100];
    int m_spicedev_index;

    QStringList m_spicefile;
    int m_spicefile_size;

    ngspiceOP m_ngspiceop;



public:
    QString m_pathXSchemLib;

    QString m_schemList[100];
    int m_schemList_num;

    //QStringList fileLines;
    //int fileSize; //number of lines

public:
    explicit XSchem(QObject *parent = 0);

    void setPathXSchemLib(QString path);

    void readXSchem(QString filename, QString parent);
    void identifyComponent(QStringList fields, QString filename, QString parent);
    void printdevices();

    void readSpice(QString filename);
    void identifyNets(QString parent, QStringList parent_nets, QString subcirc);
    void saveSpiceDev(QString name, QStringList nets);
    void printSpiceDev();

    void readNodeVoltages(QString filename);
    void printNgSpiceVoltages();

signals:

public slots:
};

#endif // XSCHEM_H

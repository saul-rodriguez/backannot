#include "device.h"
#include <qdebug.h>

void Device::printvalues()
{
    qDebug()<<"**** Device ****";
    qDebug()<<"Parent: "<<m_parent;
    qDebug()<<"value: "<<m_value;
    qDebug()<<"SPice name:"<<m_spicename;
    qDebug()<<"x = "<<m_x<<" y = "<<m_y;
    qDebug()<<"rot = "<<m_rotation<<" flip = "<<m_flip;
    qDebug()<<"Schematic: "<<m_schem_filename;

}

Device::Device(QObject *parent) : QObject(parent)
{

}

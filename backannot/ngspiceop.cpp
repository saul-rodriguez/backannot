#include "ngspiceop.h"
#include <QDebug>


ngspiceOP::ngspiceOP(QObject *parent) : QObject(parent)
{

}

void ngspiceOP::convertEng()
{
    int size = m_nets.size();
    double aux;
    int prec;

    QString value;
    QString unit;
    for (int i = 0; i < size; i++) {
        value = m_values[i];
        aux = value.toDouble();

        //aux = 0.0000123456;

        if (fabs(aux) < 1e-6) {
            aux *= 1e9; //nV
            unit = "nV";
        } else if (fabs(aux) < 1e-3) {
            aux *= 1e6; //uV
            unit = "uV";
        } else if (fabs(aux) < 1) {
            aux *= 1e3; //mV
            unit = "mV";
        } else {
            unit = "V";
        }

        if (aux > 100) {
            prec = 1;
        } else if (aux > 10) {
            prec = 2;
        } else {
            prec = 3;
        }

        QString finalvalue = QString("%1").arg(aux,5,'f',prec);
        finalvalue += unit;

        m_values_eng << finalvalue;


    }
}




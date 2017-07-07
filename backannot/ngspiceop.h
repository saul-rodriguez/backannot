#ifndef NGSPICEOP_H
#define NGSPICEOP_H

#include <QObject>

class ngspiceOP : public QObject
{
    Q_OBJECT


public:
    explicit ngspiceOP(QObject *parent = 0);

public:

    QStringList m_nets;
    QStringList m_values;
    QStringList m_values_eng;

    void convertEng();
   // void printNgSpiceNetVoltages();

signals:

public slots:
};

#endif // NGSPICEOP_H

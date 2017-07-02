#ifndef SPICEDEV_H
#define SPICEDEV_H

#include <QObject>

class SPICEdev : public QObject
{
    Q_OBJECT

public:
    QString m_name;
    QStringList m_nets;
    QString m_parent;

public:
    explicit SPICEdev(QObject *parent = 0);

signals:

public slots:
};

#endif // SPICEDEV_H

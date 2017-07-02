#include <QCoreApplication>
#include "xschem.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    XSchem m_xschem;

    QString xschemlib = "/home/saul/projects/xschem_library/";
    m_xschem.setPathXSchemLib(xschemlib);

    m_xschem.readXSchem("/home/saul/projects/xschem_library/mylib/voltage_div_tb.sch","");
    m_xschem.printdevices();

    m_xschem.readSpice("/home/saul/projects/xschem_library/simulations/voltage_div_tb.spice");
    m_xschem.printSpiceDev();

    return a.exec();
}

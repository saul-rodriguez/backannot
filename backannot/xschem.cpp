#include "xschem.h"
#include <QFile>
#include <QTextStream>
#include <qdebug.h>

XSchem::XSchem(QObject *parent) : QObject(parent)
{
    m_device_index = 0;
    m_spicedev_index = 0;
    m_schemList_num =0;
    m_spicefile_size = 0;
}

void XSchem::setPathXSchemLib(QString path)
{
    m_pathXSchemLib = path;
}

void XSchem::readXSchem(QString filename, QString parent)
{

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly)) {
                qDebug("error opening file");
    }

    QTextStream in(&file);

    QString db = in.readAll();

    QStringList fileLines;
    int numLines; //number of lines

    fileLines = db.split(QRegExp("\n"));
    numLines = fileLines.length();

    //Find primitive components
    QStringList fields;
    QChar *data;

    for (int i = 0; i < numLines; i++) { //Each line is checked for primitive components

        fields = fileLines[i].split(QRegExp("\\s+")); //Split line in strings
        data = fields[0].data(); //First string defines the type

        if(data[0] == 'C') { //primitive component was found in this schematic
            identifyComponent(fields, filename, parent); //
        }


    }

    file.close();

}

void XSchem::identifyComponent(QStringList fields, QString filename, QString parent)
{
    int ret1, ret2, ret3, ret4, ret5;
    QString aux;

    ret1 = QString::compare(fields[1],"{devices/vsource}");
    ret2 = QString::compare(fields[1],"{devices/isource}");
    ret3 = QString::compare(fields[1],"{devices/res}");
    ret4 = QString::compare(fields[1],"{devices/capa}");
    ret5 = QString::compare(fields[1],"{devices/ind}");

    if (!ret1 || !ret2 || !ret3 || !ret4 || !ret5) { //two terminal primitive device found
        m_devices[m_device_index].m_type = TWO_TERMINALS;
        aux = fields[2];
        m_devices[m_device_index].m_x = aux.toInt();
        aux = fields[3];
        m_devices[m_device_index].m_y = aux.toInt();
        aux = fields[4];
        m_devices[m_device_index].m_rotation = aux.toInt();
        aux = fields[5];
        m_devices[m_device_index].m_flip = aux.toInt();

        QString aux_value = fields[6];
        QStringList value_split =  aux_value.split("=");
        QString aux_value2 = value_split[1];
        aux_value2.replace(QString("}"),QString(""));
        m_devices[m_device_index].m_value = aux_value2;

        m_devices[m_device_index].m_parent = parent;

        int comp = QString::compare(parent,"");

        if (!comp) { //root schematic
            //newParent = aux_value;
            m_devices[m_device_index].m_spicename = aux_value2;
        } else {
            m_devices[m_device_index].m_spicename = parent + "." + aux_value2;
        }

        //[m_device_index].m_spicename = parent + "." + aux_value2;

        m_devices[m_device_index].m_schem_filename = filename;

        m_device_index++;
        return;
    }

    //other components from the device library are not supported. If the call
    //comes from a device, this function returns
    QString aux_value = fields[1];
    QStringList aux_list = aux_value.split('/');
    aux_value = aux_list[0];
    ret1 = QString::compare(aux_value,"{devices");
    if(!ret1) return;

    //The component is inside a sub-block, needs to make a recursive call to schematic inside
    //Identify the subblock schematic file name
    QString aux_value1 = fields[1];
    aux_value1.replace(QString("{"), QString(""));
    aux_value1.replace(QString("}"), QString(""));

    QString subblockFilename = m_pathXSchemLib + aux_value1 + ".sch";

    //Identify the parent subblock instance
    aux_value1 = fields[6];
    aux_list = aux_value1.split('=');
    aux_value1 = aux_list[1];
    aux_value1.replace(QString("}"),QString(""));

    ret1 = QString::compare(parent,"");

    QString newParent;
    if (!ret1) { //root schematic
        newParent = aux_value1;
    } else {
        newParent = parent + "." + aux_value1;
    }

    //Recursive call
    readXSchem(subblockFilename,newParent);

}

void XSchem::printdevices()
{
    for (int i=0; i<m_device_index; i++) {
        m_devices[i].printvalues();
    }

}

void XSchem::readSpice(QString filename)
{

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly)) {
                qDebug("error opening file");
    }

    QTextStream in(&file);

    QString db = in.readAll();

    //QStringList fileLines;
    //int numLines; //number of lines

    m_spicefile = db.split(QRegExp("\n"));
    m_spicefile_size = m_spicefile.length();

    //Parse recursively the spice and get all the nets for all devices.
    QStringList aux;
    identifyNets("",aux,"");

    file.close();

}

void XSchem::identifyNets(QString parent, QStringList parent_nets, QString subcirc)
{
  //The idea here is to use this function recursively to enter all hierarchies
  //parent is an xa.xb.xc identifier with a number, parent_nets[i] are the nets passed to the subc
  //and subcircuit is the parent subckt name. This functions parses the spice code
  //and searches for primitives and subckts.

  int index = 0;
  int el1, el2, el3;
  QStringList child_nets;
  int child_nets_size = 0;
  QStringList fields;
  QChar *data;
  int isroot;

  //Check if the call comes from the root or if it has to search a subckt
  isroot = QString::compare(parent,"");

  if (!isroot) { //called to the root schematic
      index = 0; //Iterates from the beginning of the file

  } else { //called to a subckt, searches the position of the subckt

      while(1) {
          fields = m_spicefile[index].split(QRegExp("\\s+")); //Split line in strings
          el1 = QString::compare(fields[0],".subckt");

          if (!el1) { //Subckt header found
              el2 = QString::compare(fields[1],subcirc);

              if (!el2) { //Subckt name found
                  //Save child net names
                  int num = fields.size();
                  for (int i=2; i<num; i++) {
                      child_nets << fields[i];
                  }
                  child_nets_size = child_nets.size();
                  break;
              }

          }
          index++;
      }

  }

  //The loop iterates looking for primitives and subcircuits until it finds .end or .ends or .control
  while (1) {

     fields = m_spicefile[index].split(QRegExp("\\s+")); //Split line in strings

     data = fields[0].data(); //First string defines the type

     if (data[0] == 'x') { // subcircuit found, prepare parents name and nets
        qDebug()<<"subcircuit "<<fields[0]<<" found";

        QString aux_parent;
        QString aux_subcircuit;
        QStringList aux_parent_nets;
        int aux_size = fields.size();

        if (!isroot) {
            aux_parent = fields[0];
        } else {
            aux_parent = parent + "." + fields[0];
        }
        aux_subcircuit = fields[aux_size-1];

        int marker;

        for (int i=1; i < (aux_size-1); i++) {
            marker = 0;
            //Here check if the names are the same as in the parent nets and propagate
            QString aux_net = fields[i];
            for (int j=0; j < child_nets_size; j++) {
                int ispar = QString::compare(aux_net,child_nets[j]); //Compare if a port name in parent subckt is used to call a local subckt
                if(!ispar) {
                    aux_net = parent_nets[j]; //propagate the parent name!
                    marker = 1;
                    break;
                }
            }
            if (marker) {
                aux_parent_nets << aux_net;
            } else {
                if(!isroot) { //all root nets are available to the subckt
                    aux_parent_nets << aux_net;
                } else { //internal name is used
                    aux_parent_nets << parent + "." + aux_net;
                }
            }

        }

        //Save subckt as a device (useful if primitives are implemented as subcircuits as in PDKs)
        saveSpiceDev(aux_parent,aux_parent_nets);

        // Recursive call!
        identifyNets(aux_parent,aux_parent_nets,aux_subcircuit);
     }

     //Check always if child_nets_size != 0

     //Test for end of loop
     el1 = QString::compare(m_spicefile[index],".end");
     el2 = QString::compare(m_spicefile[index],".ends");
     el3 = QString::compare(m_spicefile[index],".control");
     if (!el1 || !el2 || !el3) break;

     index++;
  }




}

void XSchem::saveSpiceDev(QString name, QStringList nets)
{
    m_spicedev[m_spicedev_index].m_name = name;
    m_spicedev[m_spicedev_index].m_nets = nets;
    m_spicedev_index++;
}

void XSchem::printSpiceDev()
{
    for (int i=0; i<m_spicedev_index;i++) {
        qDebug()<<"Spice device : " << m_spicedev[i].m_name;
        for(int j = 0; j < m_spicedev[i].m_nets.size(); j++) {
            qDebug()<< m_spicedev[i].m_nets[j];
        }
    }
}
















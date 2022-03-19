#ifndef FDISK_H
#define FDISK_H
#include <QString>
#include <QtCore/qglobal.h>
#include "obmkdisk.h"
class fdisk
{
public:
    fdisk();
    int size;
    int sizeadd;
    QString unity;
    QString path;
    QString type;
    QString fit;
    QString name;
    QString type_of_delete;
    bool delete_partition = false;
    bool sizeNegativo = false;
    bool add = false;
    void eliminarDisco(fdisk *f_disco);
    void resultado_particion(fdisk *f_disco);
};

#endif // FDISK_H
